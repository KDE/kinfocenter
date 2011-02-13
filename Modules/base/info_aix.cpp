/*
Copyright 2010 Reza Arbab <arbab@austin.ibm.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cf.h>
#define class _class
#include <odmi.h>
#include <sys/cfgodm.h>
#include <sys/cfgdb.h>
#undef class
#include <nl_types.h>
#include <string.h>
#include <kdebug.h>
#include <stdlib.h>

#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstab.h>

#include <errno.h>
#include <sys/utsname.h>

char * device_description(struct CuDv *cudv) {
	char *desc= NULL;
	struct PdDv *pddv = cudv->PdDvLn; /* link to Predefined Devices database */
	nl_catd cat;

	cat = catopen("/usr/lib/methods/devices.cat", NL_CAT_LOCALE);
	if ((int)cat == -1)
		return NULL;

	desc = strdup(catgets(cat, pddv->setno, pddv->msgno, "N/A") );
	catclose(cat);

	return desc;
}

bool list_devices(QListView *lBox, char *criteria) {
	struct CuDv *cudv; /* Customized Devices */
	struct listinfo info;
	int i;
	char *cudv_desc;
	QString cudv_status;
	QListViewItem *lastitem= NULL;

	lBox->addColumn(i18n("Name"));
	lBox->addColumn(i18n("Status"));
	lBox->addColumn(i18n("Location"));
	lBox->addColumn(i18n("Description"));

	if (odm_initialize() == -1) {
		kError(0) << "odm_initialize() failed: odmerrno = " << odmerrno << endl;
		return (false);
	}

	cudv = (struct CuDv *) odm_get_list(CuDv_CLASS, criteria, &info, 100, 2);

	if ((int)cudv == -1) {
		odm_terminate();
		kError(0) << "odm_get_list() failed: odmerrno = " << odmerrno << endl;
		return (false);
	} else if (!cudv) /* empty list */
	{
		odm_terminate();
		return (true);
	}

	for (i=0; i<info.num; i++) {
		switch (cudv[i].status) {
		case DEFINED:
			cudv_status = QString("Defined");
			break;
		case AVAILABLE:
			cudv_status = QString("Available");
			break;
		case STOPPED:
			cudv_status = QString("Stopped");
			break;
		default:
			cudv_status = QString("Unknown");
		}
		cudv_desc = device_description(&cudv[i]);

		lastitem = new QListViewItem(lBox, lastitem,
				QString(cudv[i].name),
				cudv_status,
				QString(cudv[i].location),
				QString(cudv_desc ? cudv_desc : "N/A") );

		if (cudv_desc)
			free(cudv_desc);
	}

	if (odm_free_list(cudv, &info) == -1) {
		odm_terminate();
		kError(0) << "odm_free_list() failed: odmerrno = " << odmerrno << endl;
		return (false);
	}

	odm_terminate();
	return true;
}

enum CHIP
{
	P2SC, Power, Power2, Power3, Power3_II, PowerPC,
	PowerPC_604e, PowerPersonal, RD64_II, RS_III, RS64, RS64_II,
	RS64_III, RS64_IV, RSC, ThinkPad, unknown,
	NUM_CHIPS
};
static const char chip_name[NUM_CHIPS][16] = { "P2SC", "Power", "Power2", "Power3", "Power3-II", "PowerPC", "PowerPC 604e", "PowerPersonal", "RD64-II", "RS-III", "RS64", "RS64-II", "RS64-III", "RS64-IV", "RSC", "ThinkPad", "unknown" };

struct model {
	char model_ID[21];
	char machine_type[21];
	char processor_speed[21];
	unsigned short architecture; /* enum CHIP */
};

