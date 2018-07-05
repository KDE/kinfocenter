/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _KCMUSB_H
#define _KCMUSB_H

#include <QMap>

#include <kcmodule.h>

class QTreeWidget;
class QTreeWidgetItem;
class QTextEdit;

class USBViewer : public KCModule {
Q_OBJECT

public:

        explicit USBViewer(QWidget *parent = nullptr, const QVariantList &list=QVariantList());

	void load() override;

protected Q_SLOTS:

	void selectionChanged(QTreeWidgetItem *item);
	void refresh();

private:

	QMap<int, QTreeWidgetItem*> _items;
	QTreeWidget *_devices;
	QTextEdit *_details;
};

#endif
