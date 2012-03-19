
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

//Local
#include "infocenter.h"

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
#include <KToolInvocation>
#include <KActionMenu>
#include <kxmlguifactory.h>
#include <KStandardAction>
#include <KActionCollection>
#include <KAboutApplicationDialog>
#include <KMessageBox>
#include <KFileDialog>
#include <KShortcut>
#include <KToolBar>

//QT
#include <QGridLayout>
#include <QProcess>
#include <QTextStream>
#include <QFile>
#include <QKeySequence>

KInfoCenter::KInfoCenter() : KXmlGuiWindow( 0, Qt::WindowContextHelpButtonHint )
{
	setWindowIcon(KIcon("hwinfo"));
	setWindowTitle(i18nc("Main window title", "KInfocenter"));
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_cWidget = new QWidget(this);
	setCentralWidget(m_cWidget);

	QVBoxLayout *cLayout = new QVBoxLayout(m_cWidget);
	Q_UNUSED(cLayout);

	cLayout->setSpacing(0);
	cLayout->setContentsMargins(0, 0, 0, 0);
	createMainFrame();
	createToolBar();

	//TreeWidget
	connect(m_sideMenu,SIGNAL(activated(const KcmTreeItem*)),this,SLOT(itemClickedSlot(const KcmTreeItem*)));

	//SearchBox
	connect(m_searchText, SIGNAL(textChanged(QString)), m_sideMenu, SLOT(filterSideMenuSlot(QString)));
	connect(m_searchAction, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)),m_searchText,SLOT(setFocus()));

	//Buttons
	connect(m_moduleHelpAction, SIGNAL(triggered(bool)),this,SLOT(helpClickedSlot()));
	connect(m_exportAction, SIGNAL(triggered(bool)),this,SLOT(exportClickedSlot()));

	//Menu
	connect(m_aboutKcm, SIGNAL(triggered(bool)), this, SLOT(aboutKcmSlot()));

	//Startup
	m_searchText->completionObject()->setItems(m_sideMenu->allChildKeywords());
	m_sideMenu->setFocus(Qt::OtherFocusReason);
	m_sideMenu->changeToFirstValidItem();

	m_toolTips = new ToolTipManager(m_sideMenu);
	setupGUI(QSize(640,480), ToolBar | Keys | Save | Create,"kinfocenterui.rc");

	m_helpAction->setMenu( dynamic_cast<KMenu*>( factory()->container("help", this) ) );
	menuBar()->hide();

	QAction *aboutApp = actionCollection()->action("help_about_app");
	aboutApp->setIcon(KIcon("hwinfo"));
}

KInfoCenter::~KInfoCenter()
{
	delete m_toolTips;

	//TreeWidget
	disconnect(m_sideMenu,SIGNAL(activated(const KcmTreeItem*)),this,SLOT(itemClickedSlot(const KcmTreeItem*)));

	//SearchBox
	disconnect(m_searchText, SIGNAL(textChanged(QString)), m_sideMenu, SLOT(filterSideMenuSlot(QString)));
	disconnect(m_searchAction, SIGNAL(triggered(Qt::MouseButtons,Qt::KeyboardModifiers)),m_searchText, SLOT(setFocus()));

	//Buttons
	disconnect(m_moduleHelpAction, SIGNAL(triggered(bool)),this,SLOT(helpClickedSlot()));
	disconnect(m_exportAction, SIGNAL(triggered(bool)),this,SLOT(exportClickedSlot()));

	//Menu
	disconnect(m_aboutKcm, SIGNAL(triggered(bool)), this, SLOT(aboutKcmSlot()));
}

bool KInfoCenter::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == m_sideMenu && event->type() == QEvent::Move)
	{
		m_contain->setKcmTopEdge(m_sideMenu->y());
	}
	return false;
}

