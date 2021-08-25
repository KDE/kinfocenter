/*
    SPDX-FileCopyrightText: 2012-2021 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "PlasmaEntry.h"

#include <KConfigGroup>
#include <KDesktopFile>
#include <QStandardPaths>

#include "Version.h"

PlasmaEntry::PlasmaEntry()
    : Entry(ki18n("KDE Plasma Version:"), plasmaVersion())
{
    // Since Plasma version detection isn't based on a library query it can fail
    // in weird cases; instead of admitting defeat we simply hide everything :P
    if (localizedValue().isEmpty()) {
        return;
    }
}

QString PlasmaEntry::plasmaVersion()
{
    // KInfoCenter is part of Plasma. Simply show our version!
    return QString::fromLatin1(PROJECT_VERSION);
}
