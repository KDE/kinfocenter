/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 2008, Nicolas Ternisien <nicolas.ternisien@gmail.com>   *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef SWAP_MEMORY_CHART_H
#define SWAP_MEMORY_CHART_H

#include <QFrame>
#include <QList>

#include "chartWidget.h"

class QWidget;
class QPaintEvent;

class SwapMemoryChart : public Chart {
public:

	/**
	 * Initialize the list view item and task.
	 */
	SwapMemoryChart(QWidget* parent = NULL);
	

protected:
	virtual void paintEvent(QPaintEvent* event);
	
private:
	bool colorsInitialized;

	QList<QColor> colors;
	QList<QString> texts;

};

#endif // SWAP_MEMORY_CHART_H
