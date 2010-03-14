/*
 * info_fbsd.cpp is part of the KDE program kcminfo.  This displays
 * various information about the system (hopefully a FreeBSD system)
 * it's running on.
 *
 * All of the devinfo bits were blatantly stolen from the devinfo utility
 * provided with FreeBSD 5.0 (and later).  No gross hacks were harmed
 * during the creation of info_fbsd.cpp.  Thanks Mike.
 */

/*
 * all following functions should return true, when the Information
 * was filled into the QTreeWidget. Returning false indicates that
 * information was not available.
 */

#include "config-infocenter.h" // HAVE_DEVINFO_H
#include <sys/types.h>
#include <sys/sysctl.h>

#ifdef HAVE_DEVINFO_H
extern "C" {
#include <devinfo.h>
}
#endif

#include <errno.h>
#include <fstab.h>
#include <string.h>

#include <QMap>
#include <QFile>

#include <QTextStream>

class Device {
public:
	Device(QString n=QString(), QString d=QString()) {
		name=n;
		description=d;
	}
	QString name, description;
};

void ProcessChildren(QString name);
QString GetController(const QString &line);
Device *GetDevice(const QString &line);

#ifdef HAVE_DEVINFO_H
extern "C" {
	int print_irq(struct devinfo_rman *rman, void *arg);
	int print_dma(struct devinfo_rman *rman, void *arg);
	int print_ioports(struct devinfo_rman *rman, void *arg);
	int print_resource(struct devinfo_res *res, void *arg);
}
#endif

bool GetInfo_CPU(QTreeWidget* tree) {
	// Modified 13 July 2000 for SMP by Brad Hughes - bhughes@trolltech.com

	int ncpu;
	size_t len;

	len = sizeof(ncpu);
	sysctlbyname("hw.ncpu", &ncpu, &len, NULL, 0);

	QString cpustring;
	for (int i = ncpu; i > 0; i--) {
		/* Stuff for sysctl */
		char *buf;
		int i_buf;

		// get the processor model
		sysctlbyname("hw.model", NULL, &len, NULL, 0);
		buf = new char[len];
		sysctlbyname("hw.model", buf, &len, NULL, 0);

		// get the TSC speed if we can
		len = sizeof(i_buf);
		if (sysctlbyname("machdep.tsc_freq", &i_buf, &len, NULL, 0) != -1) {
			cpustring = i18n("CPU %1: %2, %3 MHz", i, buf, i_buf/1000000);
		} else {
			cpustring = i18n("CPU %1: %2, unknown speed", i, buf);
		}

		/* Put everything in the listbox */
		QStringList list;
		list << cpustring;
		new QTreeWidgetItem(tree, list);

		/* Clean up after ourselves, this time I mean it ;-) */
		delete[] buf;
	}

	return true;
}

bool GetInfo_IRQ(QTreeWidget* tree) {
#ifdef HAVE_DEVINFO_H
	/* systat lists the interrupts assigned to devices as well as how many were
	 generated.  Parsing its output however is about as fun as a sandpaper
	 enema.  The best idea would probably be to rip out the guts of systat.
	 Too bad it's not very well commented */
	/* Oh neat, current now has a neat little utility called devinfo */
	if (devinfo_init())
	return false;
	devinfo_foreach_rman(print_irq, tree);
	return true;
#else
	return false;
#endif
}

bool GetInfo_DMA(QTreeWidget* tree) {
#ifdef HAVE_DEVINFO_H
	/* Oh neat, current now has a neat little utility called devinfo */
	if (devinfo_init())
	return false;
	devinfo_foreach_rman(print_dma, tree);
	return true;
#else
	return false;
#endif
}

bool GetInfo_IO_Ports(QTreeWidget* tree) {
#ifdef HAVE_DEVINFO_H
	/* Oh neat, current now has a neat little utility called devinfo */
	if (devinfo_init())
	return false;
	devinfo_foreach_rman(print_ioports, tree);
	return true;
#else
	return false;
#endif
}

bool GetInfo_Sound(QTreeWidget* tree) {
	QFile *sndstat = new QFile("/dev/sndstat");
	QTextStream *t;
	QString s;

	if (!sndstat->exists() || !sndstat->open(QIODevice::ReadOnly)) {

		s = i18n("Your sound system could not be queried.  /dev/sndstat does not exist or is not readable.");
		QStringList list;
		list << s;
		new QTreeWidgetItem(tree, list);
	} else {
		t = new QTextStream(sndstat);
		while (!(s=t->readLine()).isNull()) {
			QStringList list;
			list << s;

			new QTreeWidgetItem(tree, list);
		}

		delete t;
		sndstat->close();
	}

	delete sndstat;
	return true;
}

