/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2015  David Edmundson <david@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef STATISTICSPROVIDER_H
#define STATISTICSPROVIDER_H

#include <QObject>
#include <QQmlParserStatus>
#include <QPointF>

struct HistoryReply {
public:
    uint time = 0;
    double value = 0.0;
    uint charging = 0;
};

Q_DECLARE_METATYPE(HistoryReply)

class StatisticsProvider : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_ENUMS(HistoryType)

    Q_PROPERTY(QString device MEMBER m_device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(uint duration MEMBER m_duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(HistoryType type MEMBER m_type WRITE setType NOTIFY typeChanged)

    Q_PROPERTY(QVariantList points READ asPoints NOTIFY dataChanged)
    Q_PROPERTY(int count READ count NOTIFY dataChanged)
    Q_PROPERTY(int firstDataPointTime READ firstDataPointTime NOTIFY dataChanged)
    Q_PROPERTY(int lastDataPointTime READ lastDataPointTime NOTIFY dataChanged)

public:
    enum HistoryType {
        RateType,
        ChargeType
    };

    enum HistoryRoles {
        TimeRole = Qt::UserRole + 1,
        ValueRole,
        ChargingRole
    };

    StatisticsProvider(QObject *parent = Q_NULLPTR);

    void setDevice(const QString &device);
    void setDuration(int duration);
    void setType(HistoryType type);

    void load();

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

    QVariantList asPoints() const;
    int count() const;

    int firstDataPointTime();
    int lastDataPointTime();

Q_SIGNALS:
    void deviceChanged();
    void typeChanged();
    void durationChanged();

    void dataChanged();

public Q_SLOTS:
    void refresh();

private:
    QString m_device;
    HistoryType m_type;
    uint m_duration; //in seconds

    QList<HistoryReply> m_values;
    bool m_isComplete = false;
};

#endif // STATISTICSPROVIDER_H
