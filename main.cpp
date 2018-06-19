
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
#include <KWindowSystem>

//Qt
#include <QCommandLineParser>

KicApp::KicApp(int &argc, char **argv)
    : QApplication(argc, argv)
{
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    KLocalizedString::setApplicationDomain("kinfocenter");

    const auto displayName = i18n("Info Center");
    KAboutData aboutData( QStringLiteral("kinfocenter"), displayName,
        PROJECT_VERSION, displayName, KAboutLicense::GPL,
        i18n("Copyright 2009-2018 KDE"));

    aboutData.addAuthor(i18n("David Hubner"),i18n("Current Maintainer"), QStringLiteral("hubnerd@ntlworld.com"));
    aboutData.addAuthor(i18n("Helge Deller"), i18n("Previous Maintainer"), QStringLiteral("deller@kde.org"));
    aboutData.addAuthor(i18n("Matthias Hoelzer-Kluepfel"),QString(), QStringLiteral("hoelzer@kde.org"));
    aboutData.addAuthor(i18n("Matthias Elter"), QString(), QStringLiteral("elter@kde.org"));
    aboutData.addAuthor(i18n("Matthias Ettrich"), QString(), QStringLiteral("ettrich@kde.org"));
    aboutData.addAuthor(i18n("Waldo Bastian"), QString(), QStringLiteral("bastian@kde.org"));
    aboutData.addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("hwinfo")));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(*this);
    aboutData.processCommandLine(&parser);

    auto *service = new KDBusService(KDBusService::Unique, this);

    display = new KInfoCenter();
    display->show();

    connect(service, &KDBusService::activateRequested, this, [this]() {
        KWindowSystem::forceActiveWindow(display->winId());
    });
}

int main(int argc, char *argv[])
{
    KicApp Kic(argc, argv);

    KCrash::initialize();

    return Kic.exec();
}
