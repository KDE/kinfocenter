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

#ifndef KCONTROL_KCM_PCI_H
#define KCONTROL_KCM_PCI_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <kcmodule.h>
#include <kaboutdata.h>

#include "os_current.h"


class QPushButton;
class QTreeWidget;

class KCMPci : public KCModule {
	Q_OBJECT

public:
	explicit KCMPci(QWidget *parent = NULL, const QVariantList &list = QVariantList());
	~KCMPci();

	virtual void load();
	virtual QString quickHelp() const;

private:
	QTreeWidget* tree;
};

#endif

