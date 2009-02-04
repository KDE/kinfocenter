/*
 * info_netbsd.cpp is part of the KDE program kcminfo.  This displays
 * various information about the OpenBSD system it's running on.
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
 * was filled into the QTreeWidget. Returning false indicates that
 * information was not available.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <stdio.h>	/* for NULL */
#include <stdlib.h>	/* for malloc(3) */

#include <QFile>
#include <QFontMetrics>
#include <QTextStream>
#include <QTreeWidgetItemIterator>

#include <kdebug.h>

typedef struct {
	int string;
	int name;
	const char *title;
} hw_info_mib_list_t;

bool GetInfo_CPU(QTreeWidget* tree) {
	static const hw_info_mib_list_t hw_info_mib_list[]= { { 1, HW_MODEL, "Model" }, { 1, HW_MACHINE, "Machine" }, { 0, HW_NCPU, "Number of CPUs" }, { 0, HW_PAGESIZE, "Pagesize" }, { 0, 0, 0 } };
	hw_info_mib_list_t *hw_info_mib;

	int mib[2], num;
	char *buf;
	size_t len;
	QString value;

	QStringList headers;
	headers << i18n("Information") << i18n("Value");
	tree->setHeaderLabels(headers);

	for (hw_info_mib = hw_info_mib_list; hw_info_mib->title; ++hw_info_mib) {
		mib[0] = CTL_HW;
		mib[1] = hw_info_mib->name;
		if (hw_info_mib->string) {
			sysctl(mib, 2, NULL, &len, NULL, 0);
			if ( (buf = (char*)malloc(len))) {
				sysctl(mib, 2, buf, &len, NULL, 0);
				value = QString::fromLocal8Bit(buf);
				free(buf);
			} else {
				value = QString("Unknown");
			}
		} else {
			len = sizeof(num);
			sysctl(mib, 2, &num, &len, NULL, 0);
			value.sprintf("%d", num);
		}
		
		QStringList list;
		list << hw_info_mib->title << value;
		new QTreeWidgetItem(tree, list);
	}

	return true;
}

// this is used to find out which devices are currently
// on system
static bool GetDmesgInfo(QTreeWidget* tree, const char *filter, void func(QTreeWidget *, QString s, void **, bool)) {
	QFile *dmesg = new QFile("/var/run/dmesg.boot");
	bool usepipe=false;
	FILE *pipe=NULL;
	QTextStream *t;
	bool seencpu=false;
	void *opaque=NULL;
	QString s;
	bool found=false;

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

	while (!(s = t->readLine()).isNull()) {
		if (!seencpu) {
			if (s.contains("cpu"))
				seencpu = true;
			else
				continue;
		}
		if (s.contains("boot device") || s.contains("WARNING: old BSD partition ID!"))
			break;

		if (!filter || s.contains(filter)) {
			if (func) {
				func(tree, s, &opaque, false);
			} else {
				QStringList list;
				list << s;
				new QTreeWidgetItem(tree, list);
			}
			found = true;
		}
	}
	if (func) {
		func(tree, s, &opaque, true);
	}
	//tree->triggerUpdate();

	delete t;
	if (pipe) {
		pclose(pipe);
	} else {
		dmesg->close();
		delete dmesg;
	}

	return found;
}

void AddIRQLine(QTreeWidget* tree, QString s, void **opaque, bool ending) {
	QStringList *strlist = (QStringList *) *opaque;
	QString str;
	int pos, irqnum=0;
	const char *p;
	p = s.toLatin1();

	if (!strlist) {
		strlist = new QStringList();
		*opaque = (void *) strlist;
	}
	if (ending) {
		foreach(QString temp, *strlist) {
			QStringList tempList;
			tempList << temp;
			new QTreeWidgetItem(tree, tempList);
		}
		delete strlist;
		return;
	}

	pos = s.find(" irq ");
	irqnum = (pos < 0) ? 0 : atoi(&p[pos+5]);
	if (irqnum) {
		s.sprintf("%02d%s", irqnum, p);
	} else {
		s.sprintf("??%s", p);
	}
	strlist->append(s);
	strlist->sort();
}

bool GetInfo_IRQ(QTreeWidget* tree) {
	QStringList headers;
	headers << i18n("IRQ") << i18n("Device");
	tree->setHeaderLabels(headers);
	(void) GetDmesgInfo(tree, " irq ", AddIRQLine);
	return true;
}

bool GetInfo_DMA(QTreeWidget *) {
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
	if (!GetDmesgInfo(tree, "audio", NULL)) {
		QStringList list;
		list << i18n("No audio devices found.");
		new QTreeWidgetItem(tree, list);
	}

	// append information on any audio devices found
	QTreeWidgetItemIterator it(tree, QTreeWidgetItemIterator::All);
	while ( *it != NULL ) {
		QTreeWidgetItem* lvitem = *it;

		QString s;
		int pos, len;
		const char *start, *end;
		char *dev;

		s = lvitem->text(0);
		if ((pos = s.find("at ")) >= 0) {
			pos += 3; // skip "at "
			start = end = s.toAscii();
			for (; *end && (*end!=':') && (*end!='\n'); end++)
				;
			len = end - start;
			dev = (char *) malloc(len + 1);
			strncpy(dev, start, len);
			dev[len] = '\0';

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
	return true;
}

bool GetInfo_Partitions(QTreeWidget* tree) {
	QString s;
	char *line, *orig_line;
	const char *device, *mountpoint, *type, *flags;
	FILE *pipe = popen("/sbin/mount", "r");
	QTextStream *t;

	if (!pipe) {
		kError(0) << i18n("Unable to run /sbin/mount.") << endl;
		return false;
	}
	t = new QTextStream(pipe, QIODevice::ReadOnly);

	QStringList headers;
	headers << i18n("Device") << i18n("Mount Point") << i18n("FS Type") << i18n("Mount Options");
	tree->setHeaderLabels(headers);

	while (!(s = t->readLine()).isNull()) {
		orig_line = line = strdup(s.toLatin1());

		device = strsep(&line, " ");

		(void) strsep(&line, " "); // consume word "on"
		mountpoint = strsep(&line, " ");

		(void) strsep(&line, " "); // consume word "type"
		type = strsep(&line, " ");

		flags = line;

		QStringList mountList;
		mountList << device << mountpoint << type << flags;
		new QTreeWidgetItem(tree, mountList);

		free(orig_line);
	}

	delete t;
	pclose(pipe);
	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
