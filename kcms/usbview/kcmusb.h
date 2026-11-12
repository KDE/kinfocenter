/*
 *   SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <KQuickConfigModule>

class CommandOutputContext;

class USBViewer : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(CommandOutputContext *infoOutputContext READ outputContext CONSTANT FINAL)

public:
    explicit USBViewer(QObject *parent, const KPluginMetaData &data);

    CommandOutputContext *outputContext() const;

private:
    CommandOutputContext *const m_outputContext;
};