void KInfoCenter::createToolBar()
{
	KStandardAction::quit(this, SLOT(close()), actionCollection());
	KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

	toolBar()->setMovable(false);

	m_aboutKcm = actionCollection()->addAction("help_about_module");
	m_aboutKcm->setText(i18nc("Information about current module located in about menu","About Current Information Module"));
	m_aboutKcm->setIcon(KIcon("help"));
	m_aboutKcm->setEnabled(false);

	m_exportAction = new KAction(this);
	m_exportAction->setText(i18nc("Export button label", "Export"));
	m_exportAction->setIcon(KIcon("document-export"));

	m_moduleHelpAction = new KAction(this);
	m_moduleHelpAction->setText(i18nc("Module help button label", "Module Help"));
	m_moduleHelpAction->setIcon(KIcon("help-contextual"));

	m_helpAction = new KActionMenu( KIcon("help-contents"), i18nc("Help button label","Help"), this );
	m_helpAction->setDelayed( false );

	actionCollection()->addAction("export", m_exportAction);
	actionCollection()->addAction("helpModule", m_moduleHelpAction);
	actionCollection()->addAction("helpMenu", m_helpAction);
}

void KInfoCenter::createMainFrame()
{
	QWidget *mainDisplay = new QWidget();
	mainDisplay->setContentsMargins(0,0,0,0);

	QHBoxLayout *mainLayout = new QHBoxLayout(mainDisplay);

	m_splitter = new QSplitter(m_cWidget);
	m_splitter->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(m_splitter);

	createMenuFrame();

	m_contain = new KcmContainer(m_splitter);
	m_splitter->addWidget(m_contain);

	m_splitter->setStretchFactor(0, 0);
	m_splitter->setStretchFactor(1, 1);

	m_cWidget->layout()->addWidget(mainDisplay);
}

void KInfoCenter::createMenuFrame()
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

	m_searchAction = new KAction(this);
	m_searchAction->setShortcut(KShortcut(QKeySequence(Qt::CTRL + Qt::Key_F)));
	m_searchAction->setText(i18nc("Kaction search label", "Search Modules"));
	m_searchAction->setIcon(KIcon("search"));

	actionCollection()->addAction("search",m_searchAction);

	m_sideMenu = new SidePanel(sideFrame);
	m_sideMenu->installEventFilter(this);

	menuLayout->addWidget(m_searchText);
	menuLayout->addWidget(m_sideMenu);
	m_splitter->addWidget(sideFrame);
}

void KInfoCenter::itemClickedSlot(const KcmTreeItem *item)
{
	resetCondition();
	if(item->type() != KcmTreeItem::CATEGORY) setKcm(item);
}

void KInfoCenter::setKcm(const KcmTreeItem *kcmItem)
{
	m_contain->setKcm(kcmItem->kcm());

	setButtons(m_contain->buttons());
	m_aboutKcm->setEnabled(true);
}

void KInfoCenter::setButtons(const KCModule::Buttons buttons)
{
	if (buttons & KCModule::Help) m_moduleHelpAction->setEnabled(true);
	if (buttons & KCModule::Export) m_exportAction->setEnabled(true);
}

void KInfoCenter::resetCondition()
{
	m_moduleHelpAction->setEnabled(false);
	m_exportAction->setEnabled(false);

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
	QString moduleName = m_contain->name();

	if(m_contain->exportText().isEmpty())
	{
		KInfoCenter::showError(this,i18n("Export of the module has produced no output."));
		return;
	}

	QString fileName = KFileDialog::getSaveFileName(KUrl(moduleName + ".txt"),QString(),this);
	if(fileName.isEmpty()) return;

	QFile exportFile(fileName);

	if(!exportFile.open(QIODevice::WriteOnly))
	{
		KInfoCenter::showError(this,i18n("Unable to open file to write export information"));
		return;
	}

	QTextStream exportTextStream( &exportFile );
	exportTextStream << (i18n("Export information for %1", moduleName))
		<< "\n\n" << m_contain->exportText() << endl;

	exportFile.close();
	KInfoCenter::showError(this, i18n("Information exported"));
}

void KInfoCenter::aboutKcmSlot()
{
	KAboutApplicationDialog kcmAboutDialog(m_contain->kcmAboutData());
	kcmAboutDialog.exec();
}

void KInfoCenter::showError(QWidget *parent, const QString& errorMessage)
{
	KMessageBox::sorry(parent, errorMessage);
}
