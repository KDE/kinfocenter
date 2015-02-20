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

#include "kcm.h"
#include "config-kcm.h"

#include <KPluginFactory>
#include <KPluginLoader>
#include <KAboutData>
#include <QDebug>
#include <QStandardPaths>
#include <QQuickWidget>
#include <QQuickView>

#include <QtQml>
#include <QQmlEngine>
#include <QQmlContext>
#include <QStandardItemModel>

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusMetaType> // qDBusRegisterMetaType
#include <QDBusPendingReply>

#include <Solid/DeviceNotifier>
#include <Solid/Device>
#include <Solid/Battery>

#include <KLocalizedString>
#include <Plasma/PluginLoader>

#include "batterymodel.h"

const QDBusArgument &operator<<(QDBusArgument &argument, const HistoryReply &data)
{
    argument.beginStructure();
    argument << data.time << data.value << data.charging;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, HistoryReply &attrs)
{
   arg.beginStructure();
   arg >> attrs.time >> attrs.value >> attrs.charging;
   arg.endStructure();
   return arg;
}

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

K_PLUGIN_FACTORY(KCMEnergyInfoFactory, registerPlugin<KCMEnergyInfo>();)

KCMEnergyInfo::KCMEnergyInfo(QWidget *parent, const QVariantList &args) : KCModuleQml(parent, args)
{
    //This flag seems to be needed in order for QQuickWidget to work
    //see https://bugreports.qt-project.org/browse/QTBUG-40765
    //also, it seems to work only if set in the kcm, not in the systemsettings' main
    qApp->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    qmlRegisterType<QStandardItemModel>();
    qmlRegisterType<BatteryModel>();

    qDBusRegisterMetaType<HistoryReply>();
    qDBusRegisterMetaType<QList<HistoryReply>>();
    qDBusRegisterMetaType<WakeUpReply>();
    qDBusRegisterMetaType<QList<WakeUpReply>>();

    KAboutData *about = new KAboutData("kcm_energyinfo", i18n("Energy Consumption Statistics"),
                                       "0.1", QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Kai Uwe Broulik"), QString(), "kde@privat.broulik.de");
    setAboutData(about);
    //setButtons(Help | Apply | Default);

    m_batteries = new BatteryModel(this);

    m_history = new QStandardItemModel(this);
    m_history->setItemRoleNames({
        {TimeRole, "time"},
        {ValueRole, "value"},
        {ChargingRole, "charging"}
    });

    m_wakeUps = new QStandardItemModel(this);
    m_wakeUps->setItemRoleNames({
        {WakeUpNameRole, "name"},
        {WakeUpPrettyNameRole, "prettyName"},
        {WakeUpIconNameRole, "iconName"},
        {WakeUpNumberRole, "number"},
        {WakeUpPercentRole, "percent"},
        {WakeUpUserSpaceRole, "userSpace"}
    });

    m_wakeUpsTimer = new QTimer(this);
    m_wakeUpsTimer->setInterval(5000);
    connect(m_wakeUpsTimer, &QTimer::timeout, this, &KCMEnergyInfo::updateWakeUps);

    setAutoUpdateWakeUps(true); // we start on the first page
}

void KCMEnergyInfo::setAutoUpdateWakeUps(bool autoUpdateWakeUps)
{
    if (m_autoUpdateWakeUps != autoUpdateWakeUps) {
        m_autoUpdateWakeUps = autoUpdateWakeUps;
        emit autoUpdateWakeUpsChanged();

        if (autoUpdateWakeUps) {
            m_wakeUpsTimer->start();
        } else {
            m_wakeUpsTimer->stop();
        }

        updateWakeUps();
    }
}

void KCMEnergyInfo::updateWakeUps()
{
    qDebug() << "UPDATE WAKEUPS";
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

        m_wakeUps->clear();

        auto values = reply.value();

        QHash<QString, WakeUpData> combinedData;
        qreal totalWakeUps = 0.0;

        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            if (!(*it).fromUserSpace) {
                continue;
            }
            const QString appName = (*it).cmdline.split(QLatin1Char(' ')).first(); // ugly
            if (!combinedData.contains(appName)) {
                combinedData[appName].name = appName;
                combinedData[appName].userSpace = (*it).fromUserSpace;
            }
            combinedData[appName].wakeUps += (*it).wakeUpsPerSecond;
            totalWakeUps += (*it).wakeUpsPerSecond;
        }

        for (auto it = combinedData.constBegin(); it != combinedData.constEnd(); ++it) {
            if (m_wakeUps->rowCount() >= 6) {
                break;
            }

            const qreal percent = (*it).wakeUps / (qreal)totalWakeUps * 100;
            if (percent > 0.5) { // ignore those millions of tiny wakeups
                combinedData[it.key()].percent = percent;

                const QString &name = (*it).name;

                QStandardItem *item = new QStandardItem();
                item->setData(name, WakeUpNameRole);

                KService::Ptr service = KService::serviceByStorageId(name + ".desktop");
                // TODO Cache this stuff
                if (service) {
                    item->setData(service->property("Name", QVariant::Invalid).toString(), WakeUpPrettyNameRole);
                    item->setData(service->icon(), WakeUpIconNameRole);
                    //m_applicationInfo.insert(name, qMakePair(*prettyName, *icon));
                } else {
                    item->setData(name, WakeUpPrettyNameRole);
                    item->setData(name.split(QLatin1Char('/'), QString::SkipEmptyParts).last().toLower(), WakeUpIconNameRole);
                }

                item->setData((*it).wakeUps, WakeUpNumberRole);
                item->setData((*it).percent, WakeUpPercentRole);
                item->setData((*it).userSpace, WakeUpUserSpaceRole);
                m_wakeUps->appendRow(item);
            }
        }

        m_wakeUpsCount = totalWakeUps;
        emit wakeUpsCountChanged();
    });
}

