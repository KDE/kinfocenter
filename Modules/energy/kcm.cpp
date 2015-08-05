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

#include <QtQml>
#include <QQmlEngine>
#include <QQmlContext>
#include <QStandardItemModel>

#include <Solid/DeviceNotifier>
#include <Solid/Device>
#include <Solid/Battery>

#include <KLocalizedString>

#include "batterymodel.h"
#include "wakeupmodel.h"
#include "statisticsprovider.h"


K_PLUGIN_FACTORY_WITH_JSON(KCMLookandFeelFactory, "kcm_energyinfo.json", registerPlugin<KCMEnergyInfo>();)

using namespace KQuickAddons;

KCMEnergyInfo::KCMEnergyInfo(QObject *parent, const QVariantList &args) : ConfigModule(parent, args)
{
    //This flag seems to be needed in order for QQuickWidget to work
    //see https://bugreports.qt-project.org/browse/QTBUG-40765
    //also, it seems to work only if set in the kcm, not in the systemsettings' main
    qApp->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    qmlRegisterType<BatteryModel>();
    qmlRegisterType<WakeUpModel>();
    qmlRegisterType<StatisticsProvider>("org.kde.kinfocenter.energy.private", 1, 0, "HistoryModel");

    KAboutData *about = new KAboutData("kcm_energyinfo", i18n("Energy Consumption Statistics"),
                                       "0.1", QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Kai Uwe Broulik"), QString(), "kde@privat.broulik.de");
    setAboutData(about);

    m_batteries = new BatteryModel(this);

    m_wakeUps = new WakeUpModel(this);
}


#include "kcm.moc"