bool GetInfo_SCSI(QTreeWidget* tree) {
	FILE *pipe;
	QFile *camcontrol = new QFile("/sbin/camcontrol");
	QTextStream *t;
	QString s;

	if (!camcontrol->exists()) {
		s = i18n("SCSI subsystem could not be queried: /sbin/camcontrol could not be found");
		QStringList list;
		list << s;
		new QTreeWidgetItem(tree, list);
	} else if ((pipe = popen("/sbin/camcontrol devlist 2>&1", "r")) == NULL) {
		s = i18n("SCSI subsystem could not be queried: /sbin/camcontrol could not be executed");
		QStringList list;
		list << s;
		new QTreeWidgetItem(tree, list);
	} else {

		/* This prints out a list of all the scsi devies, perhaps eventually we could
		 parse it as opposed to schlepping it into a listbox */

		t = new QTextStream(pipe, QIODevice::ReadOnly);

		while (true) {
			s = t->readLine();
			if (s.isEmpty() )
				break;
			QStringList list;
			list << s;
			new QTreeWidgetItem(tree, list);
		}

		delete t;
		pclose(pipe);
	}

	delete camcontrol;

	if (!tree->topLevelItemCount())
		return false;

	return true;
}

bool GetInfo_PCI(QTreeWidget* tree) {
	FILE *pipe;
	QFile *pcicontrol;
	QString s, cmd;
	QTreeWidgetItem *olditem= NULL;

	pcicontrol = new QFile("/usr/sbin/pciconf");

	if (!pcicontrol->exists()) {
		delete pcicontrol;
		pcicontrol = new QFile("/usr/X11R6/bin/scanpci");
		if (!pcicontrol->exists()) {
			delete pcicontrol;
			pcicontrol = new QFile("/usr/X11R6/bin/pcitweak");
			if (!pcicontrol->exists()) {
				QStringList list;
				list << i18n("Could not find any programs with which to query your system's PCI information");
				new QTreeWidgetItem(tree, list);
				delete pcicontrol;
				return true;
			} else {
				cmd = "/usr/X11R6/bin/pcitweak -l 2>&1";
			}
		} else {
			cmd = "/usr/X11R6/bin/scanpci";
		}
	} else {
		cmd = "/usr/sbin/pciconf -l -v 2>&1";
	}
	delete pcicontrol;

	if ((pipe = popen(cmd.toLatin1(), "r")) == NULL) {
		QStringList list;
		list << i18n("PCI subsystem could not be queried: %1 could not be executed", cmd);
		olditem = new QTreeWidgetItem(olditem, list);
	} else {

		/* This prints out a list of all the pci devies, perhaps eventually we could
		 parse it as opposed to schlepping it into a listbox */

		pclose(pipe);
		GetInfo_ReadfromPipe(tree, cmd.toLatin1(), true);
	}

	if (!tree->topLevelItemCount()) {
		QString str = i18n("The PCI subsystem could not be queried, this may need root privileges.");
		olditem = new QTreeWidgetItem(tree, olditem);
        olditem->setText(0, str);
		return true;
	}

	return true;
}

bool GetInfo_Partitions(QTreeWidget* tree) {
	struct fstab *fstab_ent;

	if (setfsent() != 1) /* Try to open fstab */{
		int s_err= errno;
		QString s;
		s = i18n("Could not check file system info: ");
		s += strerror(s_err);
		QStringList list;
		list << s;
		new QTreeWidgetItem(tree, list);
	} else {
		QStringList headers;
		headers << i18n("Device") << i18n("Mount Point") << i18n("FS Type") << i18n("Mount Options");

		while ((fstab_ent=getfsent())!=NULL) {
			QStringList list;
			list << fstab_ent->fs_spec << fstab_ent->fs_file << fstab_ent->fs_vfstype << fstab_ent->fs_mntops;
			new QTreeWidgetItem(tree, list);
		}

		tree->sortItems(0, Qt::AscendingOrder);

		endfsent(); /* Close fstab */
	}
	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}

