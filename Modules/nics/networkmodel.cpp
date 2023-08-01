/*
 * SPDX-FileCopyrightText: 2001 Alexander Neundorf <neundorf@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 * SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "networkmodel.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "config-nic.h"

#include <array>

#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include <QHash>

#include <net/if.h>
#include <sys/ioctl.h>

#include <ifaddrs.h>
#include <netdb.h>

using HostNameArray = std::array<char, NI_MAXHOST>;

QString flags_tos(unsigned int flags);

NetworkModel::NetworkModel(QObject *parent)
    : QAbstractListModel(parent)
{
    update();
}

QVariant NetworkModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);
    if (index.isValid()) {
        const auto nic = m_nics.at(index.row());

        switch (role) {
        case NetworkModel::Roles::NameRole:
            return nic->name;
        case NetworkModel::Roles::AddrRole:
            return nic->addr;
        case NetworkModel::Roles::NetMaskRole:
            return nic->netmask;
        case NetworkModel::Roles::TypeRole:
            return nic->type;
        case NetworkModel::Roles::HWAddrRole:
            return nic->HWaddr;
        case NetworkModel::Roles::StateRole:
            return nic->state;
        }
    }
    return QVariant{};
}

QHash<int, QByteArray> NetworkModel::roleNames() const
{
    return {{NetworkModel::Roles::NameRole, QByteArrayLiteral("name")},
            {NetworkModel::Roles::AddrRole, QByteArrayLiteral("address")},
            {NetworkModel::Roles::NetMaskRole, QByteArrayLiteral("netmask")},
            {NetworkModel::Roles::TypeRole, QByteArrayLiteral("type")},
            {NetworkModel::Roles::HWAddrRole, QByteArrayLiteral("hardwareAddress")},
            {NetworkModel::Roles::StateRole, QByteArrayLiteral("state")}};
}

int NetworkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_nics.size();
}

QList<NetworkModel::MyNIC *> findNICs();

void NetworkModel::update()
{
    beginResetModel();
    m_nics = findNICs();
    endResetModel();
}

// Convenience wrapper around sa_len being available or not.
static int getNameInfo(struct sockaddr *addr, struct ifaddrs *ifa, HostNameArray &hostOut)
{
    hostOut.fill(0);
#ifdef HAVE_STRUCT_SOCKADDR_SA_LEN
    return getnameinfo(addr, ifa->ifa_addr->sa_len, hostOut.data(), hostOut.size(), nullptr, 0, NI_NUMERICHOST);
#else
    return getnameinfo(addr,
                       (ifa->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                       hostOut.data(),
                       hostOut.size(),
                       nullptr,
                       0,
                       NI_NUMERICHOST);
#endif
}

QList<NetworkModel::MyNIC *> findNICs()
{
    QList<NetworkModel::MyNIC *> nl;

    struct ifaddrs *ifap = nullptr;
    if (getifaddrs(&ifap) != 0) {
        return nl;
    }

    for (auto *ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            qDebug() << "stumbled over an interface without ifa_addr. You may wish to file a bug against kinfocenter" << ifa->ifa_name << ifa->ifa_flags;
            continue;
        }

        switch (ifa->ifa_addr->sa_family) {
        case AF_INET6:
        case AF_INET: {
            auto tmp = new NetworkModel::MyNIC;
            tmp->name = ifa->ifa_name;

            HostNameArray hostBuffer;

            getNameInfo(ifa->ifa_addr, ifa, hostBuffer);
            tmp->addr = hostBuffer.data();

            if (ifa->ifa_netmask != nullptr) {
                getNameInfo(ifa->ifa_netmask, ifa, hostBuffer);
                tmp->netmask = hostBuffer.data();
            }

            tmp->state = (ifa->ifa_flags & IFF_UP) ? true : false;
            tmp->type = flags_tos(ifa->ifa_flags);

            nl.append(tmp);
            break;
        }
        default:
            break;
        }
    }

    freeifaddrs(ifap);
    return nl;
}

QString flags_tos(unsigned int flags)
{
    QString tmp;
    if (flags & IFF_POINTOPOINT) {
        tmp += i18nc("@item:intable Network type", "Point to Point");
    }

    if (flags & IFF_BROADCAST) {
        if (tmp.length()) {
            tmp += QLatin1String(", ");
        }
        tmp += i18nc("@item:intable Network type", "Broadcast");
    }

    if (flags & IFF_MULTICAST) {
        if (tmp.length()) {
            tmp += QLatin1String(", ");
        }
        tmp += i18nc("@item:intable Network type", "Multicast");
    }

    if (flags & IFF_LOOPBACK) {
        if (tmp.length()) {
            tmp += QLatin1String(", ");
        }
        tmp += i18nc("@item:intable Network type", "Loopback");
    }
    return tmp;
}

#include "moc_networkmodel.cpp"
