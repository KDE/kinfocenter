/***************************************************************************
 *   Copyright (C) 2015 Kai Uwe Broulik <kde@privat.broulik.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "batterymodel.h"

#include <Solid/Device>
#include <Solid/DeviceNotifier>

#include <QtQml>
#include <QQmlEngine>


BatteryModel::BatteryModel(QObject *parent) : QAbstractListModel(parent)
{
    qmlRegisterType<Solid::Battery>();

    const auto devices = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    foreach (Solid::Device device, devices) {
        auto battery = device.as<Solid::Battery>();
        m_batteries.append(battery);
        // FIXME Fix Solid so it exposes the base device interface properties too
        m_batteriesUdi.append(device.udi());
    }

    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this, [this](const QString &udi) {
        if (m_batteriesUdi.contains(udi)) {
            // we already know that one
            return;
        }

        const Solid::Device device(udi);
        if (device.isValid()) {
            auto *battery = const_cast<Solid::Battery *>(device.as<Solid::Battery>());
            if (battery) {
                beginInsertRows(QModelIndex(), m_batteries.count(), m_batteries.count());
                m_batteries.append(battery);
                // FIXME Fix Solid so it exposes the base device interface properties too
                m_batteriesUdi.append(device.udi());
                endInsertRows();

                emit countChanged();
            }
        }
    });
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this, [this](const QString &udi) {
        const int index = m_batteriesUdi.indexOf(udi);
        if (index < 0) {
            // we don't have that one
            return;
        }

        beginRemoveRows(QModelIndex(), index, index);
        m_batteries.removeAt(index);
        m_batteriesUdi.removeAt(index);
        endRemoveRows();

        emit countChanged();
    });
}

Solid::Battery *BatteryModel::get(int index) const
{
    if (index < 0 || index >= m_batteries.count()) {
        return nullptr;
    }

    auto battery = m_batteries.at(index);
    QQmlEngine::setObjectOwnership(battery, QQmlEngine::CppOwnership);
    return battery;
}

QString BatteryModel::udi(int index) const
{
    if (index < 0 || index >= m_batteriesUdi.count()) {
        return QString();
    }

    return m_batteriesUdi.at(index);
}

QVariant BatteryModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_batteries.count()) {
        return QVariant();
    }

    if (role == BatteryRole) {
        return QVariant::fromValue(m_batteries.at(index.row()));
    } else if (role == UdiRole) {
        return m_batteriesUdi.at(index.row());
    }

    return QVariant();
}

int BatteryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_batteries.count();
}

QHash<int, QByteArray> BatteryModel::roleNames() const
{
    return {
        {BatteryRole, "battery"},
        {UdiRole, "udi"}
    };
}
