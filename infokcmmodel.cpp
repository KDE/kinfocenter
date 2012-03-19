
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

//Local
#include "infokcmmodel.h"
#include "kcmcategoryitem.h"

//KDE
#include <KServiceTypeTrader>
#include <KDebug>

InfoKcmModel::InfoKcmModel(QObject *parent) : QAbstractItemModel(parent), m_root(new KcmCategoryItem(i18n("Information Modules")))
{
	createTreeItems();
}

InfoKcmModel::~InfoKcmModel()
{
	delete m_root;
}

void InfoKcmModel::createTreeItems()
{
	KService::List categoryList = KServiceTypeTrader::self()->query("KInfoCenterCategory");
	foreach(const KService::Ptr &categoryModule, categoryList)
	{
		m_root->addChild(new KcmCategoryItem(categoryModule,m_root));
	}

	KService::List moduleList = KServiceTypeTrader::self()->query("KCModule", "[X-KDE-ParentApp] == 'kinfocenter'");
	foreach(const KService::Ptr &kcmModule, moduleList)
	{
		if (kcmModule->isType(KST_KService) == true)
		{
			QString category = kcmModule->property("X-KDE-KInfoCenter-Category").toString().trimmed();
			if(!category.isEmpty() || !category.isNull())
			{
				KcmTreeItem *item = m_root->containsCategory(category);
				if(item != NULL)
				{
					item->addChild(new KcmTreeItem(kcmModule,item));
				}
				else
				{
					KcmTreeItem *lost = m_root->containsCategory("lost_and_found");
					if(lost != NULL)
					{
						lost->addChild(new KcmTreeItem(kcmModule,lost));
					}
					else
					{
						kWarning() << "Lost and found category not found, unable to display lost Kcontrol modules";
					}
				}
			}
			else
			{
				m_root->addChild(new KcmTreeItem(kcmModule,m_root));
			}
		}
	}
}

QModelIndex InfoKcmModel::index(int row, int column, const QModelIndex &parent) const
{
	KcmTreeItem *parentItem;

	if (!parent.isValid())
	{
		parentItem = m_root;
	}
	else
	{
		parentItem = static_cast<KcmTreeItem*>(parent.internalPointer());
	}

	KcmTreeItem *childItem = parentItem->child(row);

	if (childItem)
	{
		return createIndex(row, column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex InfoKcmModel::index(int row, int column, KcmTreeItem *parent) const
{
	KcmTreeItem *childItem = parent->child(row);

	if (childItem)
	{
		return createIndex(row, column, childItem);
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex InfoKcmModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return QModelIndex();
	}

	KcmTreeItem *child = static_cast<KcmTreeItem*>(index.internalPointer());
	KcmTreeItem *parent = child->parent();

	if (parent == m_root)
	{
		return QModelIndex();
	}

	return createIndex(parent->row(), 0, parent);
}

QModelIndex InfoKcmModel::indexOf(KcmTreeItem *item)
{
	QModelIndex tmpIndex = createIndex(item->row(), 0, item);

	if(!tmpIndex.isValid())
	{
		return QModelIndex();
	}
	return tmpIndex;
}

int InfoKcmModel::rowCount(const QModelIndex &parent) const
{
	KcmTreeItem *parentItem;

	if (!parent.isValid())
	{
		parentItem = m_root;
	}
	else
	{
		parentItem = static_cast<KcmTreeItem*>(parent.internalPointer());
	}

	return parentItem->childCount();
}

int InfoKcmModel::columnCount(const QModelIndex &parent) const
{
	// Hard coded, menu should never have more than one column

	Q_UNUSED(parent);
	return 1;
}

QVariant InfoKcmModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	KcmTreeItem *item = static_cast<KcmTreeItem*>(index.internalPointer());
	switch(role)
	{
		case Qt::DisplayRole:
			return item->data();
			break;
		case Qt::UserRole:
			return item->weight();
			break;
		case Qt::DecorationRole:
			return item->icon();
			break;
		default:
			return QVariant();
	}
	return QVariant();
}

QVariant InfoKcmModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		return m_root->data();
	}

	return QVariant();
}

Qt::ItemFlags InfoKcmModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::ItemIsEnabled;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex InfoKcmModel::firstValid() const
{
	int rows = m_root->childCount();

	//Massive large number to max compare
	unsigned int winner = 0; winner--;

	QModelIndex winnerIndex = QModelIndex();

	for(int i=0;i<rows;i++)
	{
		KcmTreeItem *item = m_root->child(i);
		if(item->type() == KcmTreeItem::KCM)
		{
			if(winner >= (unsigned int)item->weight())
			{
				winner = item->weight();
				winnerIndex = index(item->row(),0,item->parent());
			}
		}
	}
	return winnerIndex;
}

QStringList InfoKcmModel::allChildrenKeywords()
{
	return childrenKeywords(m_root);
}

QStringList InfoKcmModel::childrenKeywords(KcmTreeItem *kcmItem)
{
	QStringList childKeywords;

	int rows = kcmItem->childCount();
	for(int i=0;i<rows;i++)
	{
		KcmTreeItem *item = kcmItem->child(i);
		if(item->type() == KcmTreeItem::CATEGORY)
		{
			childKeywords = childKeywords + childrenKeywords(item);
		}
		else
		{
			childKeywords = childKeywords + item->keywords();
		}
	}
	return childKeywords;
}
