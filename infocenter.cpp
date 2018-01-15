
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
#include <KLocalizedString>
#include <QIcon>
#include <KConfig>
#include <KToolInvocation>
#include <KActionMenu>
#include <kxmlguifactory.h>
#include <KStandardAction>
#include <KActionCollection>
#include <KAboutApplicationDialog>
#include <KMessageBox>
#include <KToolBar>

//QT
#include <QUrl>
#include <QProcess>
#include <QTextStream>
#include <QFile>
#include <QKeySequence>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>

KInfoCenter::KInfoCenter() : KXmlGuiWindow( 0, Qt::WindowContextHelpButtonHint )
{
    setWindowIcon(QIcon::fromTheme(QStringLiteral("hwinfo")));
    setWindowTitle(i18nc("Main window title", "KInfocenter"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_cWidget = new QWidget(this);
    setCentralWidget(m_cWidget);

    QVBoxLayout *cLayout = new QVBoxLayout(m_cWidget);

    cLayout->setSpacing(0);
    cLayout->setContentsMargins(0, 0, 0, 0);
    createMainFrame();
    createToolBar();

    //TreeWidget
    connect(m_sideMenu,SIGNAL(clicked(const KcmTreeItem*)),this,SLOT(itemClickedSlot(const KcmTreeItem*)));

    //SearchBox
    connect(m_searchText, &QLineEdit::textChanged, m_sideMenu, &SidePanel::filterSideMenuSlot);
    connect(m_searchAction, SIGNAL(triggered()),m_searchText,SLOT(setFocus()));

    //Buttons
    connect(m_moduleHelpAction, &QAction::triggered,this,&KInfoCenter::helpClickedSlot);

    //Menu
    connect(m_aboutKcm, &QAction::triggered, this, &KInfoCenter::aboutKcmSlot);

    //Startup
    m_searchText->completionObject()->setItems(m_sideMenu->allChildKeywords());
    m_sideMenu->setFocus(Qt::OtherFocusReason);
    m_sideMenu->changeToFirstValidItem();

    m_toolTips = new ToolTipManager(m_sideMenu);
    setupGUI(QSize(640,480), ToolBar | Keys | Save | Create, QStringLiteral("kinfocenterui.rc"));

    m_helpAction->setMenu( dynamic_cast<QMenu*>( factory()->container(QStringLiteral("help"), this) ) );
    menuBar()->hide();

    QAction *aboutApp = actionCollection()->action(QStringLiteral("help_about_app"));
    aboutApp->setIcon(QIcon::fromTheme(QStringLiteral("hwinfo")));
}

KInfoCenter::~KInfoCenter()
{
    delete m_toolTips;

    //TreeWidget
    disconnect(m_sideMenu,SIGNAL(clicked(const KcmTreeItem*)),this,SLOT(itemClickedSlot(const KcmTreeItem*)));

    //SearchBox
    disconnect(m_searchText, &QLineEdit::textChanged, m_sideMenu, &SidePanel::filterSideMenuSlot);
    disconnect(m_searchAction, SIGNAL(triggered()),m_searchText, SLOT(setFocus()));

    //Buttons
    disconnect(m_moduleHelpAction, &QAction::triggered, this, &KInfoCenter::helpClickedSlot);

    //Menu
    disconnect(m_aboutKcm, &QAction::triggered, this, &KInfoCenter::aboutKcmSlot);
}

bool KInfoCenter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_sideMenu && event->type() == QEvent::Move) {
        m_contain->setKcmTopEdge(m_sideMenu->y());
    }
    return false;
}

