/*
 Copyright (c) 2000 Matthias Elter <elter@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __indexwidget_h__
#define __indexwidget_h__

#include "global.h"

#include <QWidget>


class ConfigModuleList;
class ConfigModule;
class ModuleTreeView;
class ModuleWidgetSearchLine;

class IndexWidget : public QWidget {
Q_OBJECT

public:
	IndexWidget(ConfigModuleList *list, QWidget *parent);
	virtual ~IndexWidget();

public slots:
	void reload();

	void selectGeneral();
	void selectModule(ConfigModule *);

signals:
	void generalActivated();
	void moduleActivated(ConfigModule *module);

private:
	
	void connectTree();
	void disconnectTree();
	
	ModuleTreeView *_tree;
	
	ModuleWidgetSearchLine* _searchLine;
};

#endif
