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


class KCMEnergyInfo : public KCModuleQml
{
    Q_OBJECT

    Q_PROPERTY(BatteryModel *batteries READ batteries CONSTANT)

    Q_PROPERTY(QObject *history READ history CONSTANT)

    Q_PROPERTY(WakeUpModel *wakeUps READ wakeUps CONSTANT)
    Q_PROPERTY(WakeUpModel *wakeUps READ wakeUps CONSTANT)


public:
    explicit KCMEnergyInfo(QWidget *parent, const QVariantList &args);
    virtual ~KCMEnergyInfo() = default;

    enum BatteryRoles {
        BatteryRole = Qt::UserRole + 1,
        TestRole
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

    QObject *history() const { return m_history; }

    WakeUpModel *wakeUps() const { return m_wakeUps; }


private:

    BatteryModel *m_batteries = nullptr;

    QObject *m_history = nullptr;

    WakeUpModel *m_wakeUps = nullptr;

    int m_wakeUpsCount = 0;

};

Q_DECLARE_METATYPE(QList<QPointF>)

#endif // KCM_ENERGYINFO_H
