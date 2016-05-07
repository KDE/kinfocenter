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
#include <Solid/DeviceNotifier>

#include <QtQml>
#include <QQmlEngine>


BatteryModel::BatteryModel(QObject *parent) : QAbstractListModel(parent)
{
    qmlRegisterType<Solid::Battery>();

    m_batteries = Solid::Device::listFromType(Solid::DeviceInterface::Battery);

    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this, [this](const QString &udi) {

        auto it = std::find_if(m_batteries.constBegin(), m_batteries.constEnd(), [&udi](const Solid::Device &dev) {return dev.udi() == udi;});
        if (it != m_batteries.constEnd()) {
            return;
        }

        const Solid::Device device(udi);
        if (device.isValid() && device.is<Solid::Battery>()) {
            beginInsertRows(QModelIndex(), m_batteries.count(), m_batteries.count());
            m_batteries.append(device);
            endInsertRows();

            emit countChanged();
        }
    });
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this, [this](const QString &udi) {
        auto it = std::find_if(m_batteries.constBegin(), m_batteries.constEnd(), [&udi](const Solid::Device &dev) {return dev.udi() == udi;});
        if (it == m_batteries.constEnd()) {
            return;
        }

        int index = std::distance(m_batteries.constBegin(), it);


        beginRemoveRows(QModelIndex(), index, index);
        m_batteries.removeAt(index);
        endRemoveRows();

        emit countChanged();
    });
}

Solid::Battery *BatteryModel::get(int index) const
{
    if (index < 0 || index >= m_batteries.count()) {
        return nullptr;
    }

    Solid::Battery* battery = m_batteries.value(index).as<Solid::Battery>();

    QQmlEngine::setObjectOwnership(battery, QQmlEngine::CppOwnership);
    return battery;
}

QString BatteryModel::udi(int index) const
{
    if (index < 0 || index >= m_batteries.count()) {
        return QString();
    }

    return m_batteries.at(index).udi();
}

QVariant BatteryModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_batteries.count()) {
        return QVariant();
    }

    if (role == BatteryRole) {
        return QVariant::fromValue(m_batteries.value(index.row()).as<Solid::Battery>());
    } else if (role == UdiRole) {
        return m_batteries.at(index.row()).udi();
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
