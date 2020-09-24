/**
 * SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QObject>
#include <memory>
#include <optional>

#include "Entries.h"
#include "CPUEntry.h"
#include "GPUEntry.h"
#include "PlasmaEntry.h"
#include "BitEntry.h"
#include "MemoryEntry.h"
#include "KernelEntry.h"

class Entries : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString plasma READ plasma NOTIFY plasmaChanged)
    Q_PROPERTY(QString frameworks READ frameworks NOTIFY frameworksChanged)
    Q_PROPERTY(QString qt READ qt NOTIFY qtChanged)
    Q_PROPERTY(QString kernel READ kernel NOTIFY kernelChanged)
    Q_PROPERTY(QString bit READ bit NOTIFY bitChanged)
    Q_PROPERTY(QString cpus READ cpus NOTIFY cpusChanged)
    Q_PROPERTY(QString memory READ memory NOTIFY memoryChanged)
    Q_PROPERTY(QString gpu READ gpu NOTIFY gpuChanged)
    
public:
    Entries(QObject *parent = nullptr);
    
    QString plasma() const;
    QString frameworks() const;
    QString qt() const;
    QString kernel() const;
    QString bit() const;
    QString cpus();
    QString memory() const;
    QString gpu() const;
    
Q_SIGNALS:
    void plasmaChanged();
    void frameworksChanged();
    void qtChanged();
    void kernelChanged();
    void bitChanged();
    void cpusChanged();
    void memoryChanged();
    void gpuChanged();
    
private:
    std::optional<PlasmaEntry> m_plasma;
    std::optional<Entry> m_frameworks;
    std::optional<Entry> m_qt;
    std::optional<KernelEntry> m_kernel;
    std::optional<BitEntry> m_bit;
    std::optional<CPUEntry> m_cpus;
    std::optional<MemoryEntry> m_memory;
    std::optional<GPUEntry> m_gpu;
    
};