const
struct model _models[] = { { "02", "7015-930", "25", Power }, { "10", "7013-530", "25", Power }, { "10", "7016-730", "25", Power }, { "11", "7013-540", "30", Power }, { "14", "7013-540", "30", Power }, { "18", "7013-53H", "33", Power }, { "1C", "7013-550", "41.6", Power }, { "20", "7015-930", "25",
		Power }, { "2E", "7015-950", "41", Power }, { "30", "7013-520", "20", Power }, { "31", "7012-320", "20", Power }, { "34", "7013-52H", "25", Power }, { "35", "7012-32H", "25", Power }, { "37", "7012-340", "33", Power }, { "38", "7012-350", "41", Power }, { "41", "7011-20", "33", PowerPC }, {
		"45", "7011-220", "33", PowerPC }, { "42", "7006-41T/41W", "80", PowerPC }, { "43", "7008-M20", "33", Power }, { "43", "7008-M2A", "33", Power }, { "46", "7011-250", "66", PowerPC }, { "47", "7011-230", "45", RSC }, { "48", "7009-C10", "80", PowerPC },
/* 4C models use a different table -- see below */
{ "4C", "70XX", "(unknown)", unknown }, { "57", "7012-390", "67", Power2 }, { "57", "7030-3BT", "67", Power2 }, { "57", "9076-SP2 Thin", "67", Power2 }, { "58", "7012-380", "59", Power2 }, { "58", "7030-3AT", "59", Power2 }, { "59", "7012-39H", "67", Power2 }, { "59", "9076-SP2 Thin w/L2", "67",
		Power2 }, { "5C", "7013-560", "50", Power }, { "63", "7015-970", "50", Power }, { "63", "7015-97B", "50", Power }, { "64", "7015-980", "62.5", Power }, { "64", "7015-98B", "62.5", Power }, { "66", "7013-580", "62.5", Power }, { "67", "7013-570", "50", Power }, { "67", "7015-R10", "50",
		Power }, { "70", "7013-590", "66", Power2 }, { "70", "9076-SP2 Wide", "66", Power2 }, { "71", "7013-58H", "55", Power2 }, { "72", "7013-59H", "66", Power2 }, { "72", "7015-R20", "66", Power2 }, { "72", "9076-SP2 Wide", "66", Power2 }, { "75", "7012-370", "62", Power }, { "75", "7012-375",
		"62", Power }, { "75", "9076-SP1 Thin", "62", Power }, { "76", "7012-360", "50", Power }, { "76", "7012-365", "50", Power }, { "77", "7012-350", "41", Power }, { "77", "7012-355", "41", Power }, { "77", "7013-55L", "41.6", Power }, { "79", "7013-591", "77", Power2 }, { "79",
		"9076-SP2 Wide", "77", Power2 }, { "80", "7015-990", "71.5", Power2 }, { "81", "7015-R24", "71.5", P2SC }, { "89", "7013-595", "135", P2SC }, { "89", "9076-SP2 Wide", "135", P2SC }, { "90", "7009-C20", "120", PowerPC }, { "91", "7006-42W/42T", "120", PowerPC }, { "94", "7012-397", "160",
		P2SC }, { "94", "9076-SP2 Thin", "160", P2SC }, { "A0", "7013-J30", "75", PowerPC }, { "A1", "7013-J40", "112", PowerPC }, { "A3", "7015-R30", "(unknown)", PowerPC }, { "A4", "7015-R40", "(unknown)", PowerPC }, { "A4", "7015-R50", "(unknown)", PowerPC }, { "A4", "9076-SP2 High",
		"(unknown)", PowerPC }, { "A6", "7012-G30", "(unknown)", PowerPC }, { "A7", "7012-G40", "(unknown)", PowerPC }, { "C0", "7024-E20", "(unknown)", PowerPC }, { "C0", "7024-E30", "(unknown)", PowerPC }, { "C4", "7025-F30", "(unknown)", PowerPC }, { "F0", "7007-N40", "50", ThinkPad }, { "", "",
		"0", 0 } };

const
struct model _4C_models[] = { { "IBM,7017-S70", "7017-S70", "125", RS64 }, { "IBM,7017-S7A", "7017-S7A", "262", RD64_II }, { "IBM,7017-S80", "7017-S80", "450", RS_III }, { "IBM,7017-S85", "pSeries 680", "600", RS64_IV }, { "IBM,7025-F40", "7025-F40", "166/233", PowerPC_604e }, { "IBM,7025-F50",
		"7025-F50", "(unknown)", PowerPC_604e }, { "IBM,7025-F80", "7025-F80", "(unknown)", RS64_III }, { "IBM,7026-H10", "7025-H10", "166/233", PowerPC_604e }, { "IBM,7026-H50", "7025-H50", "(unknown)", PowerPC_604e }, { "IBM,7026-H80", "7025-H80", "(unknown)", RS64_III }, { "IBM,7026-M80",
		"7026-M80", "500", RS64_III }, { "IBM,7025-F40", "7025-F40", "166/233", PowerPC }, { "IBM,7025-F50", "7025-F50", "(unknown)", PowerPC }, { "IBM,7025-F80", "7025-F80", "450", PowerPC }, { "IBM,7026-B80", "pSeries 640", "375", Power3_II }, { "IBM,7026-H10", "7026-H10", "166/233", PowerPC }, {
		"IBM,7026-H50", "7026-H50", "(unknown)", PowerPC }, { "IBM,7026-H70", "7026-H70", "340", RS64_II }, { "IBM,7026-H80", "7026-H80", "450", PowerPC }, { "IBM,7026-M80", "7026-M80", "500", PowerPC }, { "IBM,7042-140", "7043-140", "166/200/233/332", PowerPC }, { "IBM,7042-150", "7043-150",
		"375", PowerPC }, { "IBM,7042-240", "7043-240", "166/233", PowerPC }, { "IBM,7043-140", "7043-140", "166/200/233/332", PowerPC }, { "IBM,7043-150", "7043-150", "375", PowerPC }, { "IBM,7043-240", "7043-240", "166/233", PowerPC }, { "IBM,7043-260", "7043-260", "200", Power3 }, { "IBM,7248",
		"7248-100", "100", PowerPersonal }, { "IBM,7248", "7248-120", "120", PowerPersonal }, { "IBM,7248", "7248-132", "132", PowerPersonal }, { "IBM,9076-270", "9076-SP Silver Node", "(unknown)", PowerPC }, { "", "", "", 0 }, 
};

