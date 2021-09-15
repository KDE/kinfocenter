/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

#include <CommandOutputContext.h>

class KCMXServer : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    explicit KCMXServer(QObject *parent, const QVariantList &args)
        : ConfigModule(parent, args)
    {
        auto outputContext = new CommandOutputContext(QStringLiteral("lspci"), {QStringLiteral("-v")}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.pci.private", 1, 0, "InfoOutputContext", outputContext);

        auto *about = new KAboutData(QStringLiteral("kcm_pci"), i18nc("@label kcm name", "PCI"), QStringLiteral("1.0"), QString(), KAboutLicense::GPL);
        about->addAuthor(i18n("Harald Sitter"), QString(), QStringLiteral("sitter@kde.org"));
        setAboutData(about);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMXServer, "kcm_pci.json")

#include "main.moc"
