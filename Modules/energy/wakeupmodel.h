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
#include <QTime>

class WakeUpData {
public:
    uint pid = 0;
    QString name;
    QString prettyName;
    QString iconName;
    qreal wakeUps = 0.0;
    qreal percent = 0.0;
    bool userSpace = false;
    QString details;
};

class WakeUpModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(qreal total READ total NOTIFY totalChanged)

public:
    explicit WakeUpModel(QObject *parent);
    virtual ~WakeUpModel() = default;

    enum Roles {
        PidRole = Qt::UserRole,
        NameRole,
        PrettyNameRole = Qt::DisplayRole,
        IconNameRole = Qt::DecorationRole,
        WakeUpsRole = Qt::UserRole + 2,
        PercentRole,
        UserSpaceRole,
        DetailsRole,
    };

    qreal total() const { return m_total; }

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void reload();

signals:
    void countChanged();
    void totalChanged();

private:
    QList<WakeUpData> m_data;
    QHash<QString, WakeUpData> m_combinedData;
    qreal m_total = 0.0;
    QHash<QString, QPair<QString, QString>> m_applicationInfo;
    QTime m_lastReload;

};

#endif // KCM_ENERGYINFO_WAKEUPMODEL_H
