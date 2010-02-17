/*

 Linux-specific Information about the Hardware.

 (C) Copyright 1998-2001 by Helge Deller <deller@gmx.de>

 To do (maybe?):
 - include Information about XFree86 and/or Accelerated X
 (needs to change configure-script, to see, if Header-files are available !)
 - maybe also include information about the video-framebuffer devices
 - rewrite detection-routines (maybe not to use the /proc-fs)
 - more & better sound-information

 /dev/sndstat support added: 1998-12-08 Duncan Haldane (f.d.m.haldane@cwix.com)

 */

#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <sys/stat.h>
#include <linux/kernel.h>
#include <ctype.h>
#include "config-infocenter.h"

#ifdef HAVE_FSTAB_H	/* some Linux-versions don't have fstab.h */
#  include <fstab.h>
#  include <sys/statfs.h>
#  define INFO_PARTITIONS_FULL_INFO	/* show complete info */
#elif defined HAVE_MNTENT_H	/* but maybe they have mntent.h ? */
# include <mntent.h>
# include <sys/vfs.h>
#  define INFO_PARTITIONS_FULL_INFO	/* show complete info */
#else
#  undef INFO_PARTITIONS_FULL_INFO	/* no partitions-info */
#endif

#ifdef HAVE_PCIUTILS
#include "kpci.h"
#endif //HAVE_PCIUTILS
#include <QRegExp>
#include <QFile>
#include <QHeaderView>

#include <klocale.h>
#include <kiconloader.h>
#include <kglobalsettings.h>
#include <kdebug.h>

#define INFO_CPU "/proc/cpuinfo"

#define INFO_IRQ "/proc/interrupts"
#define INFO_DMA "/proc/dma"

#define INFO_PCI "/proc/pci"

#define INFO_IOPORTS "/proc/ioports"

#define INFO_DEV_SNDSTAT "/dev/sndstat"
#define INFO_SOUND "/proc/sound"
#define INFO_ASOUND "/proc/asound/oss/sndstat"
#define INFO_ASOUND09 "/proc/asound/sndstat"

#define INFO_DEVICES "/proc/devices"
#define INFO_MISC "/proc/misc"

#define INFO_SCSI "/proc/scsi/scsi"

#define INFO_PARTITIONS "/proc/partitions"
#define INFO_MOUNTED_PARTITIONS "/etc/mtab"	/* on Linux... */

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

bool GetInfo_CPU(QTreeWidget* tree) {
	QStringList headers;
	headers << i18n("Information") << i18n("Value");
	tree->setHeaderLabels(headers);

	return GetInfo_ReadfromFile(tree, INFO_CPU, ':');
}

bool GetInfo_IRQ(QTreeWidget* tree) {
	tree->setFont(KGlobalSettings::fixedFont());
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

bool GetInfo_Sound(QTreeWidget* tree) {
	tree->setSortingEnabled(false);

	if (GetInfo_ReadfromFile(tree, INFO_DEV_SNDSTAT, 0))
		return true;
	else if (GetInfo_ReadfromFile(tree, INFO_SOUND, 0))
		return true;
	else if (GetInfo_ReadfromFile(tree, INFO_ASOUND, 0))
		return true;
	else
		return GetInfo_ReadfromFile(tree, INFO_ASOUND09, 0);
}

bool GetInfo_Devices(QTreeWidget* tree) {
	kDebug() << "Linux Info Devices" << endl;
	QFile file;
	QTreeWidgetItem *misc=NULL;

	tree->setRootIsDecorated(true);
	QStringList headers;
	headers << i18n("Devices") << i18n("Major Number") << i18n("Minor Number");
	tree->setHeaderLabels(headers);

	file.setFileName(INFO_DEVICES);
	if (file.exists() && file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);
		QTreeWidgetItem *parent=0L, *child=0L;

		QString line = stream.readLine();

		while (!line.isNull()) {
			if (!line.isEmpty()) {
				if (-1 != line.indexOf("character device", 0, Qt::CaseInsensitive)) {
					QStringList list;
					list << i18n("Character Devices");
					parent = new QTreeWidgetItem(tree, list);
					parent->setIcon(0, SmallIcon("chardevice"));
					parent->setExpanded(true);

				} else if (-1 != line.indexOf("block device", 0, Qt::CaseInsensitive)) {
					QStringList list;
					list << i18n("Block Devices");
					parent = new QTreeWidgetItem(tree, list);
					parent->setIcon(0, SmallIcon("blockdevice"));
					parent->setExpanded(true);

				} else {
					QRegExp rx("^\\s*(\\S+)\\s+(\\S+)");
					if (-1 != rx.indexIn(line)) {
						if (parent) {
							QStringList list;
							list << rx.cap(2) << rx.cap(1);
							child = new QTreeWidgetItem(parent, list);
						} else {
							QStringList list;
							list << rx.cap(2) << rx.cap(1);
							child = new QTreeWidgetItem(tree, list);
						}

						if (rx.cap(2)=="misc") {
							misc=child;
						}
					}
				}
			}
			line = stream.readLine();
		}
		file.close();
	} else {
		return false;
	}

	file.setFileName(INFO_MISC);
	if (misc && file.exists() && file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);

		misc->setText(0, i18n("Miscellaneous Devices"));
		misc->setIcon(0, SmallIcon("memory"));
		misc->setExpanded(true);

		QString line = stream.readLine();

		while (!line.isNull()) {
			if (!line.isEmpty()) {
				QRegExp rx("^\\s*(\\S+)\\s+(\\S+)");
				if (-1 != rx.indexIn(line)) {
					QStringList list;
					list << rx.cap(2) << "10" << rx.cap(1);
					new QTreeWidgetItem(misc, list);
				}
			}
			line = stream.readLine();
		}
		file.close();
	}

	return true;
}

