// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
// SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>

#pragma once

#include <KAuth/ActionReply>

class DMIDecodeHelper : public QObject
{
    Q_OBJECT

public:
    explicit DMIDecodeHelper(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * Retrieves memory (RAM) information from dmidecode.
     */
    KAuth::ActionReply memoryinformation(const QVariantMap &args);

    KAuth::ActionReply systeminformation(const QVariantMap &args);

private:
    KAuth::ActionReply executeDmidecode(const QStringList &arguments);

    /**
     * Path to the dmidecode binary.
     *
     * If the binary is not found, this helper will not work.
     */
    QString m_dmidecodePath;
};
