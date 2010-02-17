/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 2008, Nicolas Ternisien <nicolas.ternisien@gmail.com>   *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef TOTAL_MEMORY_CHART_H
#define TOTAL_MEMORY_CHART_H

#include <QFrame>
#include <QList>

#include "chartWidget.h"

class QWidget;
class QPaintEvent;

class TotalMemoryChart : public Chart {
public:

	/**
	 * Initialize the list view item and task.
	 */
	TotalMemoryChart(QWidget* parent = NULL);
	

protected:
	virtual void paintEvent(QPaintEvent* event);
	
private:
	bool colorsInitialized;

	QList<QColor> colors;
	QList<QString> texts;

};

#endif // TOTAL_MEMORY_CHART_H
