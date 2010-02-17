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

#include "usedSizeWidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>

UsedSizeWidget::UsedSizeWidget(QWidget* parent) :
	QWidget(parent), totalSize(0), usedSize(0), availableSize(0) {
	
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	//setAutoFillBackground(true);

}

void UsedSizeWidget::paintEvent(QPaintEvent* /*event*/) {
	//kDebug() << "PaintEvent" << mountPoint << totalSize << usedSize << availableSize;

	QPainter paint(this);

	int endWidth = width() - 2;
	int endHeight = height() - 2;
	
	if (endWidth<=0 || endHeight<=0) {
		return;
	}

	if (totalSize==0) {
		paint.fillRect(1, 1, endWidth, endHeight, QColor(0xCC, 0xCC, 0xCC, 50));
		paint.drawText(1, 1, endWidth, endHeight, Qt::AlignCenter | Qt::TextWordWrap, i18n("unknown"));
		return;
	}
	
	QLinearGradient gradient(QPointF(0, endHeight), QPointF(endWidth, endHeight));

	qreal percent = ((qreal)usedSize) / ((qreal)totalSize);

	gradient.setColorAt(0, QColor(0xA3, 0xF7, 0x72));
	
	if (percent > 0.5)
		gradient.setColorAt(0.5, QColor(0x5E, 0xD9, 0x35));

	if (percent > 0.6)
		paint.setPen(Qt::black);

	if (percent > 0.75)
		gradient.setColorAt(0.75, QColor(0xF7, 0xB6, 0x3D));
	
	if (percent > 0.95) {
		gradient.setColorAt(0.90, QColor(0xF7, 0x78, 0x34));
		gradient.setColorAt(0.95, QColor(0xD3, 0x42, 0x19));
	}
	
	gradient.setColorAt(percent, Qt::transparent);
	
	paint.fillRect(1, 1, endWidth, endHeight, gradient);

	int percentUsed = (usedSize * 100) / totalSize;
	paint.drawText(1, 1, endWidth, endHeight, Qt::AlignCenter | Qt::TextWordWrap, QString("%1%").arg(percentUsed));

}

void UsedSizeWidget::setUsedSize(const QString& mountPoint, quint64 totalSize, quint64 usedSize, quint64 availableSize) {
	
	this->totalSize = totalSize;
	this->usedSize = usedSize;
	this->availableSize = availableSize;
	this->mountPoint = mountPoint;
	
	kDebug() << mountPoint << totalSize << usedSize << availableSize;
	
	update();
}

#include "usedSizeWidget.moc"
