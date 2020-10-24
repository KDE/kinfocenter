/*
    SPDX-FileCopyrightText: 2010 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PHYSICAL_MEMORY_CHART_H
#define PHYSICAL_MEMORY_CHART_H

#include <QList>

#include "chartWidget.h"

class QWidget;
class QPaintEvent;

class PhysicalMemoryChart : public Chart
{
public:
    /**
     * Initialize the list view item and task.
     */
    PhysicalMemoryChart(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool colorsInitialized;

    QList<QColor> colors;
    QList<QString> texts;
};

#endif // PHYSICAL_MEMORY_CHART_H
