/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>
*/

#include <QGuiApplication>
#include <QStandardPaths>
#include <QStyleHints>

#include <KPluginFactory>
#include <KQuickConfigModule>

#include <CommandOutputContext.h>

using namespace Qt::StringLiterals;

class KCMNetwork : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(CommandOutputContext *infoOutputContext READ outputContext CONSTANT FINAL)
public:
    explicit KCMNetwork(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        const QString executable = QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"kinfocenter/network/ip.sh"_s, QStandardPaths::LocateFile);
        const QString darkness = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? u"1"_s : u"0"_s;
        m_outputContext = new CommandOutputContext({u"ip"_s, u"aha"_s}, u"/bin/sh"_s, {executable, darkness}, Qt::TextFormat::RichText, parent);
    }
    CommandOutputContext *outputContext() const
    {
        return m_outputContext;
    }

private:
    CommandOutputContext *m_outputContext;
};

K_PLUGIN_CLASS_WITH_JSON(KCMNetwork, "kcm_network.json")

#include "main.moc"
