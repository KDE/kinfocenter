/*
 Copyright (c) 2000 Matthias Elter <elter@kde.org>
 Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#include "moduletreeview.h"
#include "global.h"
#include "modules.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kservicegroup.h>
#include <kdebug.h>
#include <kicon.h>


#include "moduletreeview.moc"

ModuleTreeView::ModuleTreeView(ConfigModuleList *modules, QWidget * parent) :
	QListWidget(parent), _modules(modules) {
	
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(selectItem()));

	 setSortingEnabled(false);
	_generalItem = NULL;
}

void ModuleTreeView::fill() {
	_generalItem = new QListWidgetItem(KIcon(KINFOCENTER_ICON_NAME), i18n("General Information"), this);
	
	foreach(ConfigModule* configModule, *_modules) {
		new ModuleTreeItem(this, configModule);
	}
}

void ModuleTreeView::selectItem() {
	QListWidgetItem* item = this->currentItem();
	if (item==NULL)
		return;
	
	if (isGeneralItem(item)) {
		kDebug() << "General Selected" << endl;
		emit generalSelected();
		return;
	}
	
	kDebug() << "Select item" << endl;
	ModuleTreeItem* moduleItem = static_cast<ModuleTreeItem*>(item);
	
	emit moduleSelected(moduleItem->module());
	kDebug() << "Select item end" << endl;
}


bool ModuleTreeView::isGeneralItem(const QListWidgetItem* item) const {
	if (item == _generalItem)
		return true;
	
	return false;
}

QListWidgetItem* ModuleTreeView::generalItem() const {
	return _generalItem;
}

ModuleTreeItem* ModuleTreeView::findMatchingItem(ConfigModule* configModule) const {
	for (int i = 0; i < count() ; ++i) {
		QListWidgetItem* tempItem = item(i);
		if (isGeneralItem(tempItem)) {
			continue;
		}

		ModuleTreeItem* moduleItem = static_cast<ModuleTreeItem*>(tempItem);
		if (moduleItem->module()==configModule)
			return moduleItem;

	}

	kDebug() << "Unable to find the matching item" << endl;
	return NULL;
}


ModuleTreeItem::ModuleTreeItem(QListWidget *parent, ConfigModule *module) :
	QListWidgetItem(parent), _module(module) {

	setText(module->moduleName());
	
	setIcon(module->realIcon(KIconLoader::SizeSmall));

}

ConfigModule* ModuleTreeItem::module() const {
	return _module;
}


ModuleWidgetSearchLine::ModuleWidgetSearchLine(QWidget* parent, ModuleTreeView* listWidget) :
	KListWidgetSearchLine(parent, listWidget) {
	
}

bool ModuleWidgetSearchLine::itemMatches(const QListWidgetItem* item, const QString& search) const {
	
	const ModuleTreeView* moduleTree = static_cast<const ModuleTreeView*>(listWidget());
	if (moduleTree->isGeneralItem(item)) {
		return true;
	}
	
	const ModuleTreeItem* moduleItem = static_cast<const ModuleTreeItem*>(item);
	
	QStringList itemMatches;
	itemMatches << moduleItem->module()->moduleName();
	//kDebug() << "Module name " << moduleItem->module()->moduleName() << endl;
	
	QStringList keywords = moduleItem->module()->keywords();
	foreach(const QString &keyword, keywords) {
		//kDebug() << "Key word " << keyword << endl;
		itemMatches.append(keyword);
	}
	
	
	foreach(const QString &itemMatch, itemMatches) {
		if (itemMatch.contains(search, Qt::CaseInsensitive)) {
			return true;
		}
	}
	
	return false;
}
