/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

#include <CommandOutputContext.h>

class KCMInterrupts : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    explicit KCMInterrupts(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
        : ConfigModule(parent, data, args)
    {
        auto outputContext =
#if defined(Q_OS_FREEBSD)
            new CommandOutputContext(QStringLiteral("vmstat"), {QStringLiteral("-i")}, parent);
#else
            new CommandOutputContext(QStringLiteral("cat"), {QStringLiteral("/proc/interrupts")}, parent);
#endif
        qmlRegisterSingletonInstance("org.kde.kinfocenter.interrupts.private", 1, 0, "InfoOutputContext", outputContext);

        auto *about = new KAboutData(QStringLiteral("kcm_interrupts"), i18nc("@label kcm name", "Interrupts"), QStringLiteral("1.0"), QString(), KAboutLicense::GPL);
        about->addAuthor(i18n("Harald Sitter"), QString(), QStringLiteral("sitter@kde.org"));
        setAboutData(about);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMInterrupts, "kcm_interrupts.json")

#include "main.moc"
