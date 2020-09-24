/**
 * SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "Entries.h"
#include "FancyString.h"

Entries::Entries(QObject *parent)
    : QObject(parent)
{
}

QString Entries::bit() const
{
    if (!m_bit) {
        m_bit = std::optional<BitEntry>(BitEntry());
    }
    return m_bit.value();
}

QString Entries::cpus()
{
    if (!m_cpus) {
        m_cpus = std::optional<CPUEntry>(CPUEntry());
    }
    return m_cpus.value();
}

Entry *Entries::frameworks() const
{
    return m_frameworks.get();
}

Entry *Entries::gpu() const
{
    return m_gpu.get();
}

Entry *Entries::kernel() const
{
    return m_kernel.get();
}

Entry *Entries::memory() const
{
    return m_memory.get();
}

Entry *Entries::plasma() const
{
    return m_plasma.get();
}

Entry* Entries::qt() const
{
    return m_qt.get();
}









