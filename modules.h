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

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */

#ifndef MODULES_H
#define MODULES_H

#include <QObject>
#include <QVBoxLayout>

#include <kcmoduleinfo.h>
#include <kiconloader.h>

class KAboutData;
class KCModule;
class ProxyWidget;
class QVBoxLayout;
class KVBox;

class ConfigModule : public QObject, public KCModuleInfo {
	Q_OBJECT

public:

	ConfigModule(const KService::Ptr &s);
	~ConfigModule();

	bool isActive() {
		return _module != 0;
	}
	ProxyWidget *module();
	const KAboutData *aboutData() const;
	
	QPixmap realIcon(KIconLoader::StdSizes size);

public slots:

	void deleteClient();

private:

	ProxyWidget *_module;
	QVBoxLayout *_embedLayout;
	KVBox *_embedFrame;

};

class ConfigModuleList : public QList<ConfigModule*> {
public:

	ConfigModuleList();

	bool readDesktopEntries();

};

#endif