bool GetInfo_SCSI(QTreeWidget* tree) {
	return GetInfo_ReadfromFile(tree, INFO_SCSI, 0);
}

static void cleanPassword(QString & str) {
	int index = 0;
	QString passwd("password=");

	while (index >= 0) {
		index = str.indexOf(passwd, index, Qt::CaseInsensitive);
		if (index >= 0) {
			index += passwd.length();
			while (index < (int) str.length() && str[index] != ' ' && str[index] != ',')
				str[index++] = '*';
		}
	}
}

#ifndef INFO_PARTITIONS_FULL_INFO

bool GetInfo_Partitions(QTreeWidget* tree)
{
	return GetInfo_ReadfromFile(tree, INFO_PARTITIONS, 0);
}

#else	/* INFO_PARTITIONS_FULL_INFO */

// Some Ideas taken from garbazo from his source in info_fbsd.cpp


#if ( defined(HAVE_LINUX_RAW_H) || defined(HAVE_SYS_RAW_H) ) && defined(HAVE_SYS_IOCTL_H) && defined(__GNUC__) && !defined(__STRICT_ANSI__)
#include <sys/ioctl.h>
#include <fcntl.h>

#if defined(HAVE_SYS_RAW_H)
#include <sys/raw.h>
#elif defined(HAVE_LINUX_RAW_H)
#include <linux/raw.h>
#endif

/*
 * get raw device bindings and information
 */
void Get_LinuxRawDevices(QTreeWidget* tree)
{
	int f, i, err;
	int new_raw_devs = 1;
	struct raw_config_request rq;
	QString devname;
	QString MB(i18nc("Mebibyte", "MiB"));

	/* try to open the raw device control file */
	f = open("/dev/rawctl", O_RDWR);
	if (f == -1) {
		f = open("/dev/raw", O_RDWR);
		new_raw_devs = 0;
	}
	if (f == -1)
	return;

	for (i=1; i<256; i++) {
		rq.raw_minor = i;
		if (ioctl(f, RAW_GETBIND, &rq))
		continue;
		if (!rq.block_major) /* unbound ? */
		continue;
		unsigned int minor = rq.block_minor;
		char first_letter;
		switch ((int)rq.block_major) {

			/* IDE drives */
			case 3: first_letter = 'a';
			set_ide_name:
			devname = QString("/dev/hd%1%2")
			.arg(QChar(first_letter + minor/64))
			.arg(minor&63);
			break;
			case 22:first_letter = 'c'; goto set_ide_name;
			case 33:first_letter = 'e'; goto set_ide_name;
			case 34:first_letter = 'g'; goto set_ide_name;
			case 56:first_letter = 'i'; goto set_ide_name;
			case 57:first_letter = 'k'; goto set_ide_name;
			case 88:first_letter = 'm'; goto set_ide_name;
			case 89:first_letter = 'o'; goto set_ide_name;
			case 90:first_letter = 'q'; goto set_ide_name;
			case 91:first_letter = 's'; goto set_ide_name;

			/* SCSI drives */
			case 8: first_letter = 'a';
			set_scsi_name:
			devname = QString("/dev/sd%1%2")
			.arg(QChar(first_letter + minor/16))
			.arg(minor&15);
			break;
			case 65:first_letter = 'q'; goto set_scsi_name;

			/* Compaq /dev/cciss devices */
			case 104: case 105: case 106:
			case 107: case 108: case 109:
			devname = QString("/dev/cciss/c%1d%2")
			.arg((int)rq.block_major-104)
			.arg(minor&15);
			break;

			/* Compaq Intelligent Drive Array (ida) */
			case 72: case 73: case 74: case 75:
			case 76: case 77: case 78: case 79:
			devname = QString("/dev/ida/c%1d%2")
			.arg((int)rq.block_major-72)
			.arg(minor&15);
			break;

			default: devname = QString("%1/%2")
			.arg((int)rq.block_major)
			.arg(minor);

		}

		/* TODO: get device size */
		QString size = "";

		QStringList list;
		list << devname;
		if (new_raw_devs)
		list << QString("/dev/raw/raw%1").arg(i);
		else
		list << QString("/dev/raw%1").arg(i);

		list << "raw" << size << " " << "";
		new QTreeWidgetItem(tree, list);
	}
	close(f);
}
#else
#define Get_LinuxRawDevices(x)	/* nothing */
#endif

