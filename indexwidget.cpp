/*
 Copyright (c) 2000 Matthias Elter <elter@kde.org>
 Copyright (c) 2003 Frauke Oster <frauke.oster@t-online.de>

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

#include "indexwidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "moduletreeview.h"

#include <kdebug.h>
#include <klocale.h>

#include "indexwidget.moc"

IndexWidget::IndexWidget(ConfigModuleList *modules, QWidget *parent) :
	QWidget(parent) {
	
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);

	_tree=new ModuleTreeView(modules, this);
	
	QHBoxLayout* searchLayout = new QHBoxLayout();
	searchLayout->setMargin(0);
	
	QLabel* searchLabel = new QLabel(i18n("Filter:"), this);
	_searchLine = new ModuleWidgetSearchLine(this, _tree);
	searchLabel->setBuddy(_searchLine);
	
	searchLayout->addWidget(searchLabel);
	searchLayout->addWidget(_searchLine);
	
	_tree->fill();
	connectTree();
	
	mainLayout->addLayout(searchLayout);
	mainLayout->addWidget(_tree);

	kDebug() << "Index Widget initialized" << endl;
}

IndexWidget::~IndexWidget() {
}

void IndexWidget::reload() {
}

void IndexWidget::selectModule(ConfigModule *module) {
	kDebug() << "Selecting module..." << endl;

	ModuleTreeItem* moduleTreeItem = _tree->findMatchingItem(module);
	
	disconnectTree();
	
	_tree->scrollToItem(moduleTreeItem);
	moduleTreeItem->setSelected(true);

	connectTree();

	emit moduleActivated(module);
}

void IndexWidget::selectGeneral() {
	_tree->scrollToItem(_tree->generalItem());

	disconnectTree();
		
	_tree->generalItem()->setSelected(true);
	
	connectTree();

	emit generalActivated();
}

void IndexWidget::connectTree() {
	connect(_tree, SIGNAL(moduleSelected(ConfigModule*)), this, SLOT(selectModule(ConfigModule*)));
	connect(_tree, SIGNAL(generalSelected()), this, SLOT(selectGeneral()));
}

void IndexWidget::disconnectTree() {
	_tree->disconnect(SIGNAL(generalSelected()));
	_tree->disconnect(SIGNAL(moduleSelected(ConfigModule*)));
}
