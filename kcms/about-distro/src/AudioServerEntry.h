/*
    SPDX-FileCopyrightText: 2025 Ismael Asensio <isma.af@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef AUDIOSERVERENTRY_H
#define AUDIOSERVERENTRY_H

#include "Entry.h"

namespace PulseAudioQt
{
class Context;
}

class AudioServerEntry : public Entry
{
    Q_OBJECT

public:
    AudioServerEntry();

private:
    QString audioServerInfo();
    void update();

private:
    PulseAudioQt::Context *m_context;
};

#endif // AUDIOSERVERENTRY_H
