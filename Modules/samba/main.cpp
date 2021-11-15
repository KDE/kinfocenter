/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

#include "ksambasharemodel.h"
#include "smbmountmodel.h"

class SambaModule : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    SambaModule(QObject *parent, const KPluginMetaData &data, const QVariantList &list = QVariantList());
    ~SambaModule() override = default;
};

SambaModule::SambaModule(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, data, args)
{
    KAboutData *about = new KAboutData(i18n("kcmsamba"),
                                       i18n("Samba Status"),
                                       QString(),
                                       QString(),
                                       KAboutLicense::GPL,
                                       i18n("(c) 2002-2020 KDE Information Control Module Samba Team"));
    about->addAuthor(i18n("Michael Glauche"), QString(), QStringLiteral("glauche@isa.rwth-aachen.de"));
    about->addAuthor(i18n("Matthias Hoelzer"), QString(), QStringLiteral("hoelzer@kde.org"));
    about->addAuthor(i18n("David Faure"), QString(), QStringLiteral("faure@kde.org"));
    about->addAuthor(i18n("Harald Koschinski"), QString(), QStringLiteral("Harald.Koschinski@arcormail.de"));
    about->addAuthor(i18n("Wilco Greven"), QString(), QStringLiteral("greven@kde.org"));
    about->addAuthor(i18n("Alexander Neundorf"), QString(), QStringLiteral("neundorf@kde.org"));
    about->addAuthor(i18n("Harald Sitter"), QString(), QStringLiteral("sitter@kde.org"));
    setAboutData(about);

    qmlRegisterType<SmbMountModel>("org.kde.kinfocenter.samba", 1, 0, "MountModel");
    qmlRegisterType<KSambaShareModel>("org.kde.kinfocenter.samba", 1, 0, "ShareModel");

    setButtons(Help);
}

K_PLUGIN_CLASS_WITH_JSON(SambaModule, "smbstatus.json")

#include "main.moc"
