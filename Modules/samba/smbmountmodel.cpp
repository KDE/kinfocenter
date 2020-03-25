/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "smbmountmodel.h"

#include <QIcon>
#include <KLocalizedString>
#include <Solid/Device>
#include <Solid/DeviceInterface>
#include <Solid/DeviceNotifier>
#include <Solid/NetworkShare>
#include <Solid/StorageAccess>

SmbMountModel::SmbMountModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded,
            this, &SmbMountModel::addDevice);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved,
            this, &SmbMountModel::removeDevice);
    reloadData();
}

SmbMountModel::~SmbMountModel() = default;

int SmbMountModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_devices.size();
}

int SmbMountModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(ColumnRole::ColumnCount);
}

QVariant SmbMountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {}; // we only have column headers.
    }

    Q_ASSERT(section <static_cast<int>(ColumnRole::ColumnCount));
    switch (static_cast<ColumnRole>(section)) {
    case ColumnRole::Share:
        return i18n("Resource"); // TODO: maybe reword for 5.19
    case ColumnRole::Path:
        return i18n("Mounted Under"); // TODO: maybe reword for 5.19
    case ColumnRole::Accessible:
        return i18nc("@title:column whether a samba share is accessible locally (i.e. mounted)", "Accessible");
    case ColumnRole::ColumnCount:
        break; // noop
    }

    return {};
}

QVariant SmbMountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    Q_ASSERT(index.row() < m_devices.count());
    Q_ASSERT(index.column() <static_cast<int>(ColumnRole::ColumnCount));

    if (role == Qt::DisplayRole) {
        switch (static_cast<ColumnRole>(index.column())) {
        case ColumnRole::Share:
            return m_devices.at(index.row()).as<Solid::NetworkShare>()->url();
        case ColumnRole::Path:
            return m_devices.at(index.row()).as<Solid::StorageAccess>()->filePath();
        case ColumnRole::Accessible:
            break; // Decoration only
        case ColumnRole::ColumnCount:
            break; // noop
        }
    }

    if (role == Qt::DecorationRole) {
        switch (static_cast<ColumnRole>(index.column())) {
        case ColumnRole::Accessible:
            // Using characters here since we use a QTableView and can't just
            return m_devices.at(index.row()).as<Solid::StorageAccess>()->isAccessible()
                    ? QIcon::fromTheme("dialog-ok")
                    : QIcon::fromTheme("dialog-cancel");
        case ColumnRole::Share:
        case ColumnRole::Path:
            break; // Display only
        case ColumnRole::ColumnCount:
            break; // noop
        }
    }

    return {};
}

void SmbMountModel::addDevice(const QString &udi)
{
    auto it = deviceForUdi(udi);
    if (it != m_devices.constEnd()) {
        return; // already in the list
    }

    Solid::Device device(udi);
    if (!device.is<Solid::NetworkShare>()) {
        return;
    }

    beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
    m_devices.append(device);
    endInsertRows();
}

void SmbMountModel::removeDevice(const QString &udi)
{
    auto it = deviceForUdi(udi);
    if (it == m_devices.constEnd()) {
        return; // untracked udi
    }

    const int index = static_cast<int>(std::distance(m_devices.constBegin(), it));
    beginRemoveRows(QModelIndex(), index, index);
    m_devices.removeAt(index);
    endRemoveRows();
}

void SmbMountModel::reloadData()
{
    beginResetModel();
    m_devices.clear();
    const auto devices = Solid::Device::listFromType(Solid::DeviceInterface::NetworkShare);
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        if (!it->is<Solid::NetworkShare>()) {
            // Workaround in case listFromType still gives incorrect types.
            // https://bugs.kde.org/show_bug.cgi?id=419220
            continue;
        }
        switch (it->as<Solid::NetworkShare>()->type()) {
        case Solid::NetworkShare::Cifs:
            m_devices.append(*it);
            continue;
        case Solid::NetworkShare::Nfs:
        case Solid::NetworkShare::Unknown:
            continue;
        }
    }
    endResetModel();
}

bool SmbMountModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid() ? false : (rowCount(parent) > 0);
}
