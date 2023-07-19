/*  nicsignals.cpp
 *
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "nicsignals.h"

// KDE
#include <solid/control/networkinterface.h>
#include <solid/control/networkmanager.h>

NicSignals::NicSignals()
{
    connectToNicSignals();
}

void NicSignals::connectToNicSignals()
{
    const QList<Solid::Control::NetworkInterface *> nicList = Solid::Control::NetworkManager::networkInterfaces();

    for (const Solid::Control::NetworkInterface *nic : nicList) {
        connect(nic, SIGNAL(connectionStateChanged(int, int, int)), this, SLOT(nicChangedSignal(int, int, int)));
    }
}

void NicSignals::nicChangedSignal(int ns, int os, int reason)
{
    Q_UNUSED(os);
    Q_UNUSED(reason);
    if (ns == 8 || ns == 2) {
        Q_EMIT nicActivatedOrDisconnected();
    }
}

#include "moc_nicsignals.cpp"
