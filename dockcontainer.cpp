/*
 Copyright (c) 2000 Matthias Elter <elter@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "dockcontainer.h"

#include <kapplication.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <kvbox.h>

#include <QLabel>
#include <QPixmap>
#include <QFont>
#include <QApplication>

#include "global.h"
#include "modules.h"
#include "proxywidget.h"
#include "aboutwidget.h"

#include "dockcontainer.moc"

ModuleTitle::ModuleTitle(QWidget *parent) :
	KTitleWidget(parent) {

}

ModuleTitle::~ModuleTitle() {
	
}

void ModuleTitle::showTitleFor(ConfigModule* config) {
	kDebug() << "Show title for" << endl;
	if ( !config)
		return;

	setWhatsThis(config->comment() );
	setCommentText(config->docPath(), config->comment(), config->module()->quickHelp());
	setPixmap(config->realIcon(KIconLoader::SizeLarge));
	setText(config->moduleName());
	
	kDebug() << "Show title for done" << endl;
}

void ModuleTitle::setCommentText(const QString& docPath, const QString& text, const QString& quickHelp) {
	if (text.isEmpty() && docPath.isEmpty())
		setCommentBaseText();
	else if (docPath.isEmpty())
		setComment(text);
	else {
		setComment(quickHelp + i18n("<p>Click here to consult the full <a href=\"%1\">Manual</a>.</p>", "help:/" + docPath));
	}
}

void ModuleTitle::setCommentBaseText() {
	setComment(i18n("<h1>KDE Info Center</h1>"
		"There is no quick help available for the active info module."
		"<br /><br />"
		"Click <a href = \"kinfocenter/index.html\">here</a> to read the general Info Center manual.") );
}

DockContainer::DockContainer(AboutWidget* aboutWidget, QWidget *parent) :
	QWidget(parent) {
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	
	_moduleTitle = new ModuleTitle(this);
	mainLayout->addWidget(_moduleTitle);
	
	_moduleWidgets = new QStackedWidget(this);
	mainLayout->addWidget(_moduleWidgets);
	
	_busyWidget = new QLabel(i18n("<big><b>Loading...</b></big>"), this);
	_busyWidget->setAlignment(Qt::AlignCenter);
	_busyWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	
	_moduleWidgets->addWidget(_busyWidget);
	
	_generalWidget = aboutWidget;
	_moduleWidgets->addWidget(_generalWidget);

	showAboutWidget();
}

DockContainer::~DockContainer() {
	
}


ProxyWidget* DockContainer::initializeModule(ConfigModule * module) {
	showBusyWidget();

	QApplication::setOverrideCursor(Qt::WaitCursor);

	ProxyWidget* proxy = module->module();

	if (proxy!=NULL) {
		//If this module was not in the stack, add it.
		if ( _moduleWidgets->indexOf(proxy) == -1) {
			_moduleWidgets->addWidget(proxy);
		}
		
	}

	QApplication::restoreOverrideCursor();

	return proxy;
}

bool DockContainer::dockModule(ConfigModule *module) {

	ProxyWidget* widget = initializeModule(module);
	
	if (widget==NULL) {
		kDebug() << "Failed to display module" << module->moduleName() << endl;
		showAboutWidget();
		
		return false;
		
	}
	
	if (widget == _moduleWidgets->currentWidget()) {
		kDebug() << "Module already displayed" << endl;
		return true;
	}
	
	if (widget->isChanged()) {

		int res = KMessageBox::warningYesNoCancel(this, module ? i18n("There are unsaved changes in the active module.\n"
			"Do you want to apply the changes before running "
			"the new module or discard the changes?") : i18n("There are unsaved changes in the active module.\n"
			"Do you want to apply the changes before exiting "
			"the System Settings or discard the changes?"), i18n("Unsaved Changes"), KStandardGuiItem::apply(), KStandardGuiItem::discard());
		if (res == KMessageBox::Cancel)
			return false;
	}

	kDebug() << "Docking module..." << endl;
	
	showConfigWidget(module);
	
	return true;
}

void DockContainer::showAboutWidget() {
	kDebug() << "Show About Widget" << endl;
	_moduleWidgets->setCurrentWidget(_generalWidget);
	
	_moduleTitle->hide();

}

void DockContainer::showBusyWidget() {
	kDebug() << "Show Busy Widget" << endl;
	_moduleWidgets->setCurrentWidget(_busyWidget);
	
	_moduleTitle->hide();

	kapp->processEvents();

}

void DockContainer::showConfigWidget(ConfigModule* module) {
	kDebug() << "Show Config Widget" << endl;
	
	_moduleTitle->showTitleFor(module);
	_moduleTitle->show();

	_moduleWidgets->setCurrentWidget(module->module());
}
