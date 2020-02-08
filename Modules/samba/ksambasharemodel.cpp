/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ksambasharemodel.h"

#include <KSambaShare>
#include <KLocalizedString>

KSambaShareModel::KSambaShareModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(KSambaShare::instance(), &KSambaShare::changed,
            this, &KSambaShareModel::reloadData);
    reloadData();
}

KSambaShareModel::~KSambaShareModel() = default;

int KSambaShareModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.size();
}

int KSambaShareModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(ColumnRole::ColumnCount);
}

QVariant KSambaShareModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {}; // we only have column headers.
    }

    Q_ASSERT(section < static_cast<int>(ColumnRole::ColumnCount));
    switch (static_cast<ColumnRole>(section)) {
    case ColumnRole::Name:
        return i18nc("@title:column samba share name", "Name");
    case ColumnRole::Path:
        return i18nc("@title:column samba share dir path", "Path");
    case ColumnRole::Comment:
        return i18nc("@title:column samba share text comment/description", "Comment");
    case ColumnRole::ColumnCount:
        break; // noop
    }

    return {};
}

QVariant KSambaShareModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    Q_ASSERT(index.row() < m_list.length());
    Q_ASSERT(index.column() < static_cast<int>(ColumnRole::ColumnCount));
    if (role == Qt::DisplayRole) {
        switch (static_cast<ColumnRole>(index.column())) {
        case ColumnRole::Name:
            return m_list.at(index.row()).name();
        case ColumnRole::Path:
            return m_list.at(index.row()).path();
        case ColumnRole::Comment:
            return m_list.at(index.row()).comment();
        case ColumnRole::ColumnCount:
            break; // noop
        }
    }

    return {};
}

void KSambaShareModel::reloadData()
{
    beginResetModel();
    m_list.clear();
    const auto samba = KSambaShare::instance();
    for (const auto &path : samba->sharedDirectories()) {
        m_list += samba->getSharesByPath(path);
    }
    endResetModel();
}

bool KSambaShareModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid() ? false : (rowCount(parent) > 0);
}
