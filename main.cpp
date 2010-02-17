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

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * Howto debug:
 *    start "kinfocenter --nofork" in a debugger.
 *
 * If you want to test with command line arguments you need
 * -after you have started kinfocenter in the debugger-
 * open another shell and run kinfocenter with the desired
 * command line arguments.
 *
 * The command line arguments will be passed to the version of
 * kinfocenter in the debugger via DCOP and will cause a call
 * to newInstance().
 */

#include "main.h"
#include "toplevel.h"
#include "global.h"

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kapplication.h>

#include <QByteArray>
#include <QDesktopWidget>

#include "main.moc"

KInfoCenterApp::KInfoCenterApp() :
	KUniqueApplication() {

	toplevel = new TopLevel();

	QRect desk = KGlobalSettings::desktopGeometry(toplevel);
	KConfigGroup config(KGlobal::config(), "General");
	// Initial size is:
	// never bigger than workspace as reported by desk
	// 940x700 on 96 dpi, 12 pt font
	// 800x600 on 72 dpi, 12 pt font
	// --> 368 + 6 x dpiX, 312 + 4 x dpiY
	// Adjusted for font size
	int fontSize = toplevel->fontInfo().pointSize();
	if (fontSize == 0)
		fontSize = (toplevel->fontInfo().pixelSize() * 72) / toplevel->logicalDpiX();
	int x = config.readEntry(QString::fromLatin1("InitialWidth %1").arg(desk.width()), qMin(desk.width(), 368 + (6*toplevel->logicalDpiX()*fontSize)/12) );
	int y = config.readEntry(QString::fromLatin1("InitialHeight %1").arg(desk.height()), qMin(desk.height(), 312 + (4*toplevel->logicalDpiX()*fontSize)/12) );

	toplevel->resize(x, y);

	toplevel->show();
}

KInfoCenterApp::~KInfoCenterApp() {
	if (toplevel!=NULL) {
		KConfigGroup config(KGlobal::config(), "General");
		QDesktopWidget *desk = QApplication::desktop();
		config.writeEntry(QString::fromLatin1("InitialWidth %1").arg(desk->width()), toplevel->width());
		config.writeEntry(QString::fromLatin1("InitialHeight %1").arg(desk->height()), toplevel->height());
		config.sync();
	}
}

extern "C"KDE_EXPORT int kdemain(int argc, char *argv[])
{
	KLocale::setMainCatalog("kinfocenter");

	KAboutData aboutKInfoCenter( "kinfocenter", 0, ki18n("KDE Info Center"),
			KDE_VERSION_STRING, ki18n("The KDE Info Center"), KAboutData::License_GPL,
			ki18n("(c) 1998-2004, The KDE Control Center Developers"));

	QByteArray argv_0 = argv[0];
	KAboutData *aboutData = &aboutKInfoCenter;

	aboutData->addAuthor(ki18n("Nicolas Ternisien"), ki18n("Current Maintainer"), "nicolas.ternisien@gmail.com");
	aboutData->addAuthor(ki18n("Helge Deller"), ki18n("Previous Maintainer"), "deller@kde.org");
	aboutData->addAuthor(ki18n("Matthias Hoelzer-Kluepfel"),KLocalizedString(), "hoelzer@kde.org");
	aboutData->addAuthor(ki18n("Matthias Elter"),KLocalizedString(), "elter@kde.org");
	aboutData->addAuthor(ki18n("Matthias Ettrich"),KLocalizedString(), "ettrich@kde.org");
	aboutData->addAuthor(ki18n("Waldo Bastian"),KLocalizedString(), "bastian@kde.org");

	KCmdLineArgs::init( argc, argv, aboutData );
	KUniqueApplication::addCmdLineOptions();

	KCGlobal::init();

	if (!KInfoCenterApp::start()) {
		kDebug() << "kinfocenter is already running!\n";
		return 0;
	}

	KInfoCenterApp app;

	return app.exec();
}
