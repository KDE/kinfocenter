/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Thomas Duckworth <tduck973564@gmail.com>
    SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>
*/

#include <KQuickConfigModule>

class KCMMemory : public KQuickConfigModule
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Set by C++ as root property 'kcm'")
    Q_PROPERTY(QString memoryInformation READ memoryInformation NOTIFY changed)
public:
    explicit KCMMemory(QObject *parent, const KPluginMetaData &data);
    [[nodiscard]] QString memoryInformation() const;

private:
    Q_SIGNAL void changed();

    /**
     * The memory information that will be displayed in the KCM.
     */
    QString m_memoryInformation;

    /**
     * Load the memory information from the dmidecode helper.
     */
    void loadData();
};
