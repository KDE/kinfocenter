/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

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
    qmlRegisterType<SmbMountModel>("org.kde.kinfocenter.samba", 1, 0, "MountModel");
    qmlRegisterType<KSambaShareModel>("org.kde.kinfocenter.samba", 1, 0, "ShareModel");

    setButtons(Help);
}

K_PLUGIN_CLASS_WITH_JSON(SambaModule, "smbstatus.json")

#include "main.moc"
