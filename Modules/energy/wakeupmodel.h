/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCM_ENERGYINFO_WAKEUPMODEL_H
#define KCM_ENERGYINFO_WAKEUPMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QPair>
#include <QTime>

class WakeUpData
{
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
    ~WakeUpModel() override = default;

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

    qreal total() const
    {
        return m_total;
    }

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
