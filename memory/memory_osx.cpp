/*
 *  Copyright (c) 2003 Benjamin Reed <ranger@befunk.com>
 *
 *  memory_osx.cpp is part of the KDE program kcminfo.  Copied wholesale
 *  from memory_fbsd.cpp =)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <unistd.h>
#include <stdlib.h>
#include <QFile>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/host_info.h>
#include <sys/stat.h>
#include <dirent.h>
#include <kdebug.h>

void KCMMemory::fetchValues()
{

	vm_statistics_data_t vm_info;
	mach_msg_type_number_t info_count;
	DIR *dirp;
	struct dirent *dp;
	t_memsize total;

	info_count = HOST_VM_INFO_COUNT;
	if (host_statistics(mach_host_self (), HOST_VM_INFO, (host_info_t)&vm_info, &info_count)) {
		kDebug() << "could not get memory statistics";
		return;
	}

	memoryInfos[TOTAL_MEM]    = MEMORY(vm_info.active_count + vm_info.inactive_count +
		vm_info.free_count + vm_info.wire_count) * vm_page_size;
	memoryInfos[FREE_MEM]     = MEMORY(vm_info.free_count) * vm_page_size;
	memoryInfos[SHARED_MEM]   = NO_MEMORY_INFO;
	memoryInfos[BUFFER_MEM]   = NO_MEMORY_INFO;
	memoryInfos[CACHED_MEM]   = NO_MEMORY_INFO;

	dirp = opendir("/private/var/vm");
	if (!dirp) {
		kDebug() << "unable to open /private/var/vm";
		return;
	}

	total = 0;

	while ((dp = readdir (dirp)) != NULL) {
		struct stat sb;
		char fname [MAXNAMLEN];

		if (strncmp (dp->d_name, "swapfile", 8))
			continue;

		strcpy (fname, "/private/var/vm/");
		strcat (fname, dp->d_name);
		if (stat (fname, &sb) < 0)
			continue;

		total += sb.st_size;
	}
	closedir (dirp);

	info_count = HOST_VM_INFO_COUNT;
	if (host_statistics (mach_host_self (), HOST_VM_INFO,
		(host_info_t) &vm_info, &info_count)) {
			kDebug() << "unable to get VM info";
	}

	memoryInfos[SWAP_MEM]     = total;
	// off_t used = (vm_info.pageouts - vm_info.pageins) * vm_page_size;
	memoryInfos[FREESWAP_MEM] = NO_MEMORY_INFO;

	/* free = MEMORY(vm_info.free_count) * vm_page_size;
	   used = MEMORY(vm_info.active_count) * vm_page_size;
	   total = MEMORY(vm_info.active_count + vm_info.inactive_count +
		vm_info.free_count + vm_info.wire_count) * vm_page_size; */

}
