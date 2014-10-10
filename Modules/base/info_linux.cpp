
/*
Copyright 1998  Helge Deller deller@gmx.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <sys/stat.h>
#include <linux/kernel.h>
#include <ctype.h>
#include "config-infocenter.h"

#ifdef HAVE_WAYLAND
#include "info_wayland.h"
#endif

#ifdef HAVE_PCIUTILS
#include "kpci.h"
#endif //HAVE_PCIUTILS
#include <QRegExp>
#include <QFile>

#include <KLocalizedString>
#include <kiconloader.h>
#include <QFontDatabase>

#define INFO_IRQ "/proc/interrupts"
#define INFO_DMA "/proc/dma"

#define INFO_PCI "/proc/pci"

#define INFO_IOPORTS "/proc/ioports"

#define INFO_MISC "/proc/misc"

#define INFO_SCSI "/proc/scsi/scsi"

#define MAXCOLUMNWIDTH 600

bool GetInfo_ReadfromFile(QTreeWidget* tree, const char *FileName, const QChar& splitChar) {
	bool added = false;
	QFile file(FileName);

	if (!file.exists()) {
		return false;
	}

	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}
	QTextStream stream(&file);

	QString line = stream.readLine();

	while (!line.isNull()) {
		QString s1, s2;
		if (!line.isEmpty()) {
			if (!splitChar.isNull()) {
				int pos = line.indexOf(splitChar);
				s1 = line.left(pos-1).trimmed();
				s2 = line.mid(pos+1).trimmed();
			} else
				s1 = line;
		}
		QStringList list;
		list << s1 << s2;
		new QTreeWidgetItem(tree, list);
		added = true;
		line = stream.readLine();
	}

	file.close();

	return added;
}

bool GetInfo_IRQ(QTreeWidget* tree) {
	tree->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
	tree->setHeaderHidden(true);
	
	return GetInfo_ReadfromFile(tree, INFO_IRQ, 0);
}

bool GetInfo_DMA(QTreeWidget* tree) {
	QFile file(INFO_DMA);

	QStringList headers;
	headers << i18n("DMA-Channel") << i18n("Used By");
	tree->setHeaderLabels(headers);

	if (file.exists() && file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);
		QString line;

		line = stream.readLine();
		while (!line.isNull()) {
			if (!line.isEmpty()) {
				QRegExp rx("^\\s*(\\S+)\\s*:\\s*(\\S+)");
				if (-1 != rx.indexIn(line)) {
					QStringList list;
					list << rx.cap(1) << rx.cap(2);
					new QTreeWidgetItem(tree, list);
				}
			}
			line = stream.readLine();
		}
		file.close();
	} else {
		return false;
	}

	return true;
}

bool GetInfo_PCI(QTreeWidget* tree) {
	int num;

#ifdef HAVE_PCIUTILS
	if ( (num = GetInfo_PCIUtils(tree))) {
		return num;
	}
	
#endif //HAVE_PCIUTILS
	tree->setHeaderHidden(true);
	tree->setSortingEnabled(false);

	/* try to get the output of the lspci package first */
	if ((num = GetInfo_ReadfromPipe(tree, "lspci -v", true)) || (num = GetInfo_ReadfromPipe(tree, "/sbin/lspci -v", true)) || (num = GetInfo_ReadfromPipe(tree, "/usr/sbin/lspci -v", true)) || (num = GetInfo_ReadfromPipe(tree, "/usr/local/sbin/lspci -v", true)) || (num = GetInfo_ReadfromPipe(tree,
			"/usr/bin/lspci -v", true)))
		return num;

	/* if lspci failed, read the contents of /proc/pci */
	return GetInfo_ReadfromFile(tree, INFO_PCI, 0);

}

bool GetInfo_IO_Ports(QTreeWidget* tree) {
	QStringList headers;
	headers << i18n("I/O-Range") << i18n("Used By");
	tree->setHeaderLabels(headers);
	return GetInfo_ReadfromFile(tree, INFO_IOPORTS, ':');
}

bool GetInfo_SCSI(QTreeWidget* tree) {
	return GetInfo_ReadfromFile(tree, INFO_SCSI, 0);
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}

bool GetInfo_Wayland(QTreeWidget* tree) {
#if HAVE_WAYLAND
    WaylandModule *display = new WaylandModule(tree);

    return true;
#endif
    return false;
}
