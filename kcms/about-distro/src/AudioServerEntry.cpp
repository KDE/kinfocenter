/*
    SPDX-FileCopyrightText: 2025 Ismael Asensio <isma.af@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "AudioServerEntry.h"

#include <PulseAudioQt/Context>
#include <PulseAudioQt/Server>

using namespace Qt::StringLiterals;

AudioServerEntry::AudioServerEntry()
    : Entry(ki18n("Audio Server:"), QString())
    , m_context(PulseAudioQt::Context::instance())
{
    connect(m_context->server(), &PulseAudioQt::Server::isPipeWireChanged, this, &AudioServerEntry::update);
    connect(m_context->server(), &PulseAudioQt::Server::versionChanged, this, &AudioServerEntry::update);
    connect(m_context->server(), &PulseAudioQt::Server::hasWirePlumberChanged, this, &AudioServerEntry::update);

    update();
}

void AudioServerEntry::update()
{
    const QString value = audioServerInfo();
    if (value != m_value) {
        m_value = value;
        Q_EMIT updated();
    }
}

QString AudioServerEntry::audioServerInfo()
{
    const auto &server = m_context->server();

    QStringList info = {
        server->isPipeWire() ? u"PipeWire"_s : u"PulseAudio"_s,
        server->version(),
    };

    if (server->hasWirePlumber()) {
        info << u"(WirePlumber %1)"_s.arg(server->wirePlumberVersion());
    }

    return info.join(u" ").trimmed();
}

#include "moc_AudioServerEntry.cpp"
