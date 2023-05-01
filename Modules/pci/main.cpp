/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2022 Tobias C. Berner <tcberner@FreeBSD.org>
*/

#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickConfigModule>

#include <CommandOutputContext.h>

#include "backend.h"

class KCMXServer : public KQuickConfigModule
{
    Q_OBJECT
public:
    explicit KCMXServer(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        auto outputContext = new CommandOutputContext(QStringLiteral(PCI_BACKEND), {QStringLiteral(PCI_BACKEND_ARGUMENTS)}, parent);
        qmlRegisterSingletonInstance("org.kde.kinfocenter.pci.private", 1, 0, "InfoOutputContext", outputContext);
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMXServer, "kcm_pci.json")

#include "main.moc"
