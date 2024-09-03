// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2024 Harald Sitter <sitter@kde.org>

#include <QStandardPaths>

#include <KPluginFactory>
#include <KQuickConfigModule>

#include "CommandOutputContext.h"

using namespace Qt::StringLiterals;

class KCMEdid : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(CommandOutputContext *infoOutputContext READ outputContext CONSTANT FINAL)
public:
    explicit KCMEdid(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        const QString executable =
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kinfocenter/edid/edid.sh"), QStandardPaths::LocateFile);
        m_outputContext = new CommandOutputContext({QStringLiteral("di-edid-decode")}, QStringLiteral("/bin/sh"), {executable}, parent);
    }
    [[nodiscard]] CommandOutputContext *outputContext() const
    {
        return m_outputContext;
    }

private:
    CommandOutputContext *m_outputContext;
};

K_PLUGIN_CLASS_WITH_JSON(KCMEdid, "kcm_edid.json")

#include "main.moc"
