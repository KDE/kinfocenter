/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>
*/

#include <KPluginFactory>
#include <KQuickConfigModule>

#include "CommandOutputContext.h"

using namespace Qt::StringLiterals;

class KCMBlockDevices : public KQuickConfigModule
{
    Q_OBJECT
public:
    explicit KCMBlockDevices(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        // NOTE: careful when using -o, it tends to incorrectly print multiple mountpoints as a single path
        // (e.g. when different btrfs subvolumes are mounted at various points in the system it ought to enumerate all mountpoints)
        auto outputContext = new CommandOutputContext(u"lsblk"_s, {}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.block_devices.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMBlockDevices, "kcm_block_devices.json")

#include "main.moc"
