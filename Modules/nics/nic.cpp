/*
 * nic.cpp
 *
 *  Copyright (C) 2001 Alexander Neundorf <neundorf@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "nic.h"

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

#include <kaboutdata.h>
#include <KLocalizedString>

#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>

#ifdef USE_SOLARIS
/* net/if.h is incompatible with STL on Solaris 2.6 - 2.8, redefine
 map in the header file because we don't need it. -- Simon Josefsson */
#define map junkmap
#endif
#  include <net/if.h>
#ifdef USE_SOLARIS
#undef map
#endif

#include <sys/ioctl.h>
#include <KPluginFactory>
#include <KPluginLoader>

#ifndef	HAVE_STRUCT_SOCKADDR_SA_LEN
#undef HAVE_GETNAMEINFO
#undef HAVE_GETIFADDRS
#endif

#if defined(HAVE_GETNAMEINFO) && defined(HAVE_GETIFADDRS)
#include <ifaddrs.h>
#include <netdb.h>

QString flags_tos (unsigned int flags);
#endif

K_PLUGIN_FACTORY(KCMNicFactory,
		registerPlugin<KCMNic>();
)

struct MyNIC {
	QString name;
	QString addr;
	QString netmask;
	QString state;
	QString type;
	QString HWaddr;
};

QList<MyNIC*> findNICs();

KCMNic::KCMNic(QWidget *parent, const QVariantList &) :
	KCModule(parent) {
	QVBoxLayout *box=new QVBoxLayout(this);
	box->setMargin(0);
	m_list=new QTreeWidget(this);
	m_list->setRootIsDecorated(false);
	box->addWidget(m_list);
	QStringList columns;
	columns<<i18n("Name")<<i18n("IP Address")<<i18n("Network Mask")<<i18n("Type")<<i18n("State")<<i18n("HWAddr");
	m_list->setHeaderLabels(columns);
	QHBoxLayout *hbox=new QHBoxLayout();
	box->addItem(hbox);
	m_updateButton=new QPushButton(i18n("&Update"),this);
	hbox->addStretch(1);
	hbox->addWidget(m_updateButton);
	QTimer* timer=new QTimer(this);
	timer->start(60000);
	connect(m_updateButton, &QPushButton::clicked, this, &KCMNic::update);
	connect(timer, &QTimer::timeout, this, &KCMNic::update);
	update();
	KAboutData *about = new KAboutData(i18n("kcminfo"),
			i18n("System Information Control Module"),
			QString(), QString(), KAboutLicense::GPL,
			i18n("(c) 2001 - 2002 Alexander Neundorf"));

	about->addAuthor(i18n("Alexander Neundorf"), QString(), QStringLiteral("neundorf@kde.org"));
	setAboutData(about);

}

void KCMNic::update() {
	m_list->clear();
	QList<MyNIC*> nics=findNICs();

	foreach(MyNIC* tmp, nics) {
		QStringList lst;
		lst << tmp->name<<tmp->addr<<tmp->netmask<<tmp->type<<tmp->state<<tmp->HWaddr;
		new QTreeWidgetItem(m_list,lst);

		delete tmp;
	}

	nics.clear();

}

static QString HWaddr2String(const char *hwaddr) {
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

QList<MyNIC*> findNICs() {
	QString upMessage(i18nc("State of network card is connected", "Up") );
	QString downMessage(i18nc("State of network card is disconnected", "Down") );

	QList<MyNIC*> nl;

#if !defined(HAVE_GETIFADDRS) || !defined(HAVE_GETNAMEINFO)

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	char buf[8*1024];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_req = (struct ifreq *) buf;
	int result=ioctl(sockfd, SIOCGIFCONF, &ifc);

	for (char* ptr = buf; ptr < buf + ifc.ifc_len;) {
		struct ifreq *ifr =(struct ifreq *) ptr;
#ifdef	HAVE_STRUCT_SOCKADDR_SA_LEN
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

	MyNIC *tmp=0;
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		switch (ifa->ifa_addr->sa_family) {
			case AF_INET6:
			case AF_INET: {
				tmp = new MyNIC;
				tmp->name = ifa->ifa_name;

				char buf[128];

				bzero(buf, 128);
				getnameinfo(ifa->ifa_addr, ifa->ifa_addr->sa_len, buf, 127, 0, 0, NI_NUMERICHOST);
				tmp->addr = buf;

                if (ifa->ifa_netmask != nullptr) {
					bzero(buf, 128);
					getnameinfo(ifa->ifa_netmask, ifa->ifa_netmask->sa_len, buf, 127, 0, 0, NI_NUMERICHOST);
					tmp->netmask = buf;
				}

				tmp->state= (ifa->ifa_flags & IFF_UP) ? upMessage : downMessage;
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
QString flags_tos (unsigned int flags)
{
	QString tmp;
	if (flags & IFF_POINTOPOINT) {
		tmp += i18n("Point to Point");
	}

	if (flags & IFF_BROADCAST) {
		if (tmp.length()) {
			tmp += QLatin1String(", ");
		}
		tmp += i18n("Broadcast");
	}

	if (flags & IFF_MULTICAST) {
		if (tmp.length()) {
			tmp += QLatin1String(", ");
		}
		tmp += i18n("Multicast");
	}

	if (flags & IFF_LOOPBACK) {
		if (tmp.length()) {
			tmp += QLatin1String(", ");
		}
		tmp += i18n("Loopback");
	}
	return tmp;
}
#endif

#include "nic.moc"
