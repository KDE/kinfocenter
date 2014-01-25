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

#include "totalMemoryChart.h"

#include <KLocalizedString>
#include "base.h"


TotalMemoryChart::TotalMemoryChart(QWidget* parent) :
	Chart(parent) {

	colorsInitialized = false;
}

void TotalMemoryChart::paintEvent(QPaintEvent* /*event*/) {
	// RAM + SWAP usage: 
	if (memoryInfos[SWAP_MEM] == NO_MEMORY_INFO || memoryInfos[FREESWAP_MEM] == NO_MEMORY_INFO)
		memoryInfos[SWAP_MEM] = memoryInfos[FREESWAP_MEM] = 0;

	t_memsize usedSwap = memoryInfos[SWAP_MEM] - memoryInfos[FREESWAP_MEM];
	t_memsize totalFree = memoryInfos[FREE_MEM] + memoryInfos[FREESWAP_MEM];
	t_memsize usedPhysical = (memoryInfos[TOTAL_MEM]+memoryInfos[SWAP_MEM])-usedSwap-totalFree;
	
	QList<t_memsize> used;
	
	used.append(totalFree);
	used.append(usedSwap);
	used.append(usedPhysical);
	
	if (!colorsInitialized) {
		colorsInitialized = true;
		texts.append(i18n("Total Free Memory"));
		colors.append(COLOR_FREE_MEMORY); // free ram+swap
		texts.append(i18n("Used Swap"));
		colors.append(COLOR_USED_SWAP); // used swap 
		texts.append(i18n("Used Physical Memory"));
		colors.append(COLOR_USED_MEMORY); // used ram
	}
	
	drawChart(memoryInfos[TOTAL_MEM] + memoryInfos[SWAP_MEM], used, colors, texts);

}
