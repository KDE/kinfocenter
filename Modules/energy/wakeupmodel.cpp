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

#include "wakeupmodel.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusMetaType>

#include <KService>

class WakeUpReply {
public:
    bool fromUserSpace = false;
    quint32 id = 0;
    double wakeUpsPerSecond = 0.0;
    QString cmdline;
    QString details;
};

Q_DECLARE_METATYPE(WakeUpReply)

const QDBusArgument &operator<<(QDBusArgument &argument, const WakeUpReply &data)
{
    argument.beginStructure();
    argument << data.fromUserSpace << data.id << data.wakeUpsPerSecond << data.cmdline << data.details;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, WakeUpReply &data)
{
    argument.beginStructure();
    argument >> data.fromUserSpace >> data.id >> data.wakeUpsPerSecond >> data.cmdline >> data.details;
    argument.endStructure();
    return argument;
}

static const int s_maximumEntries = 6;

WakeUpModel::WakeUpModel(QObject *parent) : QAbstractListModel(parent)
{
    qDBusRegisterMetaType<WakeUpReply>();
    qDBusRegisterMetaType<QList<WakeUpReply>>();

    if (!QDBusConnection::systemBus().connect(QStringLiteral("org.freedesktop.UPower"),
                                               QStringLiteral("/org/freedesktop/UPower/Wakeups"),
                                               QStringLiteral("org.freedesktop.UPower.Wakeups"),
                                               QStringLiteral("DataChanged"), this,
                                               SLOT(reload()))) {
        qDebug() << "Error connecting to wakeup data changes via dbus";
    }

    reload();
}

void WakeUpModel::reload()
{
    QDBusPendingReply<QList<WakeUpReply>> reply = QDBusConnection::systemBus().asyncCall(
        QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.UPower"),
                                       QStringLiteral("/org/freedesktop/UPower/Wakeups"),
                                       QStringLiteral("org.freedesktop.UPower.Wakeups"),
                                       QStringLiteral("GetData"))
    );

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QList<WakeUpReply>> reply = *watcher;
        watcher->deleteLater();

        if (reply.isError()) {
            qWarning() << "Failed to get wakeup statistics from UPower";
            return;
        }

        const bool resetModel = m_data.isEmpty();

        if (resetModel) {
            beginResetModel();
        }

        m_data.clear();

        auto values = reply.value();

        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            if (!(*it).fromUserSpace) {
                continue;
            }
            const QString appName = (*it).cmdline.split(QLatin1Char(' ')).first(); // ugly
            if (!m_combinedData.contains(appName)) {
                m_combinedData[appName].name = appName;
                m_combinedData[appName].userSpace = (*it).fromUserSpace;
            }
            m_combinedData[appName].wakeUps += (*it).wakeUpsPerSecond;
            m_total += (*it).wakeUpsPerSecond;
        }

        for (auto it = m_combinedData.constBegin(); it != m_combinedData.constEnd(); ++it) {
            if (m_data.count() >= s_maximumEntries) {
                break;
            }

            const qreal percent = (*it).wakeUps / m_total * 100;
            if (percent > 0.5) { // ignore those millions of tiny wakeups
                const QString &name = (*it).name;

                WakeUpData item;
                item.name = name;
                item.prettyName = name;

                auto existingService = m_applicationInfo.find(name);
                if (existingService != m_applicationInfo.end()) {
                    item.prettyName = (*existingService).first;
                    item.iconName = (*existingService).second;
                } else {
                    KService::Ptr service = KService::serviceByStorageId(name + ".desktop");
                    if (service) {
                        item.prettyName = service->property("Name", QVariant::Invalid).toString();
                        item.iconName = service->icon();

                        m_applicationInfo.insert(name, qMakePair(item.prettyName, item.iconName));
                    } else {
                        // use the app name as fallback icon
                        item.iconName = name.split(QLatin1Char('/'), QString::SkipEmptyParts).last().toLower();
                    }
                }

                item.wakeUps = (*it).wakeUps;
                item.userSpace = (*it).userSpace;

                m_data.append(item);
            }
        }

        std::sort(m_data.begin(), m_data.end(), [](const WakeUpData &a, const WakeUpData &b) {
            return a.wakeUps > b.wakeUps;
        });

        emit totalChanged();
        emit countChanged();

        if (resetModel) {
            endResetModel();
        } else {
            emit dataChanged(index(0, 0), index(s_maximumEntries - 1, 0));
        }
    });
}

QVariant WakeUpModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_data.count() || index.row() >= s_maximumEntries) {
        return QVariant();
    }

    switch(role) {
    case NameRole:
        return m_data.at(index.row()).name;
    case PrettyNameRole:
        return m_data.at(index.row()).prettyName;
    case IconNameRole:
        return m_data.at(index.row()).iconName;
    case WakeUpsRole:
        return m_data.at(index.row()).wakeUps;
    case PercentRole:
        return m_data.at(index.row()).percent;
    case UserSpaceRole:
        return m_data.at(index.row()).userSpace;
    }

    return QVariant();
}

int WakeUpModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return qMin(m_data.count(), s_maximumEntries);
}

QHash<int, QByteArray> WakeUpModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PrettyNameRole, "prettyName"},
        {IconNameRole, "iconName"},
        {WakeUpsRole, "wakeUps"},
        {PercentRole, "percent"},
        {UserSpaceRole, "userSpace"}
    };
}
