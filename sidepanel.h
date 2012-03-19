
/*
 *  sidepanel.h
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

#ifndef SIDEPANEL
#define SIDEPANEL

//QT
#include <QString>
#include <QModelIndex>
#include <QContextMenuEvent>
#include <QAction>
#include <QStringList>
#include <QTreeView>

//Local
#include "infokcmmodel.h"
#include "infokcmproxymodel.h"

class InfoKcmModel;
class InfoKcmProxyModel;

class SidePanel : public QTreeView
{
	Q_OBJECT

	public:

		/**
		* Create Side Panel Widget, including search line edit.
		*
		* @param parent objects parent
		*/
		SidePanel(QWidget *parent);

		/**
		* Destory SidePanel object
		*/
		~SidePanel();

		/**
		* Select the first valid item in TreeView
		*/
		void changeToFirstValidItem();

		/**
		* Map a QAbstractItemModel index to a QSortFilterProxyModel index
		*/
		QModelIndex mapToProxySource(const QModelIndex&);

		/**
		* Get all the treeview items keywords
		*/
		QStringList allChildKeywords();

		/**
		* Expand and collapse an Item ( if an expandable item )
		*/
		void toggleExpand(const KcmTreeItem *);

		Q_SIGNALS:

		/**
		* Emitted when menu item is clicked
		*/
		void activated(const KcmTreeItem *);

		private Q_SLOTS:

		/**
		* Triggered when treeview item is clicked
		*/
		void activatedSlot(const QModelIndex &index);

		public Q_SLOTS:

		/**
		* Triggered when collapseAll is selected from tree view
		* item menu
		*/
		void collapseAllSlot();

		/**
		* Triggered when ExpandAll is selected from tree view
		* item menu
		*/
		void expandAllSlot();

		/**
		* Filter out all but menu items that fit certain keywords
		*/
		void filterSideMenuSlot(const QString &);

		/**
		* Triggered when Reset Search is selected from tree view
		* item menu
		*/
		void resetSearchSlot();

	private:

		/**
		* Created actions for the tree view item menu
		*/
		void createMenuActions();

		/**
		* Create treeview item menu
		*/
		void contextMenuEvent(QContextMenuEvent *event);

		InfoKcmModel *m_model;
		InfoKcmProxyModel *m_proxyModel;

		QAction *colAct;
		QAction *expAct;
		QAction *resetAct;
};

#endif //SIDEPANEL
