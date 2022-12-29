/*
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2022 Linus Dierheimer <linus@dierheimer.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

#include <CommandOutputContext.h>

class KCMOpenCL : public KQuickAddons::ConfigModule
{
    Q_OBJECT
public:
    explicit KCMOpenCL(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
        : ConfigModule(parent, data, args)
    {
        auto outputContext = new CommandOutputContext(QStringLiteral("clinfo"), {}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.opencl.private", 1, 0, "InfoOutputContext", outputContext);

        auto *about = new KAboutData(QStringLiteral("kcm_opencl"), i18nc("@label kcm name", "OpenCL"), QStringLiteral("1.0"), QString(), KAboutLicense::GPL);
        about->addAuthor(i18n("Linus Dierheimer"), QString(), QStringLiteral("linus@dierheimer.de"));
        setAboutData(about);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMOpenCL, "kcm_opencl.json")

#include "main.moc"
