/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "PlasmaEntry.h"

#include <QStandardPaths>
#include <KConfigGroup>
#include <KDesktopFile>

PlasmaEntry::PlasmaEntry()
    : Entry(ki18n("KDE Plasma Version:"), plasmaVersion())
{
    // Since Plasma version detection isn't based on a library query it can fail
    // in weird cases; instead of admitting defeat we simply hide everything :P
    if (value.isEmpty()) {
        return;
    }
}

QString PlasmaEntry::plasmaVersion()
{
    const QStringList &filePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                             QStringLiteral("xsessions/plasma.desktop"));

    if (filePaths.length() < 1) {
        return QString();
    }

    // Despite the fact that there can be multiple desktop files we simply take
    // the first one as users usually don't have xsessions/ in their $HOME
    // data location, so the first match should (usually) be the only one and
    // reflect the plasma session run.
    KDesktopFile desktopFile(filePaths.first());
    return desktopFile.desktopGroup().readEntry("X-KDE-PluginInfo-Version", QString());
}
