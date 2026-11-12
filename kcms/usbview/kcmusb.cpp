/*
 *   SPDX-FileCopyrightText: 2026 Tobias Fella <tobias.fella@kde.org>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kcmusb.h"

#include <KPluginFactory>

#include "CommandOutputContext.h"

using namespace Qt::StringLiterals;

K_PLUGIN_CLASS_WITH_JSON(USBViewer, "kcmusb.json")

USBViewer::USBViewer(QObject *parent, const KPluginMetaData &data)
    : KQuickConfigModule(parent, data)
    , m_outputContext(new CommandOutputContext(u"lsusb"_s, {u"-tv"_s}, this))
{
    // Pick up devices being plugged in or removed.
    m_outputContext->setAutoRefreshMs(1000);
}

CommandOutputContext *USBViewer::outputContext() const
{
    return m_outputContext;
}

#include "kcmusb.moc"

#include "moc_kcmusb.cpp"
