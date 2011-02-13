/*
 *  info_solaris.cpp
 *
 *  Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
 */

#include "config-infocenter.h"

#include <QTreeWidgetItemIterator>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mnttab.h>
#include <kstat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <time.h>

bool GetInfo_IRQ(QTreeWidget*) {
	return false;
}

bool GetInfo_DMA(QTreeWidget*) {
	return false;
}

bool GetInfo_PCI(QTreeWidget*) {
	return false;
}

bool GetInfo_IO_Ports(QTreeWidget*) {
	return false;
}

bool GetInfo_Sound(QTreeWidget*) {
	return false;
}

bool GetInfo_SCSI(QTreeWidget*) {
	return false;
}

bool GetInfo_Partitions(QTreeWidget* tree) {

	FILE *mnttab;
	struct mnttab mnt;
	struct statvfs statbuf;
	fsblkcnt_t tmp;
	QString total;
	QString avail;
	time_t mnttime;
	char *timetxt;
	char *ptr;

	if ( (mnttab = fopen(MNTTAB, "r")) == NULL) {
		return false;
	}

	/*
	 *  set up column headers
	 */
	QStringList headers;
	headers << i18n("Device") << i18n("Mount Point") << i18n("FS Type") << i18n("Total Size") << i18n("Free Size") << i18n("Mount Time") << i18n("Mount Options");
	tree->setHeaderLabels(headers);

	// XXX: FIXME: how do I set column alignment correctly?
	//tree->setColumnAlignment( 3, 2 );
	// XXX: FIXME: how do I set column alignment correctly?
	//tree->setColumnAlignment( 4, 2 );

	/*
	 *  get info about mounted file systems
	 */
	rewind(mnttab);
	while (getmntent(mnttab, &mnt) == 0) {
		/*
		 *  skip fstype "nfs" and "autofs" for two reasons:
		 *	o if the mountpoint is visible, the fs is not
		 *	  necessarily available (autofs option "-nobrowse")
		 *	  and we don't want to mount every remote fs just
		 *	  to get its size, do we?
		 *	o the name "Partitions" for this statistics implies
		 *	  "local file systems only"
		 */
		if ( (strcmp(mnt.mnt_fstype, "nfs") == 0) || (strcmp(mnt.mnt_fstype, "autofs") == 0))
			continue;
		if (statvfs(mnt.mnt_mountp, &statbuf) == 0) {
			if (statbuf.f_blocks > 0) {
				/*
				 *  produce output in KB, MB, or GB for
				 *  readability -- unfortunately, this
				 *  breaks sorting for these columns...
				 */
				tmp = statbuf.f_blocks * (statbuf.f_frsize / 1024);
				if (tmp > 9999) {
					tmp /= 1024;
					if (tmp > 9999) {
						tmp /= 1024;
						total.setNum(tmp);
						total += " G";
					} else {
						total.setNum(tmp);
						total += " M";
					}
				} else {
					total.setNum(tmp);
					total += " K";
				}
				//				avail.setNum( statbuf.f_bavail );
				//				avail += " K";
				tmp = statbuf.f_bavail * (statbuf.f_frsize / 1024);
				if (tmp > 9999) {
					tmp /= 1024;
					if (tmp > 9999) {
						tmp /= 1024;
						avail.setNum(tmp);
						avail += " G";
					} else {
						avail.setNum(tmp);
						avail += " M";
					}
				} else {
					avail.setNum(tmp);
					avail += " K";
				}
			} else {
				total = "-";
				avail = "-";
			}
		} else {
			total = "???";
			avail = "???";
		}
		/*
		 *  ctime() adds a '\n' which we have to remove
		 *  so that we get a one-line output for the QTreeWidgetItem
		 */
		mnttime = (time_t) atol(mnt.mnt_time);
		if ( (timetxt = ctime( &mnttime)) != NULL) {
			ptr = strrchr(timetxt, '\n');
			*ptr = '\0';
		}

		QStringList list;
		list << mnt.mnt_special << mnt.mnt_mountp << mnt.mnt_fstype << total << avail << QString(timetxt) << mnt.mnt_mntopts;
		new QTreeWidgetItem(tree, list);
	}
	fclose(mnttab);

	tree->sortItems(0, Qt::AscendingOrder);

	return true;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
