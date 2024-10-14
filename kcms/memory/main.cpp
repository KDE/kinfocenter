/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Thomas Duckworth <tduck973564@gmail.com>
    SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>
*/

#include <KAuth/Action>
#include <KAuth/ExecuteJob>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickConfigModule>

class KCMMemory : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(QString memoryInformation READ memoryInformation NOTIFY changed)
public:
    explicit KCMMemory(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        loadData();
    }

    QString memoryInformation() const
    {
        return m_memoryInformation;
    }

private:
    Q_SIGNAL void changed();

    /**
     * The memory information that will be displayed in the KCM.
     */
    QString m_memoryInformation;

    /**
     * Load the memory information from the dmidecode helper.
     */
    void loadData()
    {
        KAuth::Action action(QStringLiteral("org.kde.kinfocenter.dmidecode.memoryinformation"));
        action.setHelperId(QStringLiteral("org.kde.kinfocenter.dmidecode"));
        KAuth::ExecuteJob *job = action.execute();

        connect(job, &KJob::result, this, [this, job]() {
            if (job->error()) {
                qWarning() << "Failed to retrieve memory information: " << job->errorString();
                return;
            } else {
                const auto reply = job->data();
                if (reply.contains("memory")) {
                    m_memoryInformation = reply["memory"].toString();
                }
            }

            Q_EMIT changed();
        });

        job->start();
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMMemory, "kcm_memory.json")

#include "main.moc"
