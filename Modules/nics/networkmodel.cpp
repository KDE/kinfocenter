/**
 * SPDX-FileCopyrightText: 2001 Alexander Neundorf <neundorf@kde.org>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-LicenseIndentifier: GPL-2.0-or-later
 */

#include "networkmodel.h"

#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "config-nic.h"
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif

#include <KLocalizedString>

#include <QDebug>
#include <QHash>

#include <net/if.h>
#include <sys/ioctl.h>
#include <KPluginFactory>
#include <KPluginLoader>


#if defined(HAVE_GETNAMEINFO) && defined(HAVE_GETIFADDRS)
#include <ifaddrs.h>
#include <netdb.h>

QString flags_tos(unsigned int flags);
#endif

NetworkModel::NetworkModel(QObject* parent)
    : QAbstractListModel(parent)
{
    update();
}

QVariant NetworkModel::data(const QModelIndex& index, int role) const
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
    return {
        {NetworkModel::Roles::NameRole, QByteArrayLiteral("name")},
        {NetworkModel::Roles::AddrRole, QByteArrayLiteral("address")},
        {NetworkModel::Roles::NetMaskRole, QByteArrayLiteral("netmask")},
        {NetworkModel::Roles::TypeRole, QByteArrayLiteral("type")},
        {NetworkModel::Roles::HWAddrRole, QByteArrayLiteral("hardwareAddress")},
        {NetworkModel::Roles::StateRole, QByteArrayLiteral("state")}
    };
}

int NetworkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_nics.size();
}

QList<NetworkModel::MyNIC *> findNICs();

void NetworkModel::update()
{
    m_nics = findNICs();
}

static QString HWaddr2String(const char *hwaddr)
{
    QString ret;
    for (int i=0; i<6; i++, hwaddr++) {
        int v = (*hwaddr & 0xff);
        QString num = QStringLiteral("%1").arg(v, 0, 16);
        if (num.length() < 2)
            num.prepend(QStringLiteral("0"));
        if (i>0)
            ret.append(QStringLiteral(":"));
        ret.append(num);
    }
    return ret;
}

