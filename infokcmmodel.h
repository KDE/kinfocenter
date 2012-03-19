
/*
 *  infokcmmodel.h
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

#ifndef INFOKCMMODEL
#define INFOKCMMODEL

//KDE
#include <KService>

//QT
#include <QObject>
#include <QStringList>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>

//Local
#include "kcmtreeitem.h"

class KcmTreeItem;

class InfoKcmModel : public QAbstractItemModel
{
	Q_OBJECT

	public:

		/**
		* Create InfoKcmModel object.
		* Abstract data model to display KCM's for a tree view
		*
		* @param parent objects parent
		*/
		InfoKcmModel(QObject *parent);

		~InfoKcmModel();

		/**
		* Get index of item in model
		*
		* @param row row position
		* @param column column position
		* @param parent parent of object
		* @return index of object
		*/
		QModelIndex index(int row, int column, const QModelIndex& parent) const;
		QModelIndex index(int row, int column, KcmTreeItem *parent) const;

		/**
		* Get parent of item in model
		*/
		QModelIndex parent(const QModelIndex& index) const;

		/**
		* Get amount of rows under parent
		*/
		int rowCount(const QModelIndex& parent) const;

		/**
		* Get amount of columns under parent
		*/
		int columnCount(const QModelIndex& parent) const;

		/**
		* Get the stored data for a role
		*
		* @param index objects index
		* @param role role to retrieve data about
		*/
		QVariant data(const QModelIndex& index, int role) const;

		/**
		* Get header information
		*/
		QVariant headerData(int, Qt::Orientation, int) const;

		/**
		* Get set flags for a treeitem
		*/
		Qt::ItemFlags flags(const QModelIndex &) const;

		/**
		* Get the first valid item on the treeview
		* Checks main root items only
		*
		* @return index of valid item
		*/
		QModelIndex firstValid() const;

		/**
		* Get all KCM keywords for all KCMs stored in the model
		*/
		QStringList allChildrenKeywords();

		/**
		* Get QModelIndex of a KcmTreeItem
		*/
		QModelIndex indexOf(KcmTreeItem *item);

		private:

		/**
		* Init tree items
		*/
		void createTreeItems();

		/**
		* Get a certain KCM's keywords
		*/
		QStringList childrenKeywords(KcmTreeItem *kcmItem);

		KService::List m_moduleList;
		KcmTreeItem *m_root;
};

#endif // INFOKCMMODEL