void KInfoCenter::createToolBar()
{
    KStandardAction::quit(this, &KInfoCenter::close, actionCollection());
    KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

    toolBar()->setMovable(false);

    m_aboutKcm = actionCollection()->addAction(QStringLiteral("help_about_module"));
    m_aboutKcm->setText(i18nc("Information about current module located in about menu","About Current Information Module"));
    m_aboutKcm->setIcon(QIcon::fromTheme(QStringLiteral("help-about")));
    m_aboutKcm->setEnabled(false);

    m_moduleHelpAction = new QAction(this);
    m_moduleHelpAction->setText(i18nc("Module help button label", "Module Help"));
    m_moduleHelpAction->setIcon(QIcon::fromTheme(QStringLiteral("help-contextual")));

    m_helpAction = new KActionMenu( QIcon::fromTheme(QStringLiteral("help-contents")), i18nc("Help button label","Help"), this );
    m_helpAction->setDelayed( false );

    actionCollection()->addAction(QStringLiteral("helpModule"), m_moduleHelpAction);
    actionCollection()->addAction(QStringLiteral("helpMenu"), m_helpAction);
}

void KInfoCenter::createMainFrame()
{
    QWidget *mainDisplay = new QWidget();
    mainDisplay->setContentsMargins(0,0,0,0);

    QHBoxLayout *mainLayout = new QHBoxLayout(mainDisplay);

    m_splitter = new QSplitter(m_cWidget);
    m_splitter->setContentsMargins(0, 0, 0, 0);
    m_splitter->setHandleWidth(5);
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
    m_searchText->setClearButtonEnabled(true);
    m_searchText->setPlaceholderText( i18nc( "Search Bar Click Message", "Search" ) );
    m_searchText->setCompletionMode( KCompletion::CompletionPopup );
    m_searchText->completionObject()->setIgnoreCase(true);

    m_searchAction = new QAction(this);
    m_searchAction->setText(i18nc("Kaction search label", "Search Modules"));
    m_searchAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-find")));

    actionCollection()->addAction(QStringLiteral("search"),m_searchAction);
    actionCollection()->setDefaultShortcut(m_searchAction, QKeySequence(Qt::CTRL + Qt::Key_F));

    m_sideMenu = new SidePanel(sideFrame);
    m_sideMenu->installEventFilter(this);
    connect(m_sideMenu, &SidePanel::clearSearchLine, this, &KInfoCenter::clearSearchLine);

    menuLayout->addWidget(m_searchText);
    menuLayout->addWidget(m_sideMenu);
    m_splitter->addWidget(sideFrame);
}

void KInfoCenter::clearSearchLine()
{
    m_searchText->clear();
}

void KInfoCenter::itemClickedSlot(const KcmTreeItem *item)
{
	resetCondition();
	if(item->type() != KcmTreeItem::CATEGORY) {
        setKcm(item);
    }
}

void KInfoCenter::setKcm(const KcmTreeItem *kcmItem)
{
    if (m_contain->fileName() != kcmItem->kcm().fileName()) {
        m_contain->setKcm(kcmItem->kcm());

        setButtons(m_contain->buttons());
        m_aboutKcm->setEnabled(true);
    }
}

void KInfoCenter::setButtons(const KCModule::Buttons buttons)
{
    if (buttons & KCModule::Help) {
        m_moduleHelpAction->setEnabled(true);
    }
}

void KInfoCenter::resetCondition()
{
    m_moduleHelpAction->setEnabled(false);

    m_aboutKcm->setEnabled(false);
}

void KInfoCenter::helpClickedSlot()
{
    // Nicked from Ben, could not use KToolInvocation due to docpath.
    QString docPath = m_contain->helpPath();

    QUrl url(QStringLiteral("help:/") + docPath );
    QProcess::startDetached(QStringLiteral("khelpcenter"), QStringList() << url.url());
}

void KInfoCenter::aboutKcmSlot()
{
    KAboutApplicationDialog kcmAboutDialog(*(m_contain->kcmAboutData()));
    kcmAboutDialog.exec();
}

void KInfoCenter::showError(QWidget *parent, const QString& errorMessage)
{
    KMessageBox::sorry(parent, errorMessage);
}
