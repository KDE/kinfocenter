
/*
 *  kdc.cpp
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

#include "kdc.h"

Kdc::Kdc(QTreeWidget *parent) : m_treeWidget(parent)
{ 
  generateList();
}

Kdc::~Kdc()
{
}

void Kdc::generateList() 
{  
  KService::List moduleList = KServiceTypeTrader::self()->query("KCModule", "[X-KDE-ParentApp] == 'kinfocenter'");
  
  // Check List
  for (int i = 0; i < moduleList.size(); ++i) {
    const KService::Ptr &module = moduleList.at(i);
    if (module->isType(KST_KService) == false) 
    {
      moduleList.removeAt(i);
    }
  }
  m_moduleList = moduleList;
}

KService::List *Kdc::kcmList()
{  
  if(isEmpty()) return NULL;
  return &m_moduleList;
}

bool Kdc::isEmpty() const 
{  
  if(m_moduleList.isEmpty()) return true;
  return false;
}

