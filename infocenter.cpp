
/*
 *  infocenter.cpp
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

#include "infocenter.h"

KInfoCenter::KInfoCenter() : KXmlGuiWindow( 0, Qt::WindowContextHelpButtonHint )
{ 
  setWindowIcon(KIcon("hwinfo"));
  setWindowTitle(i18nc("Main window title", "KInfocenter"));
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  m_cWidget = new QWidget(this);
  setCentralWidget(m_cWidget);
  
  QVBoxLayout *cLayout = new QVBoxLayout(m_cWidget);
  Q_UNUSED(cLayout); // Is used but compiler is stupid
  
  cLayout->setSpacing(0);
  
  createMainFrame();
  createButtonBar();
  
  startupDimentions();
  initMenuBar();
  
  //TreeWidget
  connect(m_sideMenu,SIGNAL(clicked(const KcmTreeItem *)),this,SLOT(itemClickedSlot(const KcmTreeItem *)));
  
  //SearchBox
  connect(m_searchText,SIGNAL(editingFinished()),this,SLOT(searchSlot()));
  
  //Buttons
  connect(m_helpButton,SIGNAL(clicked(bool)),this,SLOT(helpClickedSlot()));
  connect(m_exportButton,SIGNAL(clicked(bool)),this,SLOT(exportClickedSlot()));
  connect(m_goButton,SIGNAL(clicked(bool)),this,SLOT(searchSlot()));
  
  //Startup
  m_sideMenu->setFocus(Qt::OtherFocusReason);
  m_sideMenu->changeToRootSelection();
  
  m_toolTips = new ToolTipManager(m_sideMenu);
  show();
}

KInfoCenter::~KInfoCenter()
{ 
  delete m_toolTips;
  
  //TreeWidget
  disconnect(m_sideMenu,SIGNAL(clicked(const KcmTreeItem *)),this,SLOT(itemClickedSlot(const KcmTreeItem *)));
  
  //SearchBox
  disconnect(m_searchText,SIGNAL(editingFinished()),this,SLOT(searchSlot()));
  
  //Buttons
  disconnect(m_helpButton,SIGNAL(clicked(bool)),this,SLOT(helpClickedSlot()));
  disconnect(m_exportButton,SIGNAL(clicked(bool)),this,SLOT(exportClickedSlot()));
  disconnect(m_goButton,SIGNAL(clicked(bool)),this,SLOT(searchSlot()));
  
  closeDimentions(this->size());
}

void KInfoCenter::initMenuBar()
{ 
  KStandardAction::quit(this, SLOT(close()), actionCollection());
  KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());
  
  m_aboutKcm = actionCollection()->addAction("help_about_module");
  m_aboutKcm->setText(i18nc("Information about current module located in about menu","About Current Information Module"));
  m_aboutKcm->setIcon(KIcon("help"));
  m_aboutKcm->setEnabled(false);
  connect(m_aboutKcm, SIGNAL(triggered(bool) ), this, SLOT(aboutKcmSlot()));
  
  createGUI();

  QAction *aboutApp = actionCollection()->action("help_about_app");
  aboutApp->setIcon(KIcon("hwinfo"));
}

void KInfoCenter::createButtonBar()
{  
  QWidget *m_buttonBar = new QWidget(); 
  QHBoxLayout *m_bbLayout = new QHBoxLayout(m_buttonBar);
  
  m_buttonBar->setFixedHeight(35);
  
  m_helpButton = new QPushButton(i18nc("Help button label","Help"));
  m_helpButton->setEnabled(false);
  m_helpButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  
  QWidget *m_blank = new QWidget();
  m_blank->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  
  m_exportButton = new QPushButton(i18nc("export button label","Export"));
  m_exportButton->setEnabled(false);
  m_exportButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  
  m_bbLayout->addWidget(m_helpButton);
  m_bbLayout->addWidget(m_blank);
  m_bbLayout->addWidget(m_exportButton);
  
  m_cWidget->layout()->addWidget(m_buttonBar);
}

void KInfoCenter::createMainFrame()
{ 
  QWidget *mainDisplay = new QWidget();
  m_cWidget->layout()->addWidget(mainDisplay);
  
  QHBoxLayout *mainLayout = new QHBoxLayout(mainDisplay);
  
  m_splitter = new QSplitter(m_cWidget);
  m_splitter->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(m_splitter);
  
  CreateMenuFrame();
  
  m_contain = new KcmContainer(m_splitter);
  m_splitter->addWidget(m_contain);
  
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
}
  
void KInfoCenter::CreateMenuFrame() 
{
  QWidget *sideFrame = new QWidget(m_splitter);
  QGridLayout *menuLayout = new QGridLayout(sideFrame);
  
  m_searchText = new QLineEdit(sideFrame);
  menuLayout->addWidget(m_searchText,0,0,1,1);
  
  m_goButton = new QPushButton("Search");
  m_goButton->setFixedWidth(70);
  menuLayout->addWidget(m_goButton,0,1,1,2);
  
  m_sideMenu = new SidePanel(sideFrame);
  menuLayout->addWidget(m_sideMenu,1,0,1,3);
  
  m_splitter->addWidget(sideFrame);
}
  
void KInfoCenter::startupDimentions()
{ 
  KConfigGroup config = KConfigGroup(KGlobal::config(), "General");
  int height = config.readEntry<int>("MainWindowHeight",0);
  int width = config.readEntry<int>("MainWindowWidth",0);

  if(height && width) this->resize(width,height);
}
  
void KInfoCenter::closeDimentions(const QSize winSizes) 
{ 
  KConfigGroup config = KConfigGroup(KGlobal::config(), "General");
  
  config.writeEntry("MainWindowHeight", winSizes.height());
  config.writeEntry("MainWindowWidth", winSizes.width());

  config.sync();
}


void KInfoCenter::itemClickedSlot(const KcmTreeItem *item) 
{ 
  resetCondition();
  if(item->isValid() == false) return;

  setKcm(item);
}

void KInfoCenter::setKcm(const KcmTreeItem *kcmItem) 
{ 
  m_contain->setKcm(kcmItem->kcm());
  
  setButtons(m_contain->buttons());
  m_aboutKcm->setEnabled(true);
}

void KInfoCenter::setButtons(const KCModule::Buttons buttons) 
{    
  if (buttons & KCModule::Help) m_helpButton->setEnabled(true);
  //if (buttons & KCModule::Export) m_exportButton->setEnabled(true);
}

void KInfoCenter::resetCondition()
{
  m_helpButton->setEnabled(false);
  m_exportButton->setEnabled(false);

  m_aboutKcm->setEnabled(false);
}

void KInfoCenter::helpClickedSlot() 
{ 
  // Nicked from Ben, could not use KToolInvocation due to docpath.  
  QString docPath = m_contain->helpPath();

  KUrl url( KUrl("help:/"), docPath );
  QProcess::startDetached("khelpcenter", QStringList() << url.url());
}

void KInfoCenter::exportClickedSlot() 
{ 
  #warning: Export unfinished
  if(m_contain->exportText().isEmpty())
  {
    return;
  }
}

void KInfoCenter::aboutKcmSlot()
{
  KAboutApplicationDialog kcmAboutDialog(m_contain->kcmAboutData());
  kcmAboutDialog.exec();
}

void KInfoCenter::searchSlot()
{
  QString searchText = m_searchText->text();
  
  if(searchText.isEmpty())
  {
    KInfoCenter::showError(this,i18n("You have not entered any search string."));
    return;
  }
  m_sideMenu->filterSideMenu(searchText);
  m_sideMenu->expandAllSlot();
}

void KInfoCenter::showError(QWidget *parent, QString errorMessage)
{
  KMessageBox::sorry(parent,errorMessage);
}