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

#ifndef KCM_ENERGYINFO_BATTERYMODEL_H
#define KCM_ENERGYINFO_BATTERYMODEL_H

#include <QAbstractListModel>
#include <QList>

#include <Solid/Device>
#include <Solid/Battery>

class BatteryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit BatteryModel(QObject *parent);
    virtual ~BatteryModel() = default;

    enum Roles {
        BatteryRole = Qt::UserRole,
        UdiRole
    };

    Q_INVOKABLE Solid::Battery *get(int index) const;
    Q_INVOKABLE QString udi(int index) const;

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

signals:
    void countChanged();

private:
    QList<Solid::Device> m_batteries;
};

#endif // KCM_ENERGYINFO_BATTERYMODEL_H
