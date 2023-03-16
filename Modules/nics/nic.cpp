/*
 * SPDX-FileCopyrightText: 2001 Alexander Neundorf <neundorf@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nic.h"

#include <KLocalizedString>
#include <KPluginFactory>

#include "networkmodel.h"

K_PLUGIN_CLASS_WITH_JSON(KCMNic, "kcm_nic.json")

KCMNic::KCMNic(QObject *parent, const KPluginMetaData &data, const QVariantList &list)
    : KQuickConfigModule(parent, data, list)
{
    qmlRegisterType<NetworkModel>("org.kde.kinfocenter.nic.private", 1, 0, "NetworkModel");
}

#include "nic.moc"
