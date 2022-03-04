/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kcm.h"

#include <KAboutData>
#include <KPluginFactory>
#include <QDebug>
#include <QStandardPaths>

#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardItemModel>
#include <QtQml>

#include <Solid/Battery>
#include <Solid/Device>
#include <Solid/DeviceNotifier>

#include <KLocalizedString>

#include "batterymodel.h"
#include "statisticsprovider.h"

K_PLUGIN_CLASS_WITH_JSON(KCMEnergyInfo, "kcm_energyinfo.json")

using namespace KQuickAddons;

KCMEnergyInfo::KCMEnergyInfo(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : ConfigModule(parent, data, args)
{
    qmlRegisterAnonymousType<BatteryModel>("org.kde.kinfocenter.energy.private", 1);
    qmlRegisterAnonymousType<WakeUpModel>("org.kde.kinfocenter.energy.private", 1);

    qmlRegisterType<StatisticsProvider>("org.kde.kinfocenter.energy.private", 1, 0, "HistoryModel");
    qmlRegisterUncreatableType<BatteryModel>("org.kde.kinfocenter.energy.private", 1, 0, "BatteryModel", QStringLiteral("Use BatteryModel"));

    KAboutData *about =
        new KAboutData(QStringLiteral("kcm_energyinfo"), i18n("Energy Consumption Statistics"), QStringLiteral("0.2"), QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Kai Uwe Broulik"), QString(), QStringLiteral("kde@privat.broulik.de"));
    setAboutData(about);

    m_batteries = new BatteryModel(this);

    m_wakeUps = new WakeUpModel(this);
}

#include "kcm.moc"
