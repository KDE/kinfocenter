/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>
*/

#include <KPluginFactory>
#include <KQuickConfigModule>

#include <CommandOutputContext.h>

using namespace Qt::StringLiterals;

class KCMNetwork : public KQuickConfigModule
{
    Q_OBJECT
public:
    explicit KCMNetwork(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        auto outputContext = new CommandOutputContext(u"ip"_s, {u"address"_s}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.network.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMNetwork, "kcm_network.json")

#include "main.moc"
