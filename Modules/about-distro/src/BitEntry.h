/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef BITENTRY_H
#define BITENTRY_H

#include "Entry.h"

#include <QString>

class BitEntry : public Entry
{
public:
    BitEntry();
    QString localizedValue(Language language = Language::System) const final;
};

#endif // BITENTRY_H