// Convenience wrapper around sa_len being available or not.
static int getNameInfo(struct sockaddr *addr, struct ifaddrs *ifa, char *hostOut)
{
#ifdef HAVE_STRUCT_SOCKADDR_SA_LEN
    return getnameinfo(addr,
                       ifa->ifa_addr->sa_len,
                       hostOut, 127,
                       nullptr, 0,
                       NI_NUMERICHOST);
#else
    return getnameinfo(addr,
                       (ifa->ifa_addr->sa_family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                               sizeof(struct sockaddr_in6),
                       hostOut, NI_MAXHOST,
                       nullptr, 0,
                       NI_NUMERICHOST);
#endif
}

QList<NetworkModel::MyNIC*> findNICs() {

    QList<NetworkModel::MyNIC*> nl;

#if !defined(HAVE_GETIFADDRS) || !defined(HAVE_GETNAMEINFO)

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    char buf[8*1024];
    struct ifconf ifc;
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_req = (struct ifreq *) buf;
    int result=ioctl(sockfd, SIOCGIFCONF, &ifc);

    for (char* ptr = buf; ptr < buf + ifc.ifc_len;) {
        struct ifreq *ifr =(struct ifreq *) ptr;
#ifdef  HAVE_STRUCT_SOCKADDR_SA_LEN
        int len = sizeof(struct sockaddr);
        if (ifr->ifr_addr.sa_len > len)
        len = ifr->ifr_addr.sa_len; /* length > 16 */
        ptr += sizeof(ifr->ifr_name) + len; /* for next one in buffer */
#else
        ptr += sizeof(*ifr); /* for next one in buffer */
#endif

        int flags;
        struct sockaddr_in *sinptr;
        MyNIC *tmp=nullptr;
        switch (ifr->ifr_addr.sa_family) {
        case AF_INET:
            sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
            flags=0;

            struct ifreq ifcopy;
            ifcopy=*ifr;
            result=ioctl(sockfd, SIOCGIFFLAGS, &ifcopy);
            flags=ifcopy.ifr_flags;

            tmp=new MyNIC;
            tmp->name=ifr->ifr_name;
            tmp->state= ((flags & IFF_UP) == IFF_UP) ? upMessage : downMessage;

            if ((flags & IFF_BROADCAST) == IFF_BROADCAST)
                tmp->type=i18nc("@item:intext Mode of network card", "Broadcast");
            else if ((flags & IFF_POINTOPOINT) == IFF_POINTOPOINT)
                tmp->type=i18nc("@item:intext Mode of network card", "Point to Point");
#ifndef _AIX
            else if ((flags & IFF_MULTICAST) == IFF_MULTICAST)
                tmp->type=i18nc("@item:intext Mode of network card", "Multicast");
#endif
            else if ((flags & IFF_LOOPBACK) == IFF_LOOPBACK)
                tmp->type=i18nc("@item:intext Mode of network card", "Loopback");
            else
                tmp->type=i18nc("@item:intext Mode of network card", "Unknown");

            tmp->addr=inet_ntoa(sinptr->sin_addr);

            ifcopy=*ifr;
            result=ioctl(sockfd, SIOCGIFNETMASK, &ifcopy);
            if (result==0) {
                sinptr = (struct sockaddr_in *) &ifcopy.ifr_addr;
                tmp->netmask=inet_ntoa(sinptr->sin_addr);
            } else
                tmp->netmask=i18nc("Unknown network mask", "Unknown");

            ifcopy=*ifr;
            result=-1; // if none of the two #ifs below matches, ensure that result!=0 so that "Unknown" is returned as result
#ifdef SIOCGIFHWADDR
            result=ioctl(sockfd, SIOCGIFHWADDR, &ifcopy);
            if (result==0) {
                char *n = &ifcopy.ifr_ifru.ifru_hwaddr.sa_data[0];
                tmp->HWaddr = HWaddr2String(n);
            }
#elif defined SIOCGENADDR
            result=ioctl(sockfd,SIOCGENADDR,&ifcopy);
            if (result==0)
            {
                char *n = &ifcopy.ifr_ifru.ifru_enaddr[0];
                tmp->HWaddr = HWaddr2String(n);
            }
#endif
            if (result!=0) {
                tmp->HWaddr = i18nc("Unknown HWaddr", "Unknown");
            }

            nl.append(tmp);
            break;

        default:
            break;
        }
    }
#else
    struct ifaddrs *ifap, *ifa;
    if (getifaddrs(&ifap) != 0) {
        return nl;
    }

    NetworkModel::MyNIC *tmp = nullptr;
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            qDebug() << "stumbled over an interface without ifa_addr. You may wish to file a bug against kinfocenter"
                     << ifa->ifa_name << ifa->ifa_flags;
            continue;
        }

        switch (ifa->ifa_addr->sa_family) {
            case AF_INET6:
            case AF_INET: {
                tmp = new NetworkModel::MyNIC;
                tmp->name = ifa->ifa_name;

                char buf[128];

                bzero(buf, 128);
                getNameInfo(ifa->ifa_addr, ifa, buf);
                tmp->addr = buf;

                if (ifa->ifa_netmask != nullptr) {
                    bzero(buf, 128);
                    getNameInfo(ifa->ifa_netmask, ifa, buf);
                    tmp->netmask = buf;
                }

                tmp->state= (ifa->ifa_flags & IFF_UP) ? true : false;
                tmp->type = flags_tos(ifa->ifa_flags);

                nl.append(tmp);
                break;
            }
            default:
            break;
        }
    }

    freeifaddrs(ifap);
#endif
    return nl;
}

#if defined(HAVE_GETNAMEINFO) && defined(HAVE_GETIFADDRS)
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
        tmp += i18nc("@item:intable Netork type", "Broadcast");
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
#endif
