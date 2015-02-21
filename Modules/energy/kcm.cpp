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
#include "wakeupmodel.h"

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

K_PLUGIN_FACTORY(KCMEnergyInfoFactory, registerPlugin<KCMEnergyInfo>();)

KCMEnergyInfo::KCMEnergyInfo(QWidget *parent, const QVariantList &args) : KCModuleQml(parent, args)
{
    //This flag seems to be needed in order for QQuickWidget to work
    //see https://bugreports.qt-project.org/browse/QTBUG-40765
    //also, it seems to work only if set in the kcm, not in the systemsettings' main
    qApp->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    qmlRegisterType<QStandardItemModel>();
    qmlRegisterType<BatteryModel>();
    qmlRegisterType<WakeUpModel>();

    qDBusRegisterMetaType<HistoryReply>();
    qDBusRegisterMetaType<QList<HistoryReply>>();

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

    m_wakeUps = new WakeUpModel(this);
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
