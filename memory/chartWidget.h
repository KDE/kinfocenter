/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 2008, Nicolas Ternisien <nicolas.ternisien@gmail.com>   *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include <QWidget>
#include <QFrame>
#include <QString>
#include <QColor>
#include <QList>

class QLabel;
class QWidget;

#include "base.h"

class Chart : public QWidget {
public:
	Chart(QWidget* parent = NULL);
	
	void setMemoryInfos(t_memsize* memoryInfos);
	void setFreeMemoryLabel(QLabel* freeMemoryLabel);
	
	static QString formattedUnit(t_memsize value);
	
protected:
	
	bool drawChart(t_memsize total, const QList<t_memsize>& used, const QList<QColor>& colors, const QList<QString>& texts);
	
	t_memsize* memoryInfos;
	
	QLabel* freeMemoryLabel;
	
};

class ChartWidget : public QWidget {
public:

	/**
	 * Initialize the list view item and task.
	 */
	ChartWidget(const QString& title, const QString& hint, Chart* chartImplementation, QWidget* parent = NULL);
	
	void setMemoryInfos(t_memsize* memoryInfos);
	void refresh();
	
private:
	
	QLabel* titleLabel;
	
	Chart* chart;
	
	QLabel* freeMemoryLabel;
	
};

#endif // CHART_WIDGET_H
