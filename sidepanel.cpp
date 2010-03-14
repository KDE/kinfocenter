
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

#include "sidepanel.h"

SidePanel::SidePanel(QWidget *parent) : QTreeWidget(parent), Kdc(this)
{ 
  setHeaderLabels(QStringList(i18nc("List header topic","Information Modules"))); 
  
  m_categories = new QHash<QString,KcmMenuItem *>();
  
  generateMenuContent();
  setSortingEnabled(true);
  expandAll();
}

void SidePanel::generateMenuContent() 
{  
  KService::List *moduleList = kcmList();
  int rowCount = 100;
  
  foreach(const KService::Ptr &module, *moduleList) 
  {
    QVariant prop = module->property("X-KDE-System-Settings-Parent-Category");
    if(prop.isValid() || !prop.toString().isEmpty()) 
    {
      QString category = prop.toString();
      if(isCategory(category) == true) 
      {
	KcmMenuItem *root = categoryTreeRoot(category);
	new KcmMenuItem(root,module);    
      } 
      else
      {
	KcmMenuItem *newCat = new KcmMenuItem(this,category,rowCount);
	rowCount+=10;
	m_categories->insert(category,newCat);
      
	new KcmMenuItem(newCat,module);
      }
    }
    else 
    {
     new KcmMenuItem(this,module);
    }
  } 
}

bool SidePanel::isCategory(QString cat) const 
{ 
  if(m_categories->contains(cat)) return true;
  return false;
}

KcmMenuItem *SidePanel::categoryTreeRoot(QString key) const 
{  
  if(!isCategory(key)) return NULL;
  return m_categories->value(key);
}
  
