
/*
 *  kcmmenuitem.cpp
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

#include "kcmmenuitem.h"

KcmMenuItem::KcmMenuItem(QTreeWidgetItem *parent, const KService::Ptr &module) : QTreeWidgetItem(parent), 
  m_isValid(true), m_modInfo(new KCModuleInfo(module))
{
  setItem();
}

KcmMenuItem::KcmMenuItem(QTreeWidget *parent, const KService::Ptr &module) : QTreeWidgetItem(parent),
  m_isValid(true), m_modInfo(new KCModuleInfo(module))
{
  setItem();
}

KcmMenuItem::KcmMenuItem(QTreeWidgetItem *parent, const QString name, int kcmWeight) : QTreeWidgetItem(parent), 
  m_weight(kcmWeight), m_isValid(false), m_modInfo(0) 
{ 
  setRootItem(name);
}

KcmMenuItem::KcmMenuItem(QTreeWidget *parent, const QString name, int kcmWeight) : QTreeWidgetItem(parent), 
   m_weight(kcmWeight), m_isValid(false), m_modInfo(0)
{ 
  setRootItem(name);
}

KcmMenuItem::~KcmMenuItem() 
{
  delete m_modInfo;
}

void KcmMenuItem::setRootItem(QString name) 
{
  setName(name);
  setIcon(0,KIcon("folder"));
}

void KcmMenuItem::setItem() 
{
  m_weight = m_modInfo->weight();
  setToolTip(0,m_modInfo->comment());
  
  setName(NULL);
  setIcon(0,defaultKcmIcon());
}

void KcmMenuItem::setName(const QString name) 
{ 
  QString defName = name;
  if(defName==NULL && m_modInfo!=NULL) defName = m_modInfo->moduleName();

  setText(0,defName);
}

const KIcon KcmMenuItem::defaultKcmIcon() 
{  
  KIcon kcmIcon;
  if(m_isValid == true)
  {
    QPixmap providedIcon = KIconLoader::global()->loadIcon(m_modInfo->icon(), KIconLoader::Small, 0,
							   KIconLoader::DefaultState, QStringList(), 0L, true);
    if(providedIcon.isNull()) 
    {
     kcmIcon = KIcon("image-missing");
    }
    else 
    {
      kcmIcon = KIcon(m_modInfo->icon());
    }
  }
  return kcmIcon;
}

const KCModuleInfo KcmMenuItem::kcm() const 
{
    return *m_modInfo;
}

int KcmMenuItem::kcmWeight() const 
{
  return m_weight;
}

bool KcmMenuItem::operator<(const QTreeWidgetItem &other) const
{
  const KcmMenuItem *kcmItem = static_cast<const KcmMenuItem *>(&other); 
  return (kcmItem->kcmWeight() < kcmWeight());
}

bool KcmMenuItem::isValid() const
{
  return m_isValid;
}