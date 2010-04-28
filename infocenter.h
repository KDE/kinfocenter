
/*
 *  infocenter.h
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

#ifndef INFOCENTER
#define INFOCENTER

//KDE
#include <KXmlGuiWindow>
#include <KLineEdit>
#include <KAction>
#include <KCModule>

//QT
#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QSplitter>
#include <QPushButton>

//Local
#include "sidepanel.h"
#include "kcmcontainer.h"
#include "infokcmmodel.h"
#include "kcmtreeitem.h"
#include "tooltipmanager.h"

class KcmContainer;
class SizePanel;
class ToolTipManager;
class KcmTreeItem;

class KInfoCenter : public KXmlGuiWindow
{
  Q_OBJECT
  
  public:
    KInfoCenter();
    ~KInfoCenter();

    bool eventFilter(QObject *watched, QEvent *event);

  public slots:
    void itemClickedSlot(const KcmTreeItem *);
    void helpClickedSlot();
    void exportClickedSlot();
    void aboutKcmSlot();
    
    static void showError(QWidget *,QString);
    
  private:
    void initMenuBar();
    void createButtonBar();
    void createMainFrame();
    void CreateMenuFrame();
    void setButtons(const KCModule::Buttons buttons);
    void setKcm(const KcmTreeItem *);
    void resetCondition();

    KcmContainer *m_contain;
    QSplitter *m_splitter;
    
    KAction *m_aboutKcm;
    KAction *m_searchBoxAction;
    
    QWidget *m_cWidget;
    KLineEdit *m_searchText;
    
    QPushButton *m_helpButton;
    QPushButton *m_exportButton;
    QPushButton *m_goButton;
    
    SidePanel *m_sideMenu;
    ToolTipManager *m_toolTips;
};

#endif //INFOCENTER
