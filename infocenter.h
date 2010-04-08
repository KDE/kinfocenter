
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

//C++
#include <iostream>

//KDE
#include <KLocale>
#include <KIcon>
#include <KMenuBar>
#include <KMenu>
#include <KHelpMenu>
#include <KConfig>
#include <KGlobal>
#include <KConfigGroup>
#include <KDebug>
#include <KCModule>
#include <KToolInvocation>
#include <KActionMenu>
#include <KXmlGuiWindow>
#include <kxmlguifactory.h>
#include <KStandardAction>
#include <KActionCollection>
#include <KAboutApplicationDialog>
#include <KMessageBox>
#include <KFileDialog>
#include <KLineEdit>

//QT
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QSplitter>
#include <QObject>
#include <QProcess>
#include <QTextStream>
#include <QFile>

//Local
#include "sidepanel.h"
#include "kcmcontainer.h"
#include "infokcmmodel.h"
#include "kcmtreeitem.h"
#include "ToolTips/tooltipmanager.h"

class KcmContainer;
class SizePanel;

class KInfoCenter : public KXmlGuiWindow
{
  Q_OBJECT
  
  public:
    KInfoCenter();
    ~KInfoCenter();

  public slots:
    void itemClickedSlot(const KcmTreeItem *);
    void helpClickedSlot();
    void exportClickedSlot();
    void aboutKcmSlot();
    void searchSlot();
    
    static void showError(QWidget *,QString);
    
  private:
    void initMenuBar();
    void createButtonBar();
    void createMainFrame();
    void CreateMenuFrame();
    void closeDimentions(const QSize);
    void startupDimentions();
    void setButtons(const KCModule::Buttons buttons);
    void setKcm(const KcmTreeItem *);
    void resetCondition();

    KcmContainer *m_contain;
    QSplitter *m_splitter;
    QAction *m_aboutKcm;
    
    QWidget *m_cWidget;
    KLineEdit *m_searchText;
    
    QPushButton *m_helpButton;
    QPushButton *m_exportButton;
    QPushButton *m_goButton;
    
    SidePanel *m_sideMenu;
    ToolTipManager *m_toolTips;
};

#endif //__INFOCENTER__
