
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
#include <KCrash>
#include <KLocalizedString>
#include <config-workspace.h>
#include <KDBusService>

//Qt
#include <QCommandLineParser>

KicApp::KicApp(int &argc, char **argv)
    : QApplication(argc, argv)
{
    KLocalizedString::setApplicationDomain("kinfocenter");

    const auto displayName = i18n("Info Center");
    QApplication::setApplicationDisplayName(displayName);
    QApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QApplication::setWindowIcon(QIcon::fromTheme("hwinfo"));

    KAboutData aboutData( QStringLiteral("kinfocenter"), displayName,
        PROJECT_VERSION, displayName, KAboutLicense::GPL,
        i18n("Copyright 2009-2016 KDE"));

    aboutData.addAuthor(i18n("David Hubner"),i18n("Current Maintainer"), QStringLiteral("hubnerd@ntlworld.com"));
    aboutData.addAuthor(i18n("Helge Deller"), i18n("Previous Maintainer"), QStringLiteral("deller@kde.org"));
    aboutData.addAuthor(i18n("Matthias Hoelzer-Kluepfel"),QString(), QStringLiteral("hoelzer@kde.org"));
    aboutData.addAuthor(i18n("Matthias Elter"), QString(), QStringLiteral("elter@kde.org"));
    aboutData.addAuthor(i18n("Matthias Ettrich"), QString(), QStringLiteral("ettrich@kde.org"));
    aboutData.addAuthor(i18n("Waldo Bastian"), QString(), QStringLiteral("bastian@kde.org"));
    aboutData.addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(*this);
    aboutData.processCommandLine(&parser);

    display = new KInfoCenter();
    display->show();

    KDBusService* service = new KDBusService(KDBusService::Unique, this);
}

int main(int argc, char *argv[])
{
    KicApp Kic(argc, argv);

    KCrash::initialize();

    return Kic.exec();
}
