/***************************************************************************
 *   Copyright (C) 2015 Pinak Ahuja <pinak.ahuja@gmail.com>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "kcm.h"

#include <KPluginFactory>
#include <KPluginLoader>
#include <KAboutData>
#include <QDebug>

#include <QtQml>
#include <QQmlEngine>
#include <QQmlContext>

#include <KLocalizedString>

K_PLUGIN_FACTORY_WITH_JSON(KCMLookandFeelFactory, "kcm_fileindexermonitor.json", registerPlugin<KCMFileIndexer>();)

using namespace KQuickAddons;

KCMFileIndexer::KCMFileIndexer(QObject *parent, const QVariantList &args) : ConfigModule(parent, args)
{
    KAboutData *about = new KAboutData(QStringLiteral("kcm_fileindexermonitor"), i18n("File Indexer Monitor"),
                                       QStringLiteral("0.1"), QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Pinak Ahuja"), QString(), QStringLiteral("pinak.ahuja@gmail.com"));
    setAboutData(about);
}


#include "kcm.moc"