bool GetInfo_Devices(QTreeWidget* tree) {
	QFile *f = new QFile("/var/run/dmesg.boot");
	if (f->open(QIODevice::ReadOnly)) {
		QTextStream qts(f);
		QMap<QString, QTreeWidgetItem*> lv_items;
		Device *dev;
		QString line, controller;
		tree->setRootIsDecorated(true);
		QStringList headers;
		headers << i18n("Device") << i18n("Description");
		tree->setHeaderLabels(headers);
		while ( !(line=qts.readLine()).isNull() ) {
			controller = GetController(line);
			if (controller.isNull())
				continue;
			dev=GetDevice(line);
			if (!dev)
				continue;
			// Ewww assuing motherboard is the only toplevel controller is rather gross
			if (controller == "motherboard") {
				if (lv_items.contains(QString(dev->name))==false) {
					QStringList list;
					list << dev->name << dev->description;

					lv_items.insert(QString(dev->name), new QTreeWidgetItem(tree, list));
				}
			} else {
				QTreeWidgetItem* parent=lv_items[controller];
				if (parent && lv_items.contains(dev->name)==false) {
					QStringList list;
					list << dev->name << dev->description;
					lv_items.insert(QString(dev->name), new QTreeWidgetItem(parent, list));
				}
			}
		}
                delete f;
		return true;
	}
        delete f;
	return false;
}

QString GetController(const QString &line) {
	if ( ( (line.startsWith("ad")) || (line.startsWith("afd")) || (line.startsWith("acd")) ) && (line.indexOf(":") < 6)) {
		QString controller = line;
		controller.remove(0, controller.indexOf(" at ")+4);
		if (controller.indexOf("-slave") != -1) {
			controller.remove(controller.indexOf("-slave"), controller.length());
		} else if (controller.indexOf("-master") != -1) {
			controller.remove(controller.indexOf("-master"), controller.length());
		} else
			controller=QString();
		if (!controller.isNull())
			return controller;
	}
	if (line.indexOf(" on ") != -1) {
		QString controller;
		controller = line;
		controller.remove(0, controller.indexOf(" on ")+4);
		if (controller.indexOf(" ") != -1)
			controller.remove(controller.indexOf(" "), controller.length());
		return controller;
	}
	return QString();
}

Device *GetDevice(const QString &line) {
	Device *dev;
	int colon = line.indexOf(":");
	if (colon == -1)
		return 0;
	dev = new Device;
	dev->name = line.mid(0, colon);
	dev->description = line.mid(line.indexOf("<")+1, line.length());
	dev->description.remove(dev->description.indexOf(">"), dev->description.length());
	return dev;
}

#ifdef HAVE_DEVINFO_H

int print_irq(struct devinfo_rman *rman, void *arg) {
	QTreeWidget* tree = (QTreeWidget *)arg;
	if (strcmp(rman->dm_desc, "Interrupt request lines")==0) {

		QStringList list;
		list << rman->dm_desc;
		new QTreeWidgetItem(tree, list);
		devinfo_foreach_rman_resource(rman, print_resource, arg);
	}
	return 0;
}

int print_dma(struct devinfo_rman *rman, void *arg)
{
	QTreeWidget* tree = (QTreeWidget *)arg;
	if (strcmp(rman->dm_desc, "DMA request lines")==0) {
		QStringList list;
		list << rman->dm_desc;
		new QTreeWidgetItem(tree, list);
		devinfo_foreach_rman_resource(rman, print_resource, arg);
	}
	return(0);
}

int print_ioports(struct devinfo_rman *rman, void *arg)
{
	QTreeWidget* tree = (QTreeWidget*) arg;

	if (strcmp(rman->dm_desc, "I/O ports")==0) {
		QStringList list;
		list << rman->dm_desc;
		new QTreeWidgetItem(tree, list);

		devinfo_foreach_rman_resource(rman, print_resource, arg);
	}
	else if (strcmp(rman->dm_desc, "I/O memory addresses")==0) {
		QStringList list;
		list << rman->dm_desc;
		new QTreeWidgetItem(tree, list);

		devinfo_foreach_rman_resource(rman, print_resource, arg);
	}
	return 0;
}

int print_resource(struct devinfo_res *res, void *arg)
{
	struct devinfo_dev *dev;
	struct devinfo_rman *rman;
	int hexmode;

	QTreeWidget* tree = (QTreeWidget*) arg;

	QString s, tmp;

	rman = devinfo_handle_to_rman(res->dr_rman);
	hexmode = (rman->dm_size > 100) || (rman->dm_size == 0);
	tmp.sprintf(hexmode ? "0x%lx" : "%lu", res->dr_start);
	s += tmp;
	if (res->dr_size > 1) {
		tmp.sprintf(hexmode ? "-0x%lx" : "-%lu",
				res->dr_start + res->dr_size - 1);
		s += tmp;
	}

	dev = devinfo_handle_to_device(res->dr_device);
	if ((dev != NULL) && (dev->dd_name[0] != 0)) {
		tmp.sprintf(" (%s)", dev->dd_name);
	} else {
		tmp.sprintf(" ----");
	}
	s += tmp;

	QStringList list;
	list << s;
	new QTreeWidgetItem(tree, list);

	return 0;
}

#endif
