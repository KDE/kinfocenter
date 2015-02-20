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

#include "batterymodel.h"

#include <Solid/Device>

#include <QtQml>

BatteryModel::BatteryModel(QObject *parent) : QAbstractListModel(parent)
{
    qmlRegisterType<Solid::Battery>();

    const auto devices = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    foreach (const Solid::Device &device, devices) {
        auto* battery = const_cast<Solid::Battery *>(device.as<Solid::Battery>());

        battery->setParent(this);
        //QQmlEngine::setObjectOwnership(battery, QQmlEngine::CppOwnership);
        m_batteries.append(battery);
        // FIXME Fix Solid so it exposes the base device interface properties too
        m_batteriesUdi.append(device.udi());
    }
}

BatteryModel::~BatteryModel()
{
    //qDeleteAll(m_batteries);
}

QVariant BatteryModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_batteries.count()) {
        return QVariant();
    }

    if (role == BatteryRole) {
        return QVariant::fromValue(m_batteries.at(index.row()));
    } else if (role == UdiRole) {
        return m_batteriesUdi.at(index.row());
    }

    return QVariant();
}

int BatteryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_batteries.count();
}

QHash<int, QByteArray> BatteryModel::roleNames() const
{
    return {
        {BatteryRole, "battery"},
        {UdiRole, "udi"}
    };
}
