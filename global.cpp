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

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */

#include "global.h"

#include <kservicegroup.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kuser.h>

#include <QObject>
#include <QByteArray>
#include <QWidget>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>

bool KCGlobal::_root = false;
QStringList KCGlobal::_types;
QString KCGlobal::_uname = "";
QString KCGlobal::_hname = "";
QString KCGlobal::_kdeversion = "";
QString KCGlobal::_isystem = "";
QString KCGlobal::_irelease = "";
QString KCGlobal::_iversion = "";
QString KCGlobal::_imachine = "";
QString KCGlobal::_baseGroup = "";

void KCGlobal::init() {
	char buf[256];
	buf[0] = '\0';
	if (!gethostname(buf, sizeof(buf)))
		buf[sizeof(buf)-1] ='\0';
	QString hostname(buf);
	setHostName(hostname);

	KUser user;
	setUserName(user.loginName());
	setRoot(user.isSuperUser());

	setKDEVersion(KDE::versionString());

	struct utsname info;
	uname(&info);

	setSystemName(info.sysname);
	setSystemRelease(info.release);
	setSystemVersion(info.version);
	setSystemMachine(info.machine);
}

void KCGlobal::setType(const QByteArray& s) {
	QString string = s.toLower();
	_types = string.split( ',');
}

QString KCGlobal::baseGroup() {
	if (_baseGroup.isEmpty() ) {
		KServiceGroup::Ptr group = KServiceGroup::baseGroup("info");
		if (group) {
			_baseGroup = group->relPath();
			kDebug() << "Found basegroup = " << _baseGroup;
			return _baseGroup;
		}
		// Compatibility with old behaviour, in case of missing .directory files.
		if (_baseGroup.isEmpty()) {
			kWarning() << "No K menu group with X-KDE-BaseGroup=info found ! Defaulting to Settings/Information/";
			_baseGroup = QLatin1String("Settings/Information/");
		}
	}
	return _baseGroup;
}