void KCMEnergyInfo::getHistory(const QString &udi, KCMEnergyInfo::HistoryType type, uint timespan, uint resolution)
{
    qDebug() << "get history" << udi <<type << timespan << resolution;
    auto msg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.UPower"),
                                              udi,
                                              QStringLiteral("org.freedesktop.UPower.Device"),
                                              QStringLiteral("GetHistory"));
    if (type == RateType) {
        msg << QLatin1String("rate");
    } else if (type == ChargeType) {
        msg << QLatin1String("charge");
    }

    msg << timespan << resolution;

    QDBusPendingReply<QList<HistoryReply>> reply = QDBusConnection::systemBus().asyncCall(msg);

    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        qDebug() << "got reply";
        QDBusPendingReply<QList<HistoryReply>> reply = *watcher;
        watcher->deleteLater();

        m_history->clear();

        if (reply.isError()) {
            qWarning() << "Failed to get device history from UPower";
            setHistoryAvailable(false);
            return;
        }

        auto values = reply.value();
        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            quint64 time = (*it).time;
            time *= (quint64)1000;
            const double &value = (*it).value;
            if (value == 0) {
                continue;
            }

            QStandardItem *item = new QStandardItem();
            item->setData(time, TimeRole);
            item->setData(value, ValueRole);
            if ((*it).charging) {
                item->setData(true, ChargingRole);
            } else {
                item->setData(false, ChargingRole);
            }
            m_history->appendRow(item);
        }
        qDebug() << "HAVE" << m_history->rowCount() << "entries";

        setHistoryAvailable(m_history->rowCount());
    });
}

void KCMEnergyInfo::setHistoryAvailable(bool historyAvailable)
{
    if (m_historyAvailable != historyAvailable) {
        m_historyAvailable = historyAvailable;
        emit historyAvailableChanged();
    }
}

#include "kcm.moc"
