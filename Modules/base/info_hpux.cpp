
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
#include <sys/types.h>
#include <sys/param.h>
#include <sys/pstat.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <sys/vfs.h>
#include <fstab.h>
#include <stdlib.h>
#include <QFile>

#	define INFO_PCI			""	// Please, who know it ????
#	define INFO_PCI_EISA		"/etc/eisa/system.sci" // File !
#	define INFO_IOPORTS_1		"/etc/dmesg"		// as pipe !
#	define INFO_IOPORTS_2		"/usr/sbin/dmesg"	// as pipe !
#	define INFO_DEVICES		"/etc/ioscan" 	// as pipe !!

/*	The following table is from an HP-UX 10.20 System
 build out of the files
 "/usr/lib/sched.models" 
 or
 "/opt/langtools/lib/sched.models"
 If you have more entries, then please add them or send them to me!
 */

// entries for PA_REVISION[]
enum V_ENTRIES
{	V_1x0,
	V_1x1, V_1x1a, V_1x1b, V_1x1c, V_1x1d, V_1x1e,
	V_2x0,
	V_LAST};

static const char PA_REVISION[V_LAST][7] = { "1.0", "1.1", "1.1a", "1.1b", "1.1c", "1.1d", "1.1e", "2.0" };

// entries for PA_NAME[]
enum PA_ENTRIES
{	PA7000,
	PA7100, PA7100LC, PA7200, PA7300,
	PA8000, PA8200, PA8500,
	PARISC_PA_LAST};

static const char PA_NAME[PARISC_PA_LAST][11] = { "PA7000", "PA7100", "PA7100LC", "PA7200", "PA7300", "PA8000", "PA8200", "PA8500" };

struct _type_LOOKUPTABLE {
	char Name[8];
	unsigned short parisc_rev; // = enum V_ENTRIES 
	unsigned short parisc_name; // = enum PA_ENTRIES 
};

