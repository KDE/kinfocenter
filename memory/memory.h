/*
 *  memory.h
 *
 *  Copyright (C) 2008 Nicolas Ternisien <nicolas.ternisien@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KCONTROL_MEMORY_H
#define KCONTROL_MEMORY_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

#include <kcmodule.h>
#include <kaboutdata.h>

#include "base.h"

class QGroupBox;
class QPushButton;
class ChartWidget;

class KCMMemory : public KCModule {
	Q_OBJECT

public:
	explicit KCMMemory(QWidget *parent = NULL, const QVariantList &list = QVariantList());
	~KCMMemory();

	QString quickHelp() const;
	
private slots:	
	void updateMemoryText();
	void updateMemoryGraphics();

	void updateDatas();
private:
	
	QGroupBox* initializeText();
	QGroupBox* initializeCharts();

	QTimer *timer;

	ChartWidget* totalMemory;
	ChartWidget* physicalMemory;
	ChartWidget* swapMemory;
	
	
	void fetchValues();

};

#endif

