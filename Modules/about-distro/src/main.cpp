/*
    SPDX-FileCopyrightText: 2012 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <KPluginFactory>

#include "Module.h"

K_PLUGIN_FACTORY(KcmAboutDistroFactory,
                 registerPlugin<Module>(QStringLiteral("kcm-about-distro"));)

#include "main.moc"
