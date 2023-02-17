/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

#include <CommandOutputContext.h>

class KCMVulkan : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    explicit KCMVulkan(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
        : ConfigModule(parent, data, args)
    {
        auto outputContext = new CommandOutputContext(QStringLiteral("vulkaninfo"), {}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.vulkan.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMVulkan, "kcm_vulkan.json")

#include "main.moc"
