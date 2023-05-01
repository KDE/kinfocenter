/*
 * SPDX-FileCopyrightText: 2001 Alexander Neundorf <neundorf@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <KPluginFactory>
#include <KQuickConfigModule>

#include "networkmodel.h"

class KCMNic : public KQuickConfigModule
{
    Q_OBJECT

public:
    explicit KCMNic(QObject *parent, const KPluginMetaData &data)
        : KQuickConfigModule(parent, data)
    {
        qmlRegisterType<NetworkModel>("org.kde.kinfocenter.nic.private", 1, 0, "NetworkModel");
    }
};

K_PLUGIN_CLASS_WITH_JSON(KCMNic, "kcm_nic.json")

#include "nic.moc"
