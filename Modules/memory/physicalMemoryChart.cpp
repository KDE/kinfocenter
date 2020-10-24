/*
 *   KT list view item task implementation.
 *   SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "physicalMemoryChart.h"

#include <KLocalizedString>

#include "base.h"

PhysicalMemoryChart::PhysicalMemoryChart(QWidget *parent)
    : Chart(parent)
{
    colorsInitialized = false;
}

void PhysicalMemoryChart::paintEvent(QPaintEvent * /*event*/)
{
    /* RAM usage: */
    /* don't rely on the SHARED_MEM value since it may refer to
     * the size of the System V sharedmem in 2.4.x. Calculate instead! */

    t_memsize bufferMemory;

    bufferMemory = 0;
    t_memsize cachedMemory = ZERO_IF_NO_INFO(memoryInfos[CACHED_MEM]);
    t_memsize freeMemory = ZERO_IF_NO_INFO(memoryInfos[FREE_MEM]);
    t_memsize totalMemory = ZERO_IF_NO_INFO(memoryInfos[TOTAL_MEM]) - bufferMemory - cachedMemory - freeMemory;

    QList<t_memsize> used;
    used.append(freeMemory);
    used.append(cachedMemory);
    used.append(bufferMemory);
    used.append(totalMemory);

    if (!colorsInitialized) {
        colorsInitialized = true;
        texts.append(i18n("Free Physical Memory"));
        colors.append(COLOR_FREE_MEMORY); // free
        texts.append(i18n("Disk Cache"));
        colors.append(QColor(88, 176, 36)); // cached
        texts.append(i18n("Disk Buffers"));
        colors.append(QColor(118, 237, 49)); // buffer
        texts.append(i18n("Application Data"));
        colors.append(COLOR_USED_MEMORY); // used+shared
    }

    drawChart(memoryInfos[TOTAL_MEM], used, colors, texts);
}
