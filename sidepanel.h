
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

#ifndef __SIDEPANEL__
#define __SIDEPANEL__

//Local
#include "kdc.h"

//KDE
#include <KLocale>

//QT
#include <QTreeWidget>
#include <QWidget>

class SidePanel : public QTreeWidget, public Kdc
{
  public:
    SidePanel(QWidget *);
    
  private:
    void generateMenuContent();
    KcmMenuItem *categoryTreeRoot(QString) const;
    bool isCategory(QString) const;
    
    QHash<QString,KcmMenuItem *> *m_categories;
};

#endif //__SIDEPANEL__