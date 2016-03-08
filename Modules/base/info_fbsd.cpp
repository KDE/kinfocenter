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

#include <string.h>

#include <QFileInfo>

#include <QTextStream>

void ProcessChildren(QString name);

#ifdef HAVE_DEVINFO_H
extern "C" {
	int print_irq(struct devinfo_rman *rman, void *arg);
	int print_dma(struct devinfo_rman *rman, void *arg);
	int print_ioports(struct devinfo_rman *rman, void *arg);
	int print_resource(struct devinfo_res *res, void *arg);
}
#endif

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

bool GetInfo_PCI(QTreeWidget* tree) {
	FILE *pipe;
	QString s, cmd;
	QTreeWidgetItem *olditem= NULL;

	const QStringList headers(i18nc("@title:column Column name for PCI information", "Information"));
	tree->setHeaderLabels(headers);

	if (!QFileInfo(QLatin1String("/usr/sbin/pciconf")).exists()) {
		QStringList list;
		list << i18n("Could not find any programs with which to query your system's PCI information");
		new QTreeWidgetItem(tree, list);
		return true;
	} else {
		cmd = "/usr/sbin/pciconf -l -v 2>&1";
	}

	// TODO: GetInfo_ReadfromPipe should be improved so that we could pass the program name and its
	//       arguments to it and remove most of the code below.
	if ((pipe = popen(cmd.toLatin1(), "r")) == NULL) {
		QStringList list;
		list << i18n("PCI subsystem could not be queried: %1 could not be executed", cmd);
		olditem = new QTreeWidgetItem(olditem, list);
	} else {
		/* This prints out a list of all the pci devies, perhaps eventually we could
		 parse it as opposed to schlepping it into a listbox */
		QTextStream outputStream(pipe, QIODevice::ReadOnly);

		while (!outputStream.atEnd()) {
			s = outputStream.readLine();
			if (s.isEmpty() )
				break;
			const QStringList list(s);
			new QTreeWidgetItem(tree, list);
		}

		pclose(pipe);
	}

	if (!tree->topLevelItemCount()) {
		QString str = i18n("The PCI subsystem could not be queried, this may need root privileges.");
		olditem = new QTreeWidgetItem(tree, olditem);
		olditem->setText(0, str);
		return true;
	}

	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
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