/*  all GetInfo_ functions should return true, when the Information 
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

bool GetInfo_XServer_and_Video(QListView *lBox) {
	return GetInfo_XServer_Generic(lBox);
}

bool GetInfo_IRQ(QListView *) {
	return false;
}

bool GetInfo_DMA(QListView *) {
	return false;
}

bool GetInfo_PCI(QTreeWidget* tree) {
	return list_devices(tree, (char *)"PdDvLn like '*/pci/*'");
}

bool GetInfo_IO_Ports(QListView *) {
	return false;
}

bool GetInfo_Sound(QListView *) {
	return false;
}

bool GetInfo_SCSI(QListView *lBox) {
	return list_devices(lBox, (char *)"PdDvLn like '*/scsi/*'");
}

/* Parts taken from fsusage.c from the Midnight Commander (mc)

 Copyright (C) 1991, 1992 Free Software Foundation, In

 Return the number of TOSIZE-byte blocks used by
 BLOCKS FROMSIZE-byte blocks, rounding away from zero.
 TOSIZE must be positive.  Return -1 if FROMSIZE is not positive.  */

static long fs_adjust_blocks(long blocks, int fromsize, int tosize) {
	if (tosize <= 0)
		abort();
	if (fromsize <= 0)
		return -1;

	if (fromsize == tosize) /* E.g., from 512 to 512.  */
		return blocks;
	else if (fromsize > tosize) /* E.g., from 2048 to 512.  */
		return blocks * (fromsize / tosize);
	else
		/* E.g., from 256 to 512.  */
		return (blocks + (blocks < 0 ? -1 : 1)) / (tosize / fromsize);
}

/* Fill in the fields of FSP with information about space usage for
 the filesystem on which PATH resides.
 Return 0 if successful, -1 if not. */

#define CONVERT_BLOCKS(b) fs_adjust_blocks ((b), fsd.f_bsize, 512)

static int get_fs_usage(char *path, long *l_total, long *l_avail) {
	struct statfs fsd; /* 4.3BSD, SunOS 4, HP-UX, AIX.  */
	unsigned long fsu_blocks, fsu_bfree, fsu_bavail;

	*l_total = *l_avail = 0;
	if (statfs(path, &fsd) < 0)
		return -1;

	fsu_blocks = CONVERT_BLOCKS (fsd.f_blocks);
	fsu_bfree = CONVERT_BLOCKS (fsd.f_bfree);
	fsu_bavail = CONVERT_BLOCKS (fsd.f_bavail);

	*l_avail = getuid() ? fsu_bavail/2 : fsu_bfree/2;
	*l_total = fsu_blocks/2;

	return 0;
}

// Some Ideas taken from garbazo from his source in info_fbsd.cpp

bool GetInfo_Partitions(QListView *lbox) {
#define NUMCOLS 5
	QString Title[NUMCOLS];
	int n;

	struct fstab *fstab_ent;
	struct statvfs svfs;
	long total, avail;
	QString str;
	QString MB(i18nc("Mebibyte", "MiB "));

	if (setfsent() != 1) // Try to open fstab 
		return false;

	Title[0] = i18n("Device");
	Title[1] = i18n("Mount Point");
	Title[2] = i18n("FS Type");
	Title[3] = i18n("Total Size");
	Title[4] = i18n("Free Size");

	for (n=0; n<NUMCOLS; ++n) {
		lbox->addColumn(Title[n]);
	}

	while ((fstab_ent=getfsent())!=NULL) {
		/* fstab_ent->fs_type holds only "rw","xx","ro"... */
		memset(&svfs, 0, sizeof(svfs));
		statvfs(fstab_ent->fs_file, &svfs);
		get_fs_usage(fstab_ent->fs_file, &total, &avail);

		if (!strcmp(fstab_ent->fs_type, FSTAB_XX)) // valid drive ?
			svfs.f_basetype[0] = 0;

		if (svfs.f_basetype[0]) {
			new QListViewItem(lbox, QString(fstab_ent->fs_spec),
					QString(fstab_ent->fs_file) + QString("  "),
					(svfs.f_basetype[0] ? QString(svfs.f_basetype) : i18n("n/a")),
					Value((total+512)/1024,6) + MB,
					Value((avail+512)/1024,6) + MB);
		} else {
			new QListViewItem(lbox, QString(fstab_ent->fs_spec),
					QString(fstab_ent->fs_file) + QString("  "),
					(svfs.f_basetype[0] ? QString(svfs.f_basetype) : i18n("n/a")));
		}

	}
	endfsent();

	return true;
}
