
/*
Copyright 2010  Nicolas Ternisien <nicolas.ternisien@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include <QColor>
#include <QList>
#include <QString>
#include <QWidget>

class QLabel;
class QWidget;

#include "base.h"

class Chart : public QWidget
{
    Q_OBJECT
public:
    explicit Chart(QWidget *parent = nullptr);

    void setMemoryInfos(t_memsize *memoryInfos);
    void setFreeMemoryLabel(QLabel *freeMemoryLabel);

    static QString formattedUnit(t_memsize value);

protected:
    bool drawChart(t_memsize total, const QList<t_memsize> &used, const QList<QColor> &colors, const QList<QString> &texts);

    t_memsize *memoryInfos = nullptr;

    QLabel *mFreeMemoryLabel = nullptr;
};

class ChartWidget : public QWidget
{
public:
    /**
     * Initialize the list view item and task.
     */
    ChartWidget(const QString &title, const QString &hint, Chart *chartImplementation, QWidget *parent = nullptr);

    void setMemoryInfos(t_memsize *memoryInfos);
    void refresh();

private:
    QLabel *titleLabel = nullptr;

    Chart *chart = nullptr;

    QLabel *mFreeMemoryLabel = nullptr;
};

#endif // CHART_WIDGET_H
