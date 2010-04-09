
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

#ifndef __INFOKCMMODEL__
#define __INFOKCMMODEL__

//KDE
#include <KService>
#include <KServiceTypeTrader>
#include <KDebug>

//QT
#include <QAbstractItemModel>
#include <QObject>
#include <QHash>
#include <QString>

//Local
#include "kcmtreeitem.h"


class QAbstractItemModel;
class QObject;

class InfoKcmModel : public QAbstractItemModel 
{
  Q_OBJECT
  
  public:
    InfoKcmModel(QObject *);
    
    QModelIndex index(int, int, const QModelIndex&) const;
    QModelIndex parent(const QModelIndex&) const;
    
    int rowCount(const QModelIndex&) const;
    int columnCount(const QModelIndex&) const;
    
    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;
    
    Qt::ItemFlags flags(const QModelIndex &) const;
    
    QModelIndex firstValid() const;
    QStringList allChildrenKeywords();
    
  private:    
    void createTreeItems();
    QModelIndex firstValid(KcmTreeItem *kcmItem) const;
    QStringList childrenKeywords(KcmTreeItem *kcmItem);
    
    KService::List m_moduleList;
    KcmTreeItem *m_root;
};

#endif // __INFOKCMMODEL__
