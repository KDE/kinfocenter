
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
  cLayout->setContentsMargins(0, 0, 0, 0);
  createMainFrame();
  createButtonBar();
  
  startupDimentions();
  initMenuBar();
  
  //TreeWidget
  connect(m_sideMenu,SIGNAL(clicked(const KcmTreeItem *)),this,SLOT(itemClickedSlot(const KcmTreeItem *)));
  
  //SearchBox
  connect(m_searchText, SIGNAL(textChanged(const QString&)), m_sideMenu, SLOT(filterSideMenuSlot(const QString&)));
  connect(m_searchBoxAction, SIGNAL(triggered(Qt::MouseButtons, Qt::KeyboardModifiers)),m_searchText, SLOT(setFocus()));
  
  //Buttons
  connect(m_helpButton,SIGNAL(clicked(bool)),this,SLOT(helpClickedSlot()));
  connect(m_exportButton,SIGNAL(clicked(bool)),this,SLOT(exportClickedSlot()));
  
  //Menu
  connect(m_aboutKcm, SIGNAL(triggered(bool) ), this, SLOT(aboutKcmSlot()));
  
  //Startup 
  m_searchText->completionObject()->setItems(m_sideMenu->allChildrenKeywords());
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
  disconnect(m_searchText, SIGNAL(textChanged(const QString&)), m_sideMenu, SLOT(filterSideMenuSlot(const QString&)));
  disconnect(m_searchBoxAction, SIGNAL(triggered(Qt::MouseButtons, Qt::KeyboardModifiers)),m_searchText, SLOT(setFocus()));
  
  //Buttons
  disconnect(m_helpButton,SIGNAL(clicked(bool)),this,SLOT(helpClickedSlot()));
  disconnect(m_exportButton,SIGNAL(clicked(bool)),this,SLOT(exportClickedSlot()));
  
  //Menu
  disconnect(m_aboutKcm, SIGNAL(triggered(bool) ), this, SLOT(aboutKcmSlot()));
  
  closeDimentions(this->size());
}

bool KInfoCenter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_sideMenu && event->type() == QEvent::Move) {
        m_contain->setKcmTopEdge(m_sideMenu->y());
    }

    return false;
}

void KInfoCenter::initMenuBar()
{ 
  KStandardAction::quit(this, SLOT(close()), actionCollection());
  KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());
  
  m_aboutKcm = actionCollection()->addAction("help_about_module");
  m_aboutKcm->setText(i18nc("Information about current module located in about menu","About Current Information Module"));
  m_aboutKcm->setIcon(KIcon("help"));
  m_aboutKcm->setEnabled(false);
  
  createGUI();

  QAction *aboutApp = actionCollection()->action("help_about_app");
  aboutApp->setIcon(KIcon("hwinfo"));
}

void KInfoCenter::createButtonBar()
{  
  QWidget *m_buttonBar = new QWidget();
  m_buttonBar->setContentsMargins(0,0,0,0);
  
  QHBoxLayout *bbLayout = new QHBoxLayout(m_buttonBar);
  bbLayout->setContentsMargins(0, 0, 0, 0);
  m_buttonBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
  
  m_helpButton = new QPushButton(i18nc("Help button label","Help"));
  m_helpButton->setIcon(KIcon("help"));
  m_helpButton->setEnabled(false);
  m_helpButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  m_helpButton->setToolTip(i18nc("Help Button ToolTip", "Opens help browser for selected module"));
  
  QWidget *m_blank = new QWidget();
  m_blank->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  
  m_exportButton = new QPushButton(i18nc("export button label","Export"));
  m_exportButton->setIcon(KIcon("document-export"));
  m_exportButton->setEnabled(false);
  m_exportButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  m_exportButton->setToolTip(i18nc("Export Button ToolTip", "Exports information for selected module"));
  
  bbLayout->addWidget(m_helpButton);
  bbLayout->addWidget(m_blank);
  bbLayout->addWidget(m_exportButton);
  
  m_cWidget->layout()->addWidget(m_buttonBar);
}

void KInfoCenter::createMainFrame()
{ 
  QWidget *mainDisplay = new QWidget();
  mainDisplay->setContentsMargins(0,0,0,0);
    
  QHBoxLayout *mainLayout = new QHBoxLayout(mainDisplay);
  
  m_splitter = new QSplitter(m_cWidget);
  m_splitter->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(m_splitter);
  
  CreateMenuFrame();
  
  m_contain = new KcmContainer(m_splitter);
  m_splitter->addWidget(m_contain);
  
  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 1);
  
  m_cWidget->layout()->addWidget(mainDisplay);
}
  
void KInfoCenter::CreateMenuFrame() 
{
  QWidget *sideFrame = new QWidget(m_splitter);
  sideFrame->setContentsMargins(0,0,0,0);
  
  QVBoxLayout *menuLayout = new QVBoxLayout(sideFrame);
  menuLayout->setContentsMargins(0, 0, 0, 0);
  
  m_searchText = new KLineEdit(sideFrame);
  m_searchText->setClearButtonShown(true);
  m_searchText->setClickMessage( i18nc( "Search Bar Click Message", "Search" ) );
  m_searchText->setCompletionMode( KGlobalSettings::CompletionPopup );
  m_searchText->completionObject()->setIgnoreCase(true);
  
  m_searchBoxAction = new KAction(sideFrame);
  m_searchBoxAction->setDefaultWidget(m_searchText);
  m_searchBoxAction->setShortcut(KShortcut(QKeySequence(Qt::CTRL + Qt::Key_F)));
  actionCollection()->addAction("searchText",m_searchBoxAction);
  m_searchText->show();
  
  m_sideMenu = new SidePanel(sideFrame);
  m_sideMenu->installEventFilter(this);
  
  menuLayout->addWidget(m_searchText);
  menuLayout->addWidget(m_sideMenu);
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
  if (buttons & KCModule::Export) m_exportButton->setEnabled(true);
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
  QString moduleName = m_contain->modulesName();
  
  if(m_contain->exportText().isEmpty())
  {
    KInfoCenter::showError(this,i18n("Export of the module has produced no output."));
    return;
  }
  
  QString fileName = KFileDialog::getSaveFileName(KUrl(moduleName + ".txt"),QString(),this);
  if(fileName.isEmpty()) return;
  
  QFile exportFile(fileName);

  if(!exportFile.open(QIODevice::WriteOnly)){
    KInfoCenter::showError(this,i18n("Unable to open file to write export information"));
    return;
  }
  
  QTextStream exportTextStream( &exportFile );
  exportTextStream << (i18n("Export information for %1",moduleName))
  << "\n\n" << m_contain->exportText() << endl;
  
  exportFile.close();
  KInfoCenter::showError(this,i18n("Information exported"));
}

void KInfoCenter::aboutKcmSlot()
{
  KAboutApplicationDialog kcmAboutDialog(m_contain->kcmAboutData());
  kcmAboutDialog.exec();
}

void KInfoCenter::showError(QWidget *parent, QString errorMessage)
{
  KMessageBox::sorry(parent,errorMessage);
}
