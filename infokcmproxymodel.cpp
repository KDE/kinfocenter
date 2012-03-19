
/*
 *  infokcmproxymodel.cpp
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
#include "infokcmproxymodel.h"

//KDE
#include <KDebug>

InfoKcmProxyModel::InfoKcmProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
	setSortRole(Qt::UserRole);
}

bool InfoKcmProxyModel::lessThan(const QModelIndex &leftIndex, const QModelIndex &rightIndex) const
{
	if(!leftIndex.isValid() && !rightIndex.isValid()) return true;

	KcmTreeItem *leftItem = static_cast<KcmTreeItem*>(leftIndex.internalPointer());
	KcmTreeItem *rightItem = static_cast<KcmTreeItem*>(rightIndex.internalPointer());

	return (leftItem->weight() < rightItem->weight());
}

bool InfoKcmProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QModelIndex index=sourceModel()->index(sourceRow, 0, sourceParent);
	KcmTreeItem *indexItem = static_cast<KcmTreeItem*>(index.internalPointer());

	if(indexItem->type() == KcmTreeItem::CATEGORY)
	{
		if(indexItem->childrenRegExp(filterRegExp()) == true) return true;
	}
	return ((indexItem->keywords().filter(filterRegExp()).count() > 0));
}
