/*
 *   SPDX-FileCopyrightText: 2015 Pinak Ahuja <pinak.ahuja@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kcm.h"

#include <KPluginFactory>
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
