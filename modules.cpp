/*
 Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>

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

#include "modules.h"
#include "global.h"
#include "proxywidget.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kservicegroup.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kauthorized.h>
#include <kservicetypetrader.h>
#include <kcmoduleloader.h>
#include <kvbox.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <QX11Info>
#include <QX11EmbedWidget>
#endif
#include <unistd.h>
#include <sys/types.h>

#include "modules.moc"

ConfigModule::ConfigModule(const KService::Ptr &s) :
	KCModuleInfo(s),  _module(0) {
}

ConfigModule::~ConfigModule() {
	deleteClient();
}

ProxyWidget *ConfigModule::module() {
	if (_module)
		return _module;

	kDebug() << "Finding proxy..." << endl;

	KCModule *modWidget = 0;

	modWidget = KCModuleLoader::loadModule(*this,/*KCModuleLoader::None*/(KCModuleLoader::ErrorReporting)NULL);

	if (modWidget==NULL) {

		kWarning() << "Unable to load KCM Module" << endl;
		return NULL;
	}
	
	_module = new ProxyWidget(modWidget);

	return _module;
}

QPixmap ConfigModule::realIcon(KIconLoader::StdSizes size) {
	//The next line is identical as SmallIcon(module->icon()), but is able to return a isNull() QPixmap
	QPixmap providedIcon = KIconLoader::global()->loadIcon(icon(), KIconLoader::Small, size, KIconLoader::DefaultState, QStringList(), 0L, true);
	if (providedIcon.isNull()) {
		kDebug() << "Icon is null" << icon() << endl;
		return SmallIcon("computer", size);

	}
	return providedIcon;

}

void ConfigModule::deleteClient() {
	kapp->syncX();

	if (_module)
		_module->close();
	_module = NULL;

}

const KAboutData *ConfigModule::aboutData() const {
	if (!_module)
		return 0;
	return _module->aboutData();
}

ConfigModuleList::ConfigModuleList() {
	foreach(ConfigModule* configModule, *this) {
		delete configModule;
	}
	
	this->clear();
}

bool ConfigModuleList::readDesktopEntries() {
	KService::List list = KServiceTypeTrader::self()->query("KCModule", "[X-KDE-ParentApp] == 'kinfocenter'");

	if (list.isEmpty()) {
		return false;
	}

	foreach(const KService::Ptr &s, list) {
		if (s->isType(KST_KService) == false) {
			continue;
		}

		if (!KAuthorized::authorizeControlModule(s->menuId())) {
			continue;
		}

		ConfigModule *module = new ConfigModule(s);
		if (module->library().isEmpty()) {
			delete module;
			continue;
		}

		append(module);

	}
	
	return true;
}
