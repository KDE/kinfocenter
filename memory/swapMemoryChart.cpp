/***************************************************************************
 *   KT list view item task implementation.                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "swapMemoryChart.h"

#include <klocale.h>
#include <kdebug.h>

#include "base.h"


SwapMemoryChart::SwapMemoryChart(QWidget* parent) :
	Chart(parent) {

	colorsInitialized = false;
}

void SwapMemoryChart::paintEvent(QPaintEvent* /*event*/) {

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
