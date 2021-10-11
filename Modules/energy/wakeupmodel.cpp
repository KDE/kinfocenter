/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "wakeupmodel.h"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDebug>

#include <KService>

class WakeUpReply
{
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

static const int s_maximumEntries = 10;

WakeUpModel::WakeUpModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qDBusRegisterMetaType<WakeUpReply>();
    qDBusRegisterMetaType<QList<WakeUpReply>>();

    if (!QDBusConnection::systemBus().connect(QStringLiteral("org.freedesktop.UPower"),
                                              QStringLiteral("/org/freedesktop/UPower/Wakeups"),
                                              QStringLiteral("org.freedesktop.UPower.Wakeups"),
                                              QStringLiteral("DataChanged"),
                                              this,
                                              SLOT(reload()))) {
        qDebug() << "Error connecting to wakeup data changes via dbus";
    }

    reload();
}

void WakeUpModel::reload()
{
    // It is possible that DataChanged is emitted very quickly, up to the point
    // that upower won't be able to respond, resulting in errors due to maximum number
    // of pending replies.
    // One update per second should be enough.
    if (!m_lastReload.isNull() && m_lastReload.secsTo(QTime::currentTime()) < 1) {
        return;
    }

    m_lastReload = QTime::currentTime();

    QDBusPendingReply<QList<WakeUpReply>> reply =
        QDBusConnection::systemBus().asyncCall(QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.UPower"),
                                                                              QStringLiteral("/org/freedesktop/UPower/Wakeups"),
                                                                              QStringLiteral("org.freedesktop.UPower.Wakeups"),
                                                                              QStringLiteral("GetData")));

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

        m_total = 0.0;
        m_combinedData.clear();
        m_data.clear();

        auto values = reply.value();

        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            if (!(*it).fromUserSpace) {
                continue;
            }
            const QString appName = (*it).cmdline.split(QLatin1Char(' '), Qt::SkipEmptyParts).first(); // ugly
            if (!m_combinedData.contains(appName)) {
                m_combinedData[appName].name = appName;
                m_combinedData[appName].userSpace = (*it).fromUserSpace;
            }
            m_combinedData[appName].wakeUps += (*it).wakeUpsPerSecond;
            if ((*it).id) {
                m_combinedData[appName].pid = (*it).id;
            }
            if (!(*it).details.isEmpty()) {
                m_combinedData[appName].details = (*it).details;
            }
            m_total += (*it).wakeUpsPerSecond;
        }

        for (auto it = m_combinedData.begin(); it != m_combinedData.end(); ++it) {
            const QString &name = (*it).name;

            auto existingService = m_applicationInfo.find(name);
            if (existingService != m_applicationInfo.end()) {
                (*it).prettyName = (*existingService).first;
                (*it).iconName = (*existingService).second;
            } else {
                KService::Ptr service = KService::serviceByStorageId(name + ".desktop");
                if (service) {
                    it->prettyName = service->property(QStringLiteral("Name"), QVariant::Invalid).toString();
                    it->iconName = service->icon();

                    m_applicationInfo.insert(name, qMakePair((*it).prettyName, (*it).iconName));
                } else {
                    // use the app name as fallback icon
                    (*it).iconName = name.split(QLatin1Char('/'), Qt::SkipEmptyParts).last().toLower();
                }
            }

            m_data.append((*it));
        }

        m_combinedData.clear();

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

    switch (role) {
    case PidRole:
        return m_data.at(index.row()).pid;
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
    case DetailsRole:
        return m_data.at(index.row()).details;
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
    return {{PidRole, "pid"},
            {NameRole, "name"},
            {PrettyNameRole, "prettyName"},
            {IconNameRole, "iconName"},
            {WakeUpsRole, "wakeUps"},
            {PercentRole, "percent"},
            {UserSpaceRole, "userSpace"},
            {DetailsRole, "details"}};
}
