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

#include "chartWidget.h"

#include <QVBoxLayout>
#include <QLinearGradient>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QColor>

#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>

Chart::Chart(QWidget* parent) :
	QWidget(parent) {
	
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	
	memoryInfos = NULL;
	freeMemoryLabel = NULL;
}


void Chart::setMemoryInfos(t_memsize* memoryInfos) {
	this->memoryInfos = memoryInfos;
}

void Chart::setFreeMemoryLabel(QLabel* freeMemoryLabel) {
	this->freeMemoryLabel = freeMemoryLabel;
}


/* Graphical Memory Display */
bool Chart::drawChart(t_memsize total, const QList<t_memsize>& used, const QList<QColor>& colors, const QList<QString>& texts) {
	
	QPainter paint(this);

	QPen pen(QColor(0, 0, 0));
	paint.setPen(pen);

	if (total == NO_MEMORY_INFO) {
		paint.fillRect(1, 1, width() - 2, height() - 2, QBrush(QColor(128, 128, 128)));
		paint.setPen(pen);
		paint.drawRect(rect());
		freeMemoryLabel->setText(i18n("Not available."));

		return false;
	}

	int startline = height()-2;
	
	int percent, localheight;
	t_memsize last_used = 0;

	for (int count = used.size()-1; count >=0; --count) {
		last_used = used.at(count);
		QColor color = colors.at(count);
		QString text = texts.at(count);

		percent = (((qint64)last_used) * 100) / total;
		
		if (count)
			localheight = ((height()-2) * percent) / 100;
		else
			localheight = startline;

		//kDebug() << "Count : " <<  count << " Percent : " << percent << "%" << " Localheight:" << localheight << endl;
		
		if (localheight>0) {
			QLinearGradient gradient(QPointF(1, startline), QPointF(width()-2, -localheight));

			QColor endProgressColor(0xFF, 0xFF, 0xFF, 100);
			gradient.setColorAt(0, color);
			gradient.setColorAt(1, endProgressColor);
			paint.fillRect(1, startline, width()-2, -localheight, gradient);

			//paint.fillRect(1, startline, width()-2, -localheight, color);

			if (localheight >= SPACING) {
				paint.drawText(0, startline-localheight, width(), localheight, Qt::AlignCenter | Qt::TextWordWrap, QString("%1 %2%").arg(text).arg(percent));
			}
		}

		startline -= localheight;

	}

	// draw surrounding box
	QRect r = rect();
	qDrawShadePanel(&paint, r.x(), r.y(), r.width(), r.height(), palette(), true, 1);

	freeMemoryLabel->setText(i18n("%1 free", formattedUnit(last_used)));

	
	return true;
}


QString Chart::formattedUnit(t_memsize value) {
	const KLocale* locale = KGlobal::locale();
	const double multiplier = (locale->binaryUnitDialect() == KLocale::MetricBinaryDialect) ? 1000 : 1024;
	if (value > (multiplier * multiplier))
		if (value > (multiplier * multiplier * multiplier))
			return locale->formatByteSize(value, 2, KLocale::DefaultBinaryDialect, KLocale::UnitGigaByte);
		else
			return locale->formatByteSize(value, 2, KLocale::DefaultBinaryDialect, KLocale::UnitMegaByte);
	else
		return locale->formatByteSize(value, 2, KLocale::DefaultBinaryDialect, KLocale::UnitKiloByte);
}

ChartWidget::ChartWidget(const QString& title, const QString& hint, Chart* chartImplementation, QWidget* parent) :
	QWidget(parent) {

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	
	titleLabel = new QLabel("<strong>" + title + "</strong>", this);
	titleLabel->setAlignment(Qt::AlignHCenter);
	titleLabel->setToolTip(hint);
	mainLayout->addWidget(titleLabel);
	
	chart = chartImplementation;
	chart->setToolTip(hint);
	mainLayout->addWidget(chart);
	
	freeMemoryLabel = new QLabel("", this);
	freeMemoryLabel->setAlignment(Qt::AlignHCenter);
	freeMemoryLabel->setToolTip(hint);
	mainLayout->addWidget(freeMemoryLabel);

	chart->setFreeMemoryLabel(freeMemoryLabel);
}

void ChartWidget::setMemoryInfos(t_memsize* memoryInfos) {
	chart->setMemoryInfos(memoryInfos);
}

void ChartWidget::refresh() {
	//The update() method will launch paintEvent() automatically
	chart->update();
	
}


