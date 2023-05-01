/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kcm.h"

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

KCMEnergyInfo::KCMEnergyInfo(QObject *parent, const KPluginMetaData &data)
    : KQuickConfigModule(parent, data)
{
    qmlRegisterAnonymousType<BatteryModel>("org.kde.kinfocenter.energy.private", 1);

    qmlRegisterType<StatisticsProvider>("org.kde.kinfocenter.energy.private", 1, 0, "HistoryModel");
    qmlRegisterUncreatableType<BatteryModel>("org.kde.kinfocenter.energy.private", 1, 0, "BatteryModel", QStringLiteral("Use BatteryModel"));

    m_batteries = new BatteryModel(this);
}

#include "kcm.moc"
