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

#ifndef KCM_ENERGYINFO_H
#define KCM_ENERGYINFO_H

#include <KCModuleQml>

class QQuickView;
class QStandardItemModel;

class BatteryModel;
class WakeUpModel;

class HistoryReply {
public:
    uint time = 0;
    double value = 0.0;
    uint charging = 0;
};
Q_DECLARE_METATYPE(HistoryReply)

class KCMEnergyInfo : public KCModuleQml
{
    Q_OBJECT
    Q_ENUMS(HistoryType)

    Q_PROPERTY(BatteryModel *batteries READ batteries CONSTANT)

    Q_PROPERTY(QStandardItemModel *history READ history CONSTANT)
    Q_PROPERTY(bool historyAvailable READ historyAvailable NOTIFY historyAvailableChanged)

    Q_PROPERTY(WakeUpModel *wakeUps READ wakeUps CONSTANT)

public:
    explicit KCMEnergyInfo(QWidget *parent, const QVariantList &args);
    virtual ~KCMEnergyInfo() = default;

    enum HistoryType {
        RateType,
        ChargeType
    };

    enum BatteryRoles {
        BatteryRole = Qt::UserRole + 1,
        TestRole
    };

    enum HistoryRoles {
        TimeRole = Qt::UserRole + 1,
        ValueRole,
        ChargingRole
    };

    enum WakeUpRoles {
        WakeUpNameRole = Qt::UserRole + 1,
        WakeUpPrettyNameRole = Qt::DisplayRole,
        WakeUpIconNameRole = Qt::DecorationRole,
        WakeUpNumberRole = Qt::UserRole,
        WakeUpPercentRole,
        WakeUpUserSpaceRole
    };

    BatteryModel *batteries() const { return m_batteries; }

    QStandardItemModel *history() const { return m_history; }
    bool historyAvailable() const { return m_historyAvailable; }

    WakeUpModel *wakeUps() const { return m_wakeUps; }

public slots:
    void getHistory(const QString &udi, HistoryType type, uint timespan, uint resolution);

signals:
    void historyAvailableChanged();

private:
    void setHistoryAvailable(bool historyAvailable);

    BatteryModel *m_batteries = nullptr;

    QStandardItemModel *m_history = nullptr;
    bool m_historyAvailable = false;

    WakeUpModel *m_wakeUps = nullptr;

    int m_wakeUpsCount = 0;

};

#endif // KCM_ENERGYINFO_H