static const struct _type_LOOKUPTABLE PA_LOOKUPTABLE[] = {
/* 			VERSION A.00.07    
 (there seems to exist several different files with same version-number !)*/
{ "600", V_1x0, PA7000 }, { "635", V_1x0, PA7000 }, { "645", V_1x0, PA7000 }, { "700", V_1x1, PA7000 }, { "705", V_1x1a, PA7000 }, { "710", V_1x1a, PA7000 }, { "712", V_1x1c, PA7100LC }, { "715", V_1x1c, PA7100LC }, { "720", V_1x1a, PA7000 }, { "722", V_1x1c, PA7100LC },
		{ "725", V_1x1c, PA7100LC }, { "728", V_1x1d, PA7200 }, { "730", V_1x1a, PA7000 }, { "735", V_1x1b, PA7100 },
		{ "742", V_1x1b, PA7100 },
		{ "743", V_1x1b, PA7100 }, // or a 1.1c,PA7100LC !!
		{ "744", V_1x1e, PA7300 }, { "745", V_1x1b, PA7100 }, { "747", V_1x1b, PA7100 }, { "750", V_1x1a, PA7000 }, { "755", V_1x1b, PA7100 }, { "770", V_1x1d, PA7200 }, { "777", V_1x1d, PA7200 }, { "778", V_1x1e, PA7300 }, { "779", V_1x1e, PA7300 }, { "780", V_2x0, PA8000 },
		{ "781", V_2x0, PA8000 }, { "782", V_2x0, PA8200 }, { "783", V_2x0, PA8500 }, { "785", V_2x0, PA8500 },
		{ "800", V_1x0, PA7000 }, // and one with: 2.0 / PA8000
		{ "801", V_1x1c, PA7100LC }, { "802", V_2x0, PA8000 }, { "803", V_1x1e, PA7300 }, { "804", V_2x0, PA8000 }, { "806", V_1x1c, PA7100LC }, { "807", V_1x1a, PA7000 }, { "808", V_1x0, PA7000 }, { "809", V_1x1d, PA7200 }, { "810", V_2x0, PA8000 }, { "811", V_1x1c, PA7100LC }, { "813", V_1x1e,
				PA7300 }, { "815", V_1x0, PA7000 }, { "816", V_1x1c, PA7100LC }, { "817", V_1x1a, PA7000 }, { "819", V_1x1d, PA7200 }, { "820", V_2x0, PA8000 }, { "821", V_1x1d, PA7200 }, { "822", V_1x0, PA7000 }, { "825", V_1x0, PA7000 }, { "826", V_1x1c, PA7100LC }, { "827", V_1x1a, PA7000 }, {
				"829", V_1x1d, PA7200 }, { "831", V_1x1d, PA7200 }, { "832", V_1x0, PA7000 }, { "834", V_1x0, PA7000 }, { "835", V_1x0, PA7000 }, { "837", V_1x1a, PA7000 }, { "839", V_1x1d, PA7200 }, { "840", V_1x0, PA7000 }, { "841", V_1x1d, PA7200 }, { "842", V_1x0, PA7000 }, { "845", V_1x0,
				PA7000 }, { "847", V_1x1a, PA7000 }, { "849", V_1x1d, PA7200 }, { "850", V_1x0, PA7000 }, { "851", V_1x1d, PA7200 }, { "852", V_1x0, PA7000 }, { "855", V_1x0, PA7000 }, { "856", V_1x1c, PA7100LC }, { "857", V_1x1a, PA7000 }, { "859", V_1x1d, PA7200 }, { "860", V_1x0, PA7000 }, {
				"861", V_2x0, PA8000 }, { "865", V_1x0, PA7000 }, { "869", V_1x1d, PA7200 }, { "870", V_1x0, PA7000 }, { "871", V_2x0, PA8000 }, { "877", V_1x1a, PA7000 }, { "879", V_2x0, PA8000 }, { "887", V_1x1b, PA7100 }, { "889", V_2x0, PA8000 }, { "890", V_1x0, PA7000 }, { "891", V_1x1b,
				PA7100 }, { "892", V_1x1b, PA7100 }, { "893", V_2x0, PA8000 }, { "895", V_2x0, PA8000 }, { "896", V_2x0, PA8000 }, { "897", V_1x1b, PA7100 }, { "898", V_2x0, PA8200 }, { "899", V_2x0, PA8200 }, { "900", V_1x0, PA7000 }, { "B115", V_1x1e, PA7300 }, { "B120", V_1x1e, PA7300 }, {
				"B132L", V_1x1e, PA7300 }, { "B160L", V_1x1e, PA7300 }, { "B180L", V_1x1e, PA7300 }, { "C100", V_1x1d, PA7200 }, { "C110", V_1x1d, PA7200 }, { "C115", V_1x1e, PA7300 }, { "C120", V_1x1e, PA7300 }, { "C130", V_2x0, PA8000 }, { "C140", V_2x0, PA8000 }, { "C160L", V_1x1e, PA7300 }, {
				"C160", V_2x0, PA8000 }, { "C180L", V_1x1e, PA7300 }, { "C180-XP", V_2x0, PA8000 }, { "C180", V_2x0, PA8000 }, { "C200+", V_2x0, PA8200 }, { "C230+", V_2x0, PA8200 }, { "C240+", V_2x0, PA8200 }, { "CB260", V_2x0, PA8200 },
		{ "D200", V_1x1d, PA7200 }, // or: 1.1c, PA7100LC
		{ "D210", V_1x1d, PA7200 }, // or: 1.1c, PA7100LC
		{ "D220", V_1x1e, PA7300 }, { "D230", V_1x1e, PA7300 }, { "D250", V_1x1d, PA7200 }, { "D260", V_1x1d, PA7200 }, { "D270", V_2x0, PA8000 }, { "D280", V_2x0, PA8000 }, { "D310", V_1x1c, PA7100LC }, { "D320", V_1x1e, PA7300 }, { "D330", V_1x1e, PA7300 }, { "D350", V_1x1d, PA7200 }, { "D360",
				V_1x1d, PA7200 }, { "D370", V_2x0, PA8000 }, { "D380", V_2x0, PA8000 }, { "D400", V_1x1d, PA7200 }, { "D410", V_1x1d, PA7200 }, { "D650", V_2x0, PA8000 }, { "DX0", V_1x1c, PA7100LC }, { "DX5", V_1x1c, PA7100LC }, { "DXO", V_1x1c, PA7100LC }, { "E25", V_1x1c, PA7100LC }, { "E35",
				V_1x1c, PA7100LC }, { "E45", V_1x1c, PA7100LC }, { "E55", V_1x1c, PA7100LC }, { "F10", V_1x1a, PA7000 }, { "F20", V_1x1a, PA7000 }, { "F30", V_1x1a, PA7000 }, { "G30", V_1x1a, PA7000 }, { "G40", V_1x1a, PA7000 }, { "G50", V_1x1b, PA7100 }, { "G60", V_1x1b, PA7100 }, { "G70", V_1x1b,
				PA7100 }, { "H20", V_1x1a, PA7000 }, { "H30", V_1x1a, PA7000 }, { "H40", V_1x1a, PA7000 }, { "H50", V_1x1b, PA7100 }, { "H60", V_1x1b, PA7100 }, { "H70", V_1x1b, PA7100 }, { "I30", V_1x1a, PA7000 }, { "I40", V_1x1a, PA7000 }, { "I50", V_1x1b, PA7100 }, { "I60", V_1x1b, PA7100 }, {
				"I70", V_1x1b, PA7100 }, { "J200", V_1x1d, PA7200 }, { "J210XC", V_1x1d, PA7200 }, { "J210", V_1x1d, PA7200 }, { "J220", V_2x0, PA8000 }, { "J2240", V_2x0, PA8200 }, { "J280", V_2x0, PA8000 }, { "J282", V_2x0, PA8000 }, { "J400", V_2x0, PA8000 }, { "J410", V_2x0, PA8000 }, { "K100",
				V_1x1d, PA7200 }, { "K200", V_1x1d, PA7200 }, { "K210", V_1x1d, PA7200 }, { "K230", V_1x1d, PA7200 }, { "K250", V_2x0, PA8000 }, { "K260", V_2x0, PA8000 }, { "K370", V_2x0, PA8200 }, { "K380", V_2x0, PA8200 }, { "K400", V_1x1d, PA7200 }, { "K410", V_1x1d, PA7200 }, { "K420", V_1x1d,
				PA7200 }, { "K430", V_1x1d, PA7200 }, { "K450", V_2x0, PA8000 }, { "K460", V_2x0, PA8000 }, { "K470", V_2x0, PA8200 }, { "K570", V_2x0, PA8200 }, { "K580", V_2x0, PA8200 }, { "S700i", V_1x1e, PA7300 }, { "S715", V_1x1e, PA7300 }, { "S744", V_1x1e, PA7300 },
		{ "S760", V_1x1e, PA7300 }, { "T500", V_1x1c, PA7100LC }, // or: 1.1b, PA7100
		{ "T520", V_1x1b, PA7100 }, { "T540", V_2x0, PA8000 }, { "T600", V_2x0, PA8000 }, { "V2000", V_2x0, PA8000 }, { "V2200", V_2x0, PA8200 }, { "V2250", V_2x0, PA8200 }, { "V2500", V_2x0, PA8500 },

		{ "", 0, 0 } /* Last Entry has to be empty. */
};

