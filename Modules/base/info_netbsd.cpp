/*
 * info_netbsd.cpp is part of the KDE program kcminfo.  This displays
 * various information about the NetBSD system it's running on.
 *
 * Originally written by Jaromir Dolecek <dolecek@ics.muni.cz>. CPU info
 * code has been imported from implementation of processor.cpp for KDE 1.0
 * by David Brownlee <abs@NetBSD.org> as found in NetBSD packages collection.
 * Hubert Feyer <hubertf@NetBSD.org> enhanced the sound information printing
 * quite a lot, too.
 *
 * The code is placed into public domain. Do whatever you want with it.
 */

/*
 * all following functions should return true, when the Information
 * was filled into the Tree Widget. Returning false indicates that
 * information was not available.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/mount.h>
#include <stdio.h>	/* for NULL */
#include <stdlib.h>	/* for malloc(3) */
#include <fstab.h>

#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QTreeWidgetItemIterator>

#include <QDebug>
#include <kio/global.h> /* for KIO::convertSize() */

typedef struct {
	int string;
	int name;
	const char *title;
} hw_info_mib_list_t;

// this is used to find out which devices are currently
// on system
static bool GetDmesgInfo(QTreeWidget* tree, const char *filter, void func(QTreeWidget* tree, QString s)) {
	QFile *dmesg = new QFile("/var/run/dmesg.boot");
	bool usepipe = false;
	FILE *pipe= NULL;
	QTextStream *t;
	bool seencpu = false;
	QString s;
	bool found = false;

	if (dmesg->exists() && dmesg->open(QIODevice::ReadOnly)) {
		t = new QTextStream(dmesg);
	} else {
		delete dmesg;
		pipe = popen("/sbin/dmesg", "r");
		if (!pipe)
			return false;
		usepipe = true;
		t = new QTextStream(pipe, QIODevice::ReadOnly);
	}

	while (!(s = t->readLine().toLocal8Bit()).isNull()) {
		if (!seencpu) {
			if (s.contains("cpu"))
				seencpu = true;
			else
				continue;
		}
		if (s.contains("boot device") || s.contains("WARNING: old BSD partition ID!"))
			break;

		if (!filter || s.contains(QRegExp(filter))) {
			if (func)
				func(tree, s);
			else {
				QStringList list;
				list << s;
				new QTreeWidgetItem(tree, list);
			}
			found = true;
		}
	}

	delete t;
	if (pipe)
		pclose(pipe);
	else {
		dmesg->close();
		delete dmesg;
	}

	return found;
}

void AddIRQLine(QTreeWidget* tree, QString s) {
	int irqnum;
	QString s2;
	char numstr[3];
	bool ok;

	s2 = s.mid(s.indexOf(QRegExp("[ (]irq "))+5);
	irqnum = s2.remove(QRegExp("[^0-9].*")).toInt(&ok);
	if (ok)
		snprintf(numstr, 3, "%02d", irqnum);
	else {
		// this should never happen
		strcpy(numstr, "??");
	}

	QStringList list;
	list << numstr << s;
	new QTreeWidgetItem(tree, list);
}

bool GetInfo_IRQ(QTreeWidget* tree) {

	QStringList headers;
	headers << i18n("IRQ") << i18n("Device");
	tree->setHeaderLabels(headers);

	tree->sortItems(0, Qt::AscendingOrder);

	tree->setSortingEnabled(false);

	(void) GetDmesgInfo(tree, "[ (]irq ", AddIRQLine);
	return true;
}

bool GetInfo_DMA(QTreeWidget*) {
	return false;
}

bool GetInfo_PCI(QTreeWidget* tree) {
	if (!GetDmesgInfo(tree, "at pci", NULL)) {
		QStringList list;
		list << i18n("No PCI devices found.");
		new QTreeWidgetItem(tree, list);
	}
	return true;
}

bool GetInfo_IO_Ports(QTreeWidget* tree) {
	if (!GetDmesgInfo(tree, "port 0x", NULL)) {
		QStringList list;
		list << i18n("No I/O port devices found.");
		new QTreeWidgetItem(tree, list);
	}
	return true;
}

bool GetInfo_SCSI(QTreeWidget* tree) {
	if (!GetDmesgInfo(tree, "scsibus", NULL)) {
		QStringList list;
		list << i18n("No SCSI devices found.");
		new QTreeWidgetItem(tree, list);
	}

	// remove the 'waiting %d seconds for devices to settle' message
	QTreeWidgetItemIterator it(tree, QTreeWidgetItemIterator::All);
	while ( *it != NULL) {
		QString s = (*it)->text(0);

		if (s.contains("seconds for devices to settle")) {
			delete tree->takeTopLevelItem( tree->indexOfTopLevelItem(*it) );
			break;
		}

		++it;
	}

	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
