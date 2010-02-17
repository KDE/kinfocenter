/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 2008, Nicolas Ternisien <nicolas.ternisien@gmail.com>   *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef USED_SIZE_WIDGET_H
#define USED_SIZE_WIDGET_H

#include <QFrame>

class UsedSizeWidget : public QWidget {
	Q_OBJECT
public:
	UsedSizeWidget(QWidget* parent = NULL);
	
	void setUsedSize(const QString& mountPoint, quint64 totalSize, quint64 usedSize, quint64 availableSize);

protected:
	virtual void paintEvent(QPaintEvent* event);
	
private:
	QString mountPoint;
	quint64 totalSize;
	quint64 usedSize;
	quint64 availableSize;

	
};

#endif // USED_SIZE_WIDGET_H
