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

#ifndef __PROXYWIDGET_H__
#define __PROXYWIDGET_H__


class KCModule;
class KAboutData;

#include "dockcontainer.h"

#include <QPointer>


class ProxyWidget : public QWidget {
	Q_OBJECT

public:

	ProxyWidget(KCModule *client);
	~ProxyWidget();

	QString quickHelp() const;
	const KAboutData *aboutData() const;

	bool isChanged() {
		return _changed;
	}
	void setChanged(bool changed) {
		_changed = changed;
	}


private:

	bool _changed;
	
	// Just in case the module was deleted from outside (i.e. by unloading the module)
	QPointer<KCModule> _client;

};

#endif
