
/*
Copyright 2010  Nicolas Ternisien <nicolas.ternisien@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KCONTROL_MEMORY_H
#define KCONTROL_MEMORY_H

#include <QWidget>
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

	QString quickHelp() const Q_DECL_OVERRIDE;
	
private Q_SLOTS:
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

