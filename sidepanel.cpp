
/*
 *  sidepanel.cpp
 *
 *  Copyright (C) 2010 David Hubner <hubnerd@ntlworld.com>
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
 *
 */

//Local
#include "sidepanel.h"

//KDE
#include <KLocale>
#include <KDebug>

//QT
#include <QWidget>
#include <QRegExp>
#include <QMenu>

SidePanel::SidePanel(QWidget *parent) : QTreeView(parent)
{
	setSortingEnabled(true);
	setAnimated(true);
	sortByColumn(0, Qt::AscendingOrder);
	m_model = new InfoKcmModel(this);

	m_proxyModel = new InfoKcmProxyModel(this);
	m_proxyModel->setSourceModel(m_model);

	createMenuActions();

	setMouseTracking(true);
	setModel(m_proxyModel);
	connect(this,SIGNAL(activated(QModelIndex)),this,SLOT(activatedSlot(QModelIndex)));
}

SidePanel::~SidePanel()
{
	disconnect(this,SIGNAL(activated(QModelIndex)),this,SLOT(activatedSlot(QModelIndex)));

	delete m_proxyModel;
	delete m_model;
}

void SidePanel::activatedSlot(const QModelIndex &index)
{
	if(index.isValid() == false) return;

	const KcmTreeItem *item = static_cast<KcmTreeItem*>(m_proxyModel->mapToSource(index).internalPointer());
	emit activated(item);
}

void SidePanel::changeToFirstValidItem()
{
	QModelIndex rootIndex = m_proxyModel->mapFromSource(m_model->firstValid());
	if(rootIndex.isValid() == false) return;

	setCurrentIndex(rootIndex);
	emit activatedSlot(rootIndex);
}

QModelIndex SidePanel::mapToProxySource(const QModelIndex& index)
{
	QModelIndex tmp = m_proxyModel->mapToSource(index);
	if(tmp.isValid() == true) return tmp;
	return QModelIndex();
}

void SidePanel::filterSideMenuSlot(const QString &pattern)
{
	if(pattern.isEmpty())
	{
		collapseAll();
	}
	else
	{
		expandAll();
	}
	m_proxyModel->setFilterRegExp(QRegExp(pattern,Qt::CaseInsensitive));
}

void SidePanel::createMenuActions()
{
	resetAct = new QAction(i18n("Clear Search"), this);
	connect(resetAct, SIGNAL(triggered()), this, SLOT(resetSearchSlot()));

	expAct = new QAction(i18n("Expand All Categories"), this);
	connect(expAct, SIGNAL(triggered()), this, SLOT(expandAllSlot()));

	colAct = new QAction(i18n("Collapse All Categories"), this);
	connect(colAct, SIGNAL(triggered()), this, SLOT(collapseAllSlot()));
}

void SidePanel::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);

	menu.addAction(colAct);
	menu.addAction(expAct);
	menu.addAction(resetAct);
	menu.exec(event->globalPos());
}

void SidePanel::collapseAllSlot()
{
	collapseAll();
}

void SidePanel::expandAllSlot()
{
	expandAll();
}

QStringList SidePanel::allChildKeywords()
{
	return m_model->allChildrenKeywords();
}

void SidePanel::resetSearchSlot()
{
	filterSideMenuSlot("");
}

