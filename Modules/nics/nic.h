/*
 * SPDX-FileCopyrightText: 2001 Alexander Neundorf <neundorf@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <KQuickAddons/ConfigModule>

class KCMNic : public KQuickAddons::ConfigModule
{
    Q_OBJECT

public:
    explicit KCMNic(QObject *parent = nullptr, const QVariantList &list = QVariantList());
    virtual ~KCMNic() override = default;
};
