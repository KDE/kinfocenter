/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef MEMORYENTRY_H
#define MEMORYENTRY_H

#include "Entry.h"

class MemoryEntry : public Entry
{
public:
    MemoryEntry();

    // Overwrite to get correct localization for the value.
    QString localizedValue(Language language = Language::System) const final;
    QString localizedHelp(Language language = Language::System) const final;

private:
    static std::optional<qlonglong> calculateTotalRam();
    static std::optional<qlonglong> calculateAvailableRam();

    std::optional<qlonglong> m_totalRam = calculateTotalRam();
    std::optional<qlonglong> m_availableRam = calculateAvailableRam();
};

#endif // MEMORYENTRY_H
