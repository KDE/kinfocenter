/*
    SPDX-FileCopyrightText: 2010 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
