
/*
 *  kcmmenuitem.h
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

#ifndef __MENUITEM__
#define __MENUITEM__

//QT
#include <QTreeWidgetItem>

//KDE
#include <KCModuleInfo>
#include <KIcon>
#include <KDebug>
#include <KIconLoader>

class KcmMenuItem : public QTreeWidgetItem
{
  public:
    
    KcmMenuItem(QTreeWidgetItem *, const KService::Ptr &);
    KcmMenuItem(QTreeWidget *, const KService::Ptr &);
    
    KcmMenuItem(QTreeWidgetItem *, const QString, int);
    KcmMenuItem(QTreeWidget *, const QString, int);
    
    ~KcmMenuItem();
    
    void setRootItem(QString);
    void setItem();
    const KCModuleInfo kcm() const;
    int kcmWeight() const;
    bool operator<(const QTreeWidgetItem &) const;
    bool isValid() const;
    const KIcon defaultKcmIcon(); 
    
  private:
    void setName(const QString name=NULL);
    
    int m_weight;
    const bool m_isValid;
    KCModuleInfo *m_modInfo;
};

#endif //__MENUITEM__