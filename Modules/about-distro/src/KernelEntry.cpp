/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "KernelEntry.h"

#include <sys/utsname.h>

KernelEntry::KernelEntry()
    : Entry(ki18n("Kernel Version:"), kernelVersion())
{
}

QString KernelEntry::kernelVersion()
{
    struct utsname utsName;
    if (uname(&utsName) == 0) {
        return QString::fromLatin1(utsName.release);
    }
    return QString();
}
