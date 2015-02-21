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

#ifndef KCM_ENERGYINFO_WAKEUPMODEL_H
#define KCM_ENERGYINFO_WAKEUPMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QPair>

class WakeUpData {
public:
    QString name;
    QString prettyName;
    QString iconName;
    qreal wakeUps = 0.0;
    qreal percent = 0.0;
    bool userSpace = false;
};

class WakeUpModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit WakeUpModel(QObject *parent);
    virtual ~WakeUpModel() = default;

    enum Roles {
        NameRole = Qt::UserRole,
        PrettyNameRole = Qt::DisplayRole,
        IconNameRole = Qt::DecorationRole,
        WakeUpsRole = Qt::UserRole + 1,
        PercentRole,
        UserSpaceRole
    };

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void reload();

signals:
    void countChanged();

private:
    QList<WakeUpData> m_data;
    QHash<QString, QPair<QString, QString>> m_applicationInfo;

};

#endif // KCM_ENERGYINFO_WAKEUPMODEL_H
