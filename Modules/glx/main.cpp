/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#include <KPluginFactory>
#include <KQuickConfigModule>

#include <CommandOutputContext.h>

class KCMXServer : public KQuickConfigModule
{
    Q_OBJECT
public:
    explicit KCMXServer(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        auto outputContext = new CommandOutputContext(QStringLiteral("glxinfo"), {}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.glx.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMXServer, "kcm_glx.json")

#include "main.moc"
