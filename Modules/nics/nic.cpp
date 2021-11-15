/*
 * SPDX-FileCopyrightText: 2001 Alexander Neundorf <neundorf@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nic.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>

#include "networkmodel.h"

K_PLUGIN_CLASS_WITH_JSON(KCMNic, "kcm_nic.json")

KCMNic::KCMNic(QObject *parent, const KPluginMetaData &data, const QVariantList &list)
    : KQuickAddons::ConfigModule(parent, data, list)
{
    KAboutData *about = new KAboutData(i18n("kcm_nic"),
                                       i18nc("@title:window", "Network Interfaces"),
                                       QString(),
                                       QString(),
                                       KAboutLicense::GPL,
                                       i18nc("@info", "(c) 2001 - 2002 Alexander Neundorf"));

    about->addAuthor(i18nc("@info:credit", "Alexander Neundorf"), i18nc("@info:credit", "creator"), QStringLiteral("neundorf@kde.org"));
    about->addAuthor(i18nc("@info:credit", "Carl Schwan"), i18nc("@info:credit", "developer"), QStringLiteral("carl@carlschwan.eu"));
    setAboutData(about);

    qmlRegisterType<NetworkModel>("org.kde.kinfocenter.nic.private", 1, 0, "NetworkModel");
}

#include "nic.moc"