bool GetInfo_Partitions(QTreeWidget* tree) {
	QStringList Mounted_Partitions;
	bool found_in_List;
	int n;

#ifdef HAVE_FSTAB_H
	struct fstab *fstab_ent;
# define FS_NAME	fstab_ent->fs_spec	// device-name
# define FS_FILE	fstab_ent->fs_file	// mount-point
# define FS_TYPE	fstab_ent->fs_vfstype	// fs-type
# define FS_MNTOPS 	fstab_ent->fs_mntops	// mount-options
#else
	struct mntent *mnt_ent;
	FILE *fp;
# define FS_NAME	mnt_ent->mnt_fsname	// device-name
# define FS_FILE	mnt_ent->mnt_dir	// mount-point
# define FS_TYPE	mnt_ent->mnt_type	// fs-type
# define FS_MNTOPS 	mnt_ent->mnt_opts	// mount-options
#endif

	struct statfs sfs;
	quint64 total, avail;
	QString str, mountopts;
	QString MB(i18nc("Mebibyte", "MiB"));

#ifdef HAVE_FSTAB_H
	if (setfsent() == 0) /* Try to open fstab */
		return false;
#else
	if (!(fp = setmntent("/etc/fstab", "r")))
	return false;
#endif

	/* read the list of already mounted file-systems.. */
	QFile *file = new QFile(INFO_MOUNTED_PARTITIONS);
	if (file->open(QIODevice::ReadOnly)) {
		char buf[1024];
		while (file->readLine(buf, sizeof(buf )) > 0) {
			str = QString::fromLocal8Bit(buf);
			if (str.length()) {
				int p = str.indexOf(' '); /* find first space. */
				if (p)
					str.remove(p, 1024); /* erase all chars including space. */
				Mounted_Partitions.append(str);
			}
		}
		file->close();
	}
	delete file;

	/* create the header-tables */
	MB = QString(" ") + MB;

	QStringList headers;
	headers << i18n("Device") << i18n("Mount Point") << i18n("FS Type") << i18n("Total Size") << i18n("Free Size") << i18n("Mount Options");
	tree->setHeaderLabels(headers);

	for (n = 0; n < 6; ++n)

		/* loop through all partitions... */
#ifdef HAVE_FSTAB_H
		while ((fstab_ent = getfsent()) != NULL)
#else
		while ((mnt_ent = getmntent(fp)) != NULL)
#endif
		{
			total = avail = 0; /* initialize size.. */
			found_in_List = (Mounted_Partitions.contains(FS_NAME) > 0);
			if (found_in_List && statfs(FS_FILE, &sfs) == 0) {
				total = ((quint64) sfs.f_blocks) * sfs.f_bsize;
				avail = (getuid() ? sfs.f_bavail : sfs.f_bfree) * ((quint64) sfs.f_bsize);
			};
			/*
			 if (stat(fstab_ent->fs_file,&st)!=0)
			 total = 0;
			 if (!S_ISDIR(st.st_mode))
			 total = 0;
			 */
			mountopts = FS_MNTOPS;
			cleanPassword(mountopts);
			if (total) {
				QStringList list;
				list << QString(FS_NAME) + "  " << QString(FS_FILE) + "  " << QString(FS_TYPE) + "  " << Value((int) (((total / 1024) + 512) / 1024), 6) + MB << Value((int) (((avail / 1024) + 512) / 1024), 6) + MB << mountopts;

				new QTreeWidgetItem(tree, list);
			} else {
				QStringList list;
				list << QString(FS_NAME) + "  " << QString(FS_FILE) + "  " << QString(FS_TYPE) + "  " << " " << " " << mountopts;

				new QTreeWidgetItem(tree, list);

			}
		}

#ifdef HAVE_FSTAB_H
	endfsent(); /* close fstab.. */
#else
	endmntent(fp); /* close fstab.. */
#endif

	/* get raw device entires if available... */
	Get_LinuxRawDevices(tree);

	tree->sortItems(1, Qt::AscendingOrder);

	return true;
}
#endif				/* INFO_PARTITIONS_FULL_INFO */

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
