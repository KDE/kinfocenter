
/*
 *  main.cpp
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
#include "main.h"

//KDE
#include <kaboutdata.h>
#include <kcmdlineargs.h>

KicApp::KicApp() : KApplication()
{
	display = new KInfoCenter();
	display->show();
}

int main(int argc, char *argv[])
{
	KLocale::setMainCatalog("kinfocenter");

	KAboutData aboutKInfoCenter( "kinfocenter", 0, ki18n("KDE Info Center"),
		KDE_VERSION_STRING, ki18n("The KDE Info Center"), KAboutData::License_GPL,
		ki18n("(c) 2009-2010, The KDE SC KInfocenter Development Team"));

	QByteArray argv_0 = argv[0];
	KAboutData *aboutData = &aboutKInfoCenter;

	aboutData->addAuthor(ki18n("David Hubner"),ki18n("Current Maintainer"), "hubnerd@ntlworld.com");
	aboutData->addAuthor(ki18n("Helge Deller"), ki18n("Previous Maintainer"), "deller@kde.org");
	aboutData->addAuthor(ki18n("Matthias Hoelzer-Kluepfel"),KLocalizedString(), "hoelzer@kde.org");
	aboutData->addAuthor(ki18n("Matthias Elter"),KLocalizedString(), "elter@kde.org");
	aboutData->addAuthor(ki18n("Matthias Ettrich"),KLocalizedString(), "ettrich@kde.org");
	aboutData->addAuthor(ki18n("Waldo Bastian"),KLocalizedString(), "bastian@kde.org");
	aboutData->addAuthor(ki18n("Nicolas Ternisien"), KLocalizedString(), "nicolas.ternisien@gmail.com");

	KCmdLineOptions options;
	KCmdLineArgs::addCmdLineOptions( options );

	KCmdLineArgs::init( argc, argv, aboutData );

	KicApp Kic;
	return Kic.exec();
}
