/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "BitEntry.h"


BitEntry::BitEntry()
    : Entry(ki18n("OS Type:"), QString())
{
}

QString BitEntry::localizedValue(Entry::Language language) const
{
    const int bits = QT_POINTER_SIZE == 8 ? 64 : 32;
    return localize(ki18nc("@label %1 is the CPU bit width (e.g. 32 or 64)",
                           "%1-bit").subs(QString::number(bits)),
                    language);
}
