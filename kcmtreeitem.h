
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

#ifndef __KCMTREEITEM__
#define __KCMTREEITEM__

//KDE
#include <KCModuleInfo>
#include <KDebug>
#include <KIcon>

//QT
#include <QList>
#include <QString>
#include <QIcon>

class KcmTreeItem
{
  public:
    explicit KcmTreeItem(const KService::Ptr, KcmTreeItem *parent=0);
    explicit KcmTreeItem(QString, KcmTreeItem *parent=0);
    ~KcmTreeItem();
    
    void addChild(KcmTreeItem *);
    KcmTreeItem *child(int row);
    int childCount();
    
    KcmTreeItem *parent();
    int indexOf(KcmTreeItem *);
    
    int columnCount();
    int row();
    
    QString data();
    QString category();
    
    bool isValid() const;
    KcmTreeItem *containsCategory(QString); 
    
    const KCModuleInfo kcm() const; 
    int weight();
    QIcon icon() const;
    
    QString whatsThis() const;
    bool childrenRegExp(QRegExp pattern);
    
  private:
     QList<KcmTreeItem *> m_children;
     KcmTreeItem *m_parent;
     const KService::Ptr m_module;
     
     const QString m_category;
     const KCModuleInfo *m_moduleInfo;
     bool m_isValid;
};

#endif // __KCMTREEITEM__