/* Helper-Functions */

// Value() is defined in info.cpp !!!

static bool Find_in_LOOKUPTABLE(QListView *lBox, char *machine) {
	char *Machine;
	int len;
	const struct _type_LOOKUPTABLE *Entry = PA_LOOKUPTABLE;
	QString str;
	QListViewItem* olditem = 0;

	Machine = machine; // machine is like: "9000/715/D"
	while ((*Machine) && (*Machine!='/'))
		++Machine;

	if (*Machine)
		++Machine;
	else
		Machine=machine;

	len = strlen(Machine);

	while (Entry->Name[0]) {
		if (strncmp(Entry->Name, Machine, len)==0) {
			olditem = new QListViewItem(lBox, olditem, i18n("PA-RISC Processor"),
					QString(PA_NAME[Entry->parisc_name]));
			olditem = new QListViewItem(lBox, olditem, i18n("PA-RISC Revision"),
					QString("PA-RISC ") + QString(PA_REVISION[Entry->parisc_rev]));
			return true;
		} else
			++Entry; // next Entry !
	}

	return false;
}

/*  all following functions should return true, when the Information 
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

bool GetInfo_ReadfromFile(QListView *lBox, const char *Name) {
	char buf[2048];

	QFile *file = new QFile(Name);
	QListViewItem* olditem = 0;

	if (!file->open(QIODevice::ReadOnly)) {
		delete file;
		return false;
	}

	while (file->readLine(buf, sizeof(buf)-1) >= 0) {
		if (strlen(buf))
			olditem = new QListViewItem(lBox, olditem, QString::fromLocal8Bit(buf));
	}

	file->close();
	delete file;
	return (lBox->childCount());
}

bool GetInfo_IRQ(QListView *) {
	return false;
}

bool GetInfo_DMA(QListView *) {
	return false;
}

bool GetInfo_PCI(QTreeWidget* tree) {
	return (GetInfo_ReadfromFile(tree, INFO_PCI) + GetInfo_ReadfromFile(tree, INFO_PCI_EISA) );
}

bool GetInfo_IO_Ports(QListView *lBox) {
	if (GetInfo_ReadfromPipe(lBox, INFO_IOPORTS_1, false))
		return true;
	else
		return GetInfo_ReadfromPipe(lBox, INFO_IOPORTS_2, false);
}

bool GetInfo_SCSI(QListView *lBox) {
	return GetInfo_ReadfromPipe(lBox, INFO_DEVICES, false);
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

bool GetInfo_XServer_and_Video(QListView *lBox) {
	lBox = lBox;
	return GetInfo_XServer_Generic(lBox);
}
