/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCM_ENERGYINFO_H
#define KCM_ENERGYINFO_H

#include <KQuickAddons/ConfigModule>

class BatteryModel;
class WakeUpModel;

class KCMEnergyInfo : public KQuickAddons::ConfigModule
{
    Q_OBJECT

    Q_PROPERTY(BatteryModel *batteries READ batteries CONSTANT)

    Q_PROPERTY(WakeUpModel *wakeUps READ wakeUps CONSTANT)

public:
    explicit KCMEnergyInfo(QObject *parent, const QVariantList &args);
    virtual ~KCMEnergyInfo() = default;

    BatteryModel *batteries() const
    {
        return m_batteries;
    }

    WakeUpModel *wakeUps() const
    {
        return m_wakeUps;
    }

private:
    BatteryModel *m_batteries = nullptr;

    WakeUpModel *m_wakeUps = nullptr;

    int m_wakeUpsCount = 0;
};

Q_DECLARE_METATYPE(QList<QPointF>)

#endif // KCM_ENERGYINFO_H
