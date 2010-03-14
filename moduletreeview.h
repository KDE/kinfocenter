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

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */

#ifndef __moduletreeview_h__
#define __moduletreeview_h__

#include <QListWidget>
#include <QPixmap>

#include <klistwidgetsearchline.h>

class ConfigModule;
class ConfigModuleList;

class ModuleTreeItem : public QListWidgetItem {

public:
	explicit ModuleTreeItem(QListWidget* parent, ConfigModule* module = NULL);

	ConfigModule* module() const;

private:
	ConfigModule *_module;

};

class ModuleTreeView : public QListWidget {
Q_OBJECT

public:
	explicit ModuleTreeView(ConfigModuleList* configModules, QWidget* parent = NULL);

	void fill();

	bool isGeneralItem(const QListWidgetItem* item) const;
	
	QListWidgetItem* generalItem() const;
	
	ModuleTreeItem* findMatchingItem(ConfigModule* configModule) const;

signals:
	void moduleSelected(ConfigModule* configModule);
	void generalSelected();
	
private slots:
	void selectItem();

private:
	QListWidgetItem* _generalItem;
	
	ConfigModuleList* _modules;
};

class ModuleWidgetSearchLine : public KListWidgetSearchLine {
public:
	ModuleWidgetSearchLine(QWidget* parent, ModuleTreeView* listWidget);

protected:

	virtual bool itemMatches(const QListWidgetItem* item, const QString& search) const;
};

#endif
