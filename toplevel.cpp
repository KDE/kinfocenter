/*
 Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 Copyright (c) 2000 Matthias Elter <elter@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA  02110-1301, USA.
 */

#include "toplevel.h"
#include "indexwidget.h"
#include "aboutwidget.h"
#include "proxywidget.h"
#include "moduletreeview.h"
#include "modules.h"

#include <kaboutapplicationdialog.h>
#include <kactioncollection.h>
#include <kbugreport.h>
#include <kaboutdata.h>
#include <kconfig.h>
#include <kcomponentdata.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <kwindowsystem.h>
#include <kxmlguifactory.h>

#include <kdebug.h>

#include <QTabWidget>
#include <QSplitter>

#include <stdio.h>

#include "toplevel.moc"

TopLevel::TopLevel() :
	KXmlGuiWindow( 0, Qt::WindowContextHelpButtonHint), _active(NULL), dummyAbout(NULL) {
	setCaption(QString());

	report_bug = NULL;

	// initialize the entries
	_modules = new ConfigModuleList();
	_modules->readDesktopEntries();

	// create the layout box
	_splitter = new QSplitter( Qt::Horizontal, this );
	_splitter->setContentsMargins(0, 0, 0, 0);

	// index tab
	_indextab = new IndexWidget(_modules, this);
	connect(_indextab, SIGNAL(moduleActivated(ConfigModule*)), this, SLOT(activateModule(ConfigModule*)));
	connect(_indextab, SIGNAL(generalActivated()), this, SLOT(activateGeneral()));
	_splitter->addWidget(_indextab);

	_indextab->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred) );

	// Restore sizes
	KConfigGroup config = KConfigGroup(KGlobal::config(), "General");
	QList<int> sizes = config.readEntry("SplitterSizes", QList<int>() );
	if (!sizes.isEmpty())
		_splitter->setSizes(sizes);

	AboutWidget* aboutWidget = new AboutWidget( this, _modules);
	connect(aboutWidget, SIGNAL( moduleSelected( ConfigModule * ) ), _indextab, SLOT(selectModule(ConfigModule*)));

	// set up the right hand side (the docking area)
	_dock = new DockContainer(aboutWidget, _splitter);

	// That one does the trick ...
	_splitter->setStretchFactor(_splitter->indexOf(_indextab), 0);
	_splitter->setStretchFactor(_splitter->indexOf(_dock), 1);

	connect(_indextab, SIGNAL(generalActivated()), _dock, SLOT(showAboutWidget()));

	// set the main view
	setCentralWidget(_splitter);

	// initialize the GUI actions
	setupActions();

	KWindowSystem::setIcons(winId(), KIconLoader::global()->loadIcon(KINFOCENTER_ICON_NAME, KIconLoader::NoGroup, 32), KIconLoader::global()->loadIcon("hwinfo", KIconLoader::NoGroup, 16) );

	_indextab->selectGeneral();
}

TopLevel::~TopLevel() {

	KConfigGroup config = KConfigGroup(KGlobal::config(), "General");
	config.writeEntry("SplitterSizes", _splitter->sizes());

	config.sync();

	delete _modules;
}

void TopLevel::setupActions() {
	KStandardAction::quit(this, SLOT(close()), actionCollection());
	KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

	about_module = actionCollection()->addAction("help_about_module");
	about_module->setText(i18n("About Current Module"));
	about_module->setVisible(false);
	connect(about_module, SIGNAL(triggered(bool) ), SLOT(aboutModule()));

	createGUI("kinfocenterui.rc");

	report_bug = actionCollection()->action("help_report_bug");
	report_bug->setText(i18n("&Report Bug..."));
	report_bug->disconnect();
	connect(report_bug, SIGNAL(triggered()), SLOT(reportBug()));
}

void TopLevel::activateModule(ConfigModule *configModule) {
	kDebug() << "Activating module..." << endl;

	_active=configModule;

	// dock it
	if (!_dock->dockModule(configModule)) {
		kDebug() << "Activating module by docking it." << endl;

		return;
	}

	kDebug() << "Modifying About Module." << endl;

	if (configModule->aboutData()) {
		about_module->setText(i18nc("Help menu->about <modulename>", "About %1", handleAmpersand(configModule->moduleName())));
		about_module->setIcon(configModule->realIcon(KIconLoader::SizeSmall));
		about_module->setVisible(true);
	} else {
		about_module->setText(i18n("About Current Module"));
		about_module->setIcon(KIcon());
		about_module->setVisible(false);
	}

	setCaption(configModule->moduleName(), false);

	if (configModule->moduleName().isEmpty())
		report_bug->setText(i18n("&Report Bug..."));
	else
		report_bug->setText(i18n("Report Bug on Module %1...", handleAmpersand(configModule->moduleName())));


	kDebug() << "Activating module done." << endl;
}

void TopLevel::deleteDummyAbout() {
	delete dummyAbout;
	dummyAbout = 0;
}

void TopLevel::reportBug() {
	dummyAbout = NULL;
	bool deleteit = false;

	if (!_active) // report against kinfocenter
		dummyAbout = const_cast<KAboutData*>(KGlobal::mainComponent().aboutData());
	else {
		if (_active->aboutData())
			dummyAbout = const_cast<KAboutData*>(_active->aboutData());
		else {
			QString kcmLibrary = "kcm" + _active->library();
			dummyAbout = new KAboutData(kcmLibrary.toLatin1(), 0, ki18n(_active->moduleName().toUtf8()), "2.0");
			deleteit = true;
		}
	}
	KBugReport *br = new KBugReport(this, false, dummyAbout);
	if (deleteit)
		connect(br, SIGNAL(finished()), SLOT(deleteDummyAbout()));
	else
		dummyAbout = NULL;
	
	br->show();
}

void TopLevel::aboutModule() {
	kDebug() << "About " << _active->moduleName() << endl;
	
	KAboutApplicationDialog dlg(_active->aboutData());
	dlg.exec();
}

QString TopLevel::handleAmpersand(const QString& modName) const {
	QString modulename = modName;
	// double it
	if (modulename.contains( '&')) {
		for (int i = modulename.length(); i >= 0; --i)
			if (modulename[ i ] == '&')
				modulename.insert(i, "&");
	}
	
	return modulename;
}

void TopLevel::activateGeneral() {
	kDebug() << "Activating General..." << endl;

	about_module->setText(i18n("About Current Module"));
	about_module->setIcon(KIcon());
	about_module->setVisible(false);

	setCaption(i18n("General Information"), false);

	report_bug->setText(i18n("&Report Bug..."));
	
}
