/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMAENTRY_H
#define PLASMAENTRY_H

#include "Entry.h"

class PlasmaEntry : public Entry
{
public:
    PlasmaEntry();
    static QString plasmaVersion();
};

#endif // PLASMAENTRY_H
