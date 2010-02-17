/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 2008, Nicolas Ternisien <nicolas.ternisien@gmail.com>   *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef PHYSICAL_MEMORY_CHART_H
#define PHYSICAL_MEMORY_CHART_H

#include <QFrame>
#include <QList>

#include "chartWidget.h"

class QWidget;
class QPaintEvent;

class PhysicalMemoryChart : public Chart {
public:

	/**
	 * Initialize the list view item and task.
	 */
	PhysicalMemoryChart(QWidget* parent = NULL);
	

protected:
	virtual void paintEvent(QPaintEvent* event);
	
private:
	bool colorsInitialized;

	QList<QColor> colors;
	QList<QString> texts;

};

#endif // PHYSICAL_MEMORY_CHART_H
