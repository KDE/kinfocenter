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
#include <QFontMetrics>
#include <QStringList>
#include <QTextStream>
#include <QTreeWidgetItemIterator>

#include <kdebug.h>
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

bool GetInfo_Sound(QTreeWidget* tree) {
	tree->setSortingEnabled(false);

	if (!GetDmesgInfo(tree, "audio", NULL)) {
		QStringList list;
		list << i18n("No audio devices found.");
		new QTreeWidgetItem(tree, list);
	}

	// append information for each audio devices found

	QTreeWidgetItemIterator it(tree, QTreeWidgetItemIterator::All);
	while ( *it != NULL) {
		QString s, s2;
		int pos;
		char *dev;

		s = (*it)->text(0);
		// The autoconf message is in form 'audio0 at auvia0: ...'
		if (s.indexOf("audio") == 0 && (pos = s.indexOf(" at ")) > 0) {
			s2 = s.mid(pos+4); // skip " at "
			s2.remove(QRegExp("[:\n\t ].*"));
			dev = strdup(s2.toAscii().data());

			GetDmesgInfo(tree, dev, NULL);

			free(dev);
		}

		++it;
	}

	return true;
}

bool GetInfo_Devices(QTreeWidget* tree) {
	(void) GetDmesgInfo(tree, NULL, NULL);
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

bool GetInfo_Partitions(QTreeWidget* tree) {
	int num; // number of mounts
#ifdef HAVE_STATVFS
	struct statvfs *mnt; // mount data pointer
#else
	struct statfs *mnt; // mount data pointer
#endif

	// get mount info
	if (!(num=getmntinfo(&mnt, MNT_WAIT))) {
		kError() << "getmntinfo failed" << endl;
		return false;
	}

	// table headers
	QStringList headers;
	headers << i18n("Device") << i18n("Mount Point") << i18n("FS Type") << i18n("Total Size") << i18n("Free Size") << i18n("Total Nodes") << i18n("Free Nodes") << i18n("Flags");
	tree->setHeaderLabels(headers);

	// mnt points into a static array (no need to free it)
	for (; num--; ++mnt) {
		unsigned long long big[2];
		QString vv[5];

#ifdef HAVE_STATVFS
		big[0] = big[1] = mnt->f_frsize; // coerce the product
#else
		big[0] = big[1] = mnt->f_bsize; // coerce the product
#endif
		big[0] *= mnt->f_blocks;
		big[1] *= mnt->f_bavail; // FIXME: use f_bfree if root?

		// convert to strings
		vv[0] = KIO::convertSize(big[0]);
		vv[1] = QString("%1 (%2%)")
		.arg(KIO::convertSize(big[1]))
		.arg(mnt->f_blocks ? mnt->f_bavail*100/mnt->f_blocks : 0);

		vv[2] = QString("%L1").arg(mnt->f_files);
		vv[3] = QString("%L1 (%2%) ")
		.arg(mnt->f_ffree)
		.arg(mnt->f_files ? mnt->f_ffree*100/mnt->f_files : 0);

		vv[4].clear();
#ifdef HAVE_STATVFS
#define MNTF(x) if (mnt->f_flag & ST_##x) vv[4] += QLatin1String(#x " ");
#else
#define MNTF(x) if (mnt->f_flags & MNT_##x) vv[4] += QLatin1String(#x " ");
#endif
		MNTF(ASYNC)
		MNTF(DEFEXPORTED)
		MNTF(EXKERB)
		MNTF(EXNORESPORT)
		MNTF(EXPORTANON)
		MNTF(EXPORTED)
		MNTF(EXPUBLIC)
		MNTF(EXRDONLY)
#ifndef HAVE_STATVFS
		MNTF(IGNORE)
#endif
		MNTF(LOCAL)
		MNTF(NOATIME)
		MNTF(NOCOREDUMP)
		MNTF(NODEV)
		MNTF(NODEVMTIME)
		MNTF(NOEXEC)
		MNTF(NOSUID)
		MNTF(QUOTA)
		MNTF(RDONLY)
		MNTF(ROOTFS)
		MNTF(SOFTDEP)
		MNTF(SYMPERM)
		MNTF(SYNCHRONOUS)
		MNTF(UNION)
#undef MNTF

		// put it in the table

		QStringList list;
		// FIXME: there're more data but we have limited args (this is wrong! just add!)
		// FIXME: names need pad space
		list << mnt->f_mntfromname << mnt->f_mntonname << mnt->f_fstypename << vv[0] << vv[1] << vv[2] << vv[3] << vv[4];
		new QTreeWidgetItem(tree, list);
	}

	// job well done
	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
