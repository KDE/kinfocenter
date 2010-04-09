
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

#include "infokcmmodel.h"

InfoKcmModel::InfoKcmModel(QObject *parent) : QAbstractItemModel(parent), m_root(new KcmTreeItem(i18n("Information Modules")))
{
  createTreeItems();  
}

void InfoKcmModel::createTreeItems() 
{ 
  // Automatically creates categories from X-KDE-KInfoCenter-Category.
  // Category tree item should never have more than one child due to usability reason.
 
  KService::List moduleList = KServiceTypeTrader::self()->query("KCModule", "[X-KDE-ParentApp] == 'kinfocenter'");
 
  foreach(const KService::Ptr &module, moduleList)
  {
    if (module->isType(KST_KService) == true)
    {   
      QString category = module->property("X-KDE-KInfoCenter-Category").toString().trimmed();
      if(!category.isEmpty() || !category.isNull()) 
      {
	KcmTreeItem *item = m_root->containsCategory(category);
	if(item != NULL) 
	{
	  item->addChild(new KcmTreeItem(module,item));
	}
	else 
	{
	  KcmTreeItem *newCategory = new KcmTreeItem(category,m_root);
	 
	  m_root->addChild(newCategory);
	  newCategory->addChild(new KcmTreeItem(module,newCategory));
	}
      }
      else
      {
	m_root->addChild(new KcmTreeItem(module,m_root));
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
  return firstValid(m_root);
}

QModelIndex InfoKcmModel::firstValid(KcmTreeItem *kcmItem) const
{ 
  int rows = kcmItem->childCount();
  for(int i=0;i<rows;i++)  
  {
    KcmTreeItem *item = kcmItem->child(i);
    if(item->isValid() !=  false)
    {
      return createIndex(item->row(),0,item);
    }
  }
  
  for(int i=0;i<rows;i++)  
  {
    KcmTreeItem *item = kcmItem->child(i);
    if(item->isValid() ==  false)
    {
      QModelIndex valid = firstValid(item);
      if(valid != QModelIndex()) return valid;
    }
  }
  return QModelIndex();
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
    if(item->isValid() ==  false)
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
