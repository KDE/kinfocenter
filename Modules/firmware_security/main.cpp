/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KPackage/PackageLoader>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

#include <CommandOutputContext.h>

class KCMFirmwareSecurity : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    explicit KCMFirmwareSecurity(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
        : ConfigModule(parent, data, args)
    {
        auto *about = new KAboutData(QStringLiteral("kcm_firmware_security"),
                                     i18nc("@label kcm name", "Firmware Security"),
                                     QStringLiteral("1.0"),
                                     QString(),
                                     KAboutLicense::GPL);
        about->addAuthor(i18n("Harald Sitter"), QString(), QStringLiteral("sitter@kde.org"));
        setAboutData(about);

        KPackage::Package package = KPackage::PackageLoader::self()->loadPackage(QStringLiteral("KPackage/GenericQML"));
        package.setDefaultPackageRoot(QStringLiteral("kpackage/kcms"));
        package.setPath(about->componentName());

        auto outputContext = new CommandOutputContext({QStringLiteral("fwupdmgr"), QStringLiteral("aha")},
                                                      QStringLiteral("/bin/sh"),
                                                      {package.path() + QStringLiteral("contents/code/fwupdmgr.sh")},
                                                      parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.firmware_security.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMFirmwareSecurity, "kcm_firmware_security.json")

#include "main.moc"