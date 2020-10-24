/*
 *   KT list view item task implementation.
 *   SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "swapMemoryChart.h"

#include <KLocalizedString>

#include "base.h"

SwapMemoryChart::SwapMemoryChart(QWidget *parent)
    : Chart(parent)
{
    colorsInitialized = false;
}

void SwapMemoryChart::paintEvent(QPaintEvent * /*event*/)
{
    /* display graphical output (ram, hdd, at last: HDD+RAM) */
    /* be careful ! Maybe we have not all info available ! */

    // First check swap see bug 167608

    if (memoryInfos[SWAP_MEM] == Q_INT64_C(0))
        memoryInfos[SWAP_MEM] = NO_MEMORY_INFO;

    // SWAP usage:

    t_memsize freeSwap = ZERO_IF_NO_INFO(memoryInfos[FREESWAP_MEM]);
    t_memsize swap = ZERO_IF_NO_INFO(memoryInfos[SWAP_MEM]) - freeSwap;

    QList<t_memsize> used;

    used.append(freeSwap);
    used.append(swap);

    if (!colorsInitialized) {
        colorsInitialized = true;
        texts.append(i18n("Free Swap"));
        colors.append(COLOR_FREE_MEMORY); // free
        texts.append(i18n("Used Swap"));
        colors.append(COLOR_USED_SWAP); // used swap
    }

    drawChart(memoryInfos[SWAP_MEM], used, colors, texts);
}
