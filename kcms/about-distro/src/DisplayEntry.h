/*
 *    SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
 *    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef DISPLAYENTRY_H
#define DISPLAYENTRY_H

#include "Entry.h"
#include <QScreen>

class DisplayEntry : public Entry
{
public:
    DisplayEntry();
    static QString displayLabel();
    static QString displayInformation();
};

#endif // DISPLAYENTRY_H
