/*
    SPDX-FileCopyrightText: 2010 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef TOTAL_MEMORY_CHART_H
#define TOTAL_MEMORY_CHART_H

#include <QList>

#include "chartWidget.h"

class QWidget;
class QPaintEvent;

class TotalMemoryChart : public Chart
{
public:
    /**
     * Initialize the list view item and task.
     */
    explicit TotalMemoryChart(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool colorsInitialized;

    QList<QColor> colors;
    QList<QString> texts;
};

#endif // TOTAL_MEMORY_CHART_H
