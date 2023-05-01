/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickConfigModule>

#include <CommandOutputContext.h>

class KCMInterrupts : public KQuickConfigModule
{
    Q_OBJECT
public:
    explicit KCMInterrupts(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        auto outputContext =
#if defined(Q_OS_FREEBSD)
            new CommandOutputContext(QStringLiteral("vmstat"), {QStringLiteral("-i")}, parent);
#else
            new CommandOutputContext(QStringLiteral("cat"), {QStringLiteral("/proc/interrupts")}, parent);
#endif
        qmlRegisterSingletonInstance("org.kde.kinfocenter.interrupts.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMInterrupts, "kcm_interrupts.json")

#include "main.moc"
