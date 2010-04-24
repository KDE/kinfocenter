
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

//KDE
#include <KLocale>

//QT
#include <QTreeView>
#include <QWidget>
#include <QRegExp>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

//Local
#include "infokcmmodel.h"
#include "infokcmproxymodel.h"


class SidePanel : public QTreeView
{
  Q_OBJECT
  
  public:
    SidePanel(QWidget *);
    ~SidePanel();
    
    void changeToRootSelection();
    QModelIndex mapProxySource(QModelIndex);
    void filterSideMenu(QString);
    QStringList allChildrenKeywords();
    
  signals:
    void clicked(const KcmTreeItem *);
    
  private slots:
    void clickedSlot(const QModelIndex &index);
    
  public slots:
    void collapseAllSlot();
    void expandAllSlot();	
    void filterSideMenuSlot(const QString &);
    void resetSearchSlot();

  private:
    void createMenuActions();
    void contextMenuEvent(QContextMenuEvent *event);
    
    InfoKcmModel *m_model;
    InfoKcmProxyModel *m_proxyModel;
    
    QAction *colAct;
    QAction *expAct;
    QAction *resetAct;
};

#endif //__SIDEPANEL__
