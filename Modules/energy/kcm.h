/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCM_ENERGYINFO_H
#define KCM_ENERGYINFO_H

#include <KQuickConfigModule>
class BatteryModel;

class KCMEnergyInfo : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(BatteryModel *batteries READ batteries CONSTANT)

public:
    explicit KCMEnergyInfo(QObject *parent, const KPluginMetaData &data);
    ~KCMEnergyInfo() override = default;

    BatteryModel *batteries() const
    {
        return m_batteries;
    }

private:
    BatteryModel *m_batteries = nullptr;

};

Q_DECLARE_METATYPE(QList<QPointF>)

#endif // KCM_ENERGYINFO_H
