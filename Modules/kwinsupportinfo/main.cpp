/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2022 Nate Graham <nate@kde.org>
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>
#include <QLibraryInfo>

#include <CommandOutputContext.h>

class KCMKWinSupportInfo : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    explicit KCMKWinSupportInfo(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
        : ConfigModule(parent, data, args)
    {
        auto outputContext = new CommandOutputContext(QLibraryInfo::location(QLibraryInfo::BinariesPath) + QStringLiteral("/qdbus"),
                                                      {QStringLiteral("org.kde.KWin"), QStringLiteral("/KWin"), QStringLiteral("supportInformation")},
                                                      parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.kwinsupportinfo.private", 1, 0, "InfoOutputContext", outputContext);

        auto *about = new KAboutData(QStringLiteral("kcm_kwinsupportinfo"),
                                     i18nc("@label kcm name", "Window Manager"),
                                     QStringLiteral("1.0"),
                                     QString(),
                                     KAboutLicense::GPL);
        about->addAuthor(i18n("Nate Graham"), QString(), QStringLiteral("nate@kde.org"));
        setAboutData(about);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMKWinSupportInfo, "kcm_kwinsupportinfo.json")

#include "main.moc"