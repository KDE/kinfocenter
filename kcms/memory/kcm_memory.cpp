/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Thomas Duckworth <tduck973564@gmail.com>
    SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>
*/

#include "kcm_memory.h"

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>

KCMMemory::KCMMemory(QObject *parent, const KPluginMetaData &data)
    : KQuickConfigModule(parent, data)
{
    loadData();
}

QString KCMMemory::memoryInformation() const
{
    return m_memoryInformation;
}

void KCMMemory::loadData()
{
    KAuth::Action action(QStringLiteral("org.kde.kinfocenter.dmidecode.memoryinformation"));
    action.setHelperId(QStringLiteral("org.kde.kinfocenter.dmidecode"));
    KAuth::ExecuteJob *job = action.execute();

    connect(job, &KJob::result, this, [this, job]() {
        if (job->error()) {
            qWarning() << "Failed to retrieve memory information: " << job->errorString();
            return;
        }

        const auto reply = job->data();
        if (reply.contains("memory")) {
            m_memoryInformation = reply["memory"].toString();
        }

        Q_EMIT changed();
    });

    job->start();
}
