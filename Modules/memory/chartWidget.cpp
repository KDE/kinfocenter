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
#include <qdrawutil.h>

#include <KLocalizedString>
#include <KFormat>
#include <QDebug>

Chart::Chart(QWidget* parent) :
	QWidget(parent) {
	
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	
    memoryInfos = nullptr;
    mFreeMemoryLabel = nullptr;
}


void Chart::setMemoryInfos(t_memsize* memoryInfos) {
	this->memoryInfos = memoryInfos;
}

void Chart::setFreeMemoryLabel(QLabel* freeMemoryLabel) {
    this->mFreeMemoryLabel = freeMemoryLabel;
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
		setAccessibleDescription(i18n("Not available."));
        mFreeMemoryLabel->setText(i18n("Not available."));

		return false;
	}

	QStringList accessibleDescription;
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

		////qDebug() << "Count : " <<  count << " Percent : " << percent << "%" << " Localheight:" << localheight << endl;
		
		if (localheight>0) {
			QLinearGradient gradient(QPointF(1, startline), QPointF(width()-2, -localheight));

			QColor endProgressColor(0xFF, 0xFF, 0xFF, 100);
			gradient.setColorAt(0, color);
			gradient.setColorAt(1, endProgressColor);
			paint.fillRect(1, startline, width()-2, -localheight, gradient);

			//paint.fillRect(1, startline, width()-2, -localheight, color);

			if (localheight >= SPACING) {
				paint.drawText(0, startline-localheight, width(), localheight, Qt::AlignCenter | Qt::TextWordWrap, QStringLiteral("%1 %2%").arg(text).arg(percent));
				accessibleDescription.append(QStringLiteral("%1 %2%").arg(text).arg(percent));
			}
		}

		startline -= localheight;

	}

	// draw surrounding box
	QRect r = rect();
	qDrawShadePanel(&paint, r.x(), r.y(), r.width(), r.height(), palette(), true, 1);

    mFreeMemoryLabel->setText(i18n("%1 free", formattedUnit(last_used)));

    setAccessibleDescription(accessibleDescription.join(QLatin1Char('\n')));
	return true;
}


QString Chart::formattedUnit(t_memsize value) {
	return KFormat().formatByteSize(value, 2);
}

ChartWidget::ChartWidget(const QString& title, const QString& hint, Chart* chartImplementation, QWidget* parent) :
	QWidget(parent) {

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	
    titleLabel = new QLabel(QStringLiteral("<strong>") + title + QStringLiteral("</strong>"), this);
	titleLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
	titleLabel->setAlignment(Qt::AlignHCenter);
	titleLabel->setToolTip(hint);
	mainLayout->addWidget(titleLabel);
	
	chart = chartImplementation;
	chart->setToolTip(hint);
	mainLayout->addWidget(chart);
	
    mFreeMemoryLabel = new QLabel(QString(), this);
    mFreeMemoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    mFreeMemoryLabel->setAlignment(Qt::AlignHCenter);
    mFreeMemoryLabel->setToolTip(hint);
    mainLayout->addWidget(mFreeMemoryLabel);

    chart->setFreeMemoryLabel(mFreeMemoryLabel);
}

void ChartWidget::setMemoryInfos(t_memsize* memoryInfos) {
	chart->setMemoryInfos(memoryInfos);
}

void ChartWidget::refresh() {
	//The update() method will launch paintEvent() automatically
	chart->update();
}

