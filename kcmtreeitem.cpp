
/*
 *  kcmtreeitem.h
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

//KDE
#include <KDebug>

//Local
#include "kcmtreeitem.h"

KcmTreeItem::KcmTreeItem(const KService::Ptr module, KcmTreeItem *parent) : m_parent(parent), m_module(module), 
  m_moduleInfo(new KCModuleInfo(m_module)), m_isValid(true)
{
}

KcmTreeItem::KcmTreeItem(QString category, KcmTreeItem *parent) : m_parent(parent), m_category(category), 
  m_isValid(false)
{
}

KcmTreeItem::~KcmTreeItem()
{
  qDeleteAll(m_children);
  delete m_parent;
}

void KcmTreeItem::addChild(KcmTreeItem *child) 
{
  m_children.append(child);
}

KcmTreeItem *KcmTreeItem::child(int row)
{
  if(childCount() > row) return m_children.value(row);
  return NULL;
}

int KcmTreeItem::childCount() 
{
  return m_children.count();
}

int KcmTreeItem::columnCount()
{
  // Hard coded, menu should never have more than one column
  return 1;
}

KcmTreeItem *KcmTreeItem::parent()
{
  return m_parent;
}

int KcmTreeItem::row()
{
  if (m_parent)
  {
    return indexOf(const_cast<KcmTreeItem*>(this));
  }
  
  return 0;
}

int KcmTreeItem::indexOf(KcmTreeItem *item)
{
  if (m_parent)
  {
    return m_parent->m_children.indexOf(item);
  }
  
  return 0;
}

QString KcmTreeItem::data()
{
  if(isValid() == true)
  {
    return m_moduleInfo->moduleName();
  }
  else 
  {
    return category();
  }
}

QString KcmTreeItem::category()
{
  if(isValid() == false)
  {
    return m_category;
  }
  return QString("");
}

bool KcmTreeItem::isValid() const 
{
  return m_isValid;
}

KcmTreeItem *KcmTreeItem::containsCategory(QString category) 
{
  foreach(KcmTreeItem *item, m_children)
  {
    if(item->isValid() == false)
    {
      if(item->category().contains(category))
      {
	return item;
      }
      else
      {
	if(item->containsCategory(category))
	{
	  return item;
	}
      }
    }
  }
  return NULL;
}

const KCModuleInfo KcmTreeItem::kcm() const 
{
  if(isValid() == false) return KCModuleInfo();
  return *m_moduleInfo;
}

int KcmTreeItem::weight() 
{
  if(isValid() == false) return category().count();
  return m_moduleInfo->weight();
}

KIcon KcmTreeItem::icon() const
{
  if(isValid() == false)
  {
    return KIcon("inode-directory");
  }
  else 
  {
    return KIcon(m_moduleInfo->icon());
  }
}

QString KcmTreeItem::whatsThis() const
{
  if(isValid() == false) return QString();
  return m_moduleInfo->comment();
}

bool KcmTreeItem::childrenRegExp(QRegExp pattern)
{
  foreach(KcmTreeItem *item, m_children)
  {
    if((item->keywords().filter(pattern).count() > 0) == true)
    {
      return true;
    }
  }
  return false;
}

QStringList KcmTreeItem::keywords()
{
  if(isValid() == false) return QStringList();
  if(m_moduleInfo->keywords().isEmpty()) return QStringList(data());
  return m_moduleInfo->keywords();
}
