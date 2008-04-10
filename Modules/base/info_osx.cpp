/*
 *  Copyright (c) 2003 Benjamin Reed <ranger@befunk.com>
 *
 *  info_osx.cpp is part of the KDE program kcminfo.  Copied wholesale
 *  from info_fbsd.cpp =)
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

/*
 * all following functions should return true, when the Information
 * was filled into the lBox-Widget. Returning false indicates that
 * information was not available.
 */

#include <sys/types.h>
#include <sys/sysctl.h>

#include <fstab.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream.h>

#include <qdict.h>
#include <QFile>
#include <QFontMetrics>

#include <QTextStream>

#include <kdebug.h>

#include <mach/mach.h>
#include <mach-o/arch.h>

#ifdef HAVE_COREAUDIO
#include <CoreAudio/CoreAudio.h>
#endif

#include <machine/limits.h>

bool GetInfo_CPU(QListView *lBox) {

	QString cpustring;

	kern_return_t ret;
	struct host_basic_info basic_info;
	unsigned int count=HOST_BASIC_INFO_COUNT;

	ret=host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&basic_info, &count);
	if (ret != KERN_SUCCESS) {
		kDebug() << "unable to get host information from mach";
		return false;
	} else {
		kDebug() << "got Host Info: (" << basic_info.avail_cpus << ") CPUs available";
		const NXArchInfo *archinfo;
		archinfo=NXGetArchInfoFromCpuType(basic_info.cpu_type, basic_info.cpu_subtype);
		new QListViewItem(lBox, i18n("Kernel is configured for %1 CPUs", basic_info.max_cpus));
		for (int i = 1; i <= basic_info.avail_cpus; i++) {
			cpustring = i18n("CPU %1: %2", i, archinfo->description);
			new QListViewItem(lBox, cpustring);
		}
		return true;
	}
	return false;
}

bool GetInfo_IRQ(QListView *) {
	return false;
}

bool GetInfo_DMA(QListView *) {
	return false;
}

bool GetInfo_PCI(QTreeWidget*) {
	return false;
}

bool GetInfo_IO_Ports(QListView *) {
	return false;
}

bool GetInfo_Sound(QListView *lBox) {
#ifdef HAVE_COREAUDIO
#define qMaxStringSize 1024
	OSStatus status;
	AudioDeviceID gOutputDeviceID;
	unsigned long propertySize;
	char deviceName[qMaxStringSize];
	char manufacturer[qMaxStringSize];
	propertySize = sizeof(gOutputDeviceID);
	status = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &propertySize, &gOutputDeviceID);
	if (status) {
		kDebug() << "get default output device failed, status = " << (int)status;
		return false;
	}

	if (gOutputDeviceID != kAudioDeviceUnknown) {

		propertySize = qMaxStringSize;

		/* Device Name */
		status = AudioDeviceGetProperty(gOutputDeviceID, 1, 0, kAudioDevicePropertyDeviceName, &propertySize, deviceName);
		if (status) {
			kDebug() << "get device name failed, status = " << (int)status;
			return false;
		}
		new QListViewItem(lBox, i18n("Device Name: %1", deviceName));

		/* Manufacturer */
		status = AudioDeviceGetProperty(gOutputDeviceID, 1, 0, kAudioDevicePropertyDeviceManufacturer, &propertySize, manufacturer);
		if (status) {
			kDebug() << "get manufacturer failed, status = " << (int)status;
			return false;
		}
		new QListViewItem(lBox, i18n("Manufacturer: %1", manufacturer));
		return true;
	} else {
		return false;
	}
#else
	return false;
#endif
}

bool GetInfo_SCSI(QListView *lbox) {
	return false;
}

bool GetInfo_Partitions(QListView *lbox) {
	return false;
}

bool GetInfo_XServer_and_Video(QListView *lBox) {
	return GetInfo_XServer_Generic(lBox);
}

bool GetInfo_Devices(QListView *lbox) {
	return false;
}
