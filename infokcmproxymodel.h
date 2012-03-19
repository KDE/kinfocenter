
/*
 *  infokcmproxymodel.h
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

#ifndef INFOKCMPROXYMODEL
#define INFOKCMPROXYMODEL

//QT
#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>

//Local
#include "kcmtreeitem.h"

class InfoKcmProxyModel : public QSortFilterProxyModel
{
	public:

		/**
		* Create InfoKcMProxyModel object. Used to filter out
		* and sort a tree view.
		*
		* @param parent objects parent
		*/
		InfoKcmProxyModel(QObject *parent);

		/**
		* Check if left item is less than right item
		* Uses X-KDE-Weight for measurements
		*/
		bool lessThan(const QModelIndex &, const QModelIndex &) const;

	private:

		/**
		* Filter out rows
		* Used in search implementation
		*/
		bool filterAcceptsRow(int, const QModelIndex &) const;
};

#endif // INFOKCMPROXYMODEL
