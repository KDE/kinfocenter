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

#include "proxywidget.h"
#include "global.h"

#include <QByteArray>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include <QLabel>

#include <kpushbutton.h>
#include <klocale.h>
#include <kapplication.h>
#include <kcmodule.h>
#include <kseparator.h>
#include <kdialog.h>
#include <kstandardguiitem.h>
#include <kdebug.h>

#include <unistd.h> // for getuid()
#include "proxywidget.moc"


ProxyWidget::ProxyWidget(KCModule *client) :
	QWidget(), _changed(false), _client(client) {

	kDebug() << "Creating Proxy Widget..." << endl;

	if (_client->quickHelp().isEmpty() == false )
		setWhatsThis(_client->quickHelp());
	else
		setWhatsThis(i18n("The currently loaded configuration module."));

	QVBoxLayout *top = new QVBoxLayout(this);
	top->setMargin(0);
	top->setSpacing(0);
	top->addWidget(_client);

	kDebug() << "Proxy Widget created" << endl;
}

ProxyWidget::~ProxyWidget() {
	delete _client;
}

QString ProxyWidget::quickHelp() const {
	if (_client)
		return _client->quickHelp();
	else
		return "";
}

const KAboutData *ProxyWidget::aboutData() const {
	return _client->aboutData();
}
