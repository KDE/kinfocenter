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

#include <iostream>

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

bool GetInfo_Sound(QTreeWidget *tree) {
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
		QStringList deviceList;
		deviceList << i18n("Device Name") << deviceName;
		new QTreeWidgetItem(tree, deviceList);

		/* Manufacturer */
		status = AudioDeviceGetProperty(gOutputDeviceID, 1, 0, kAudioDevicePropertyDeviceManufacturer, &propertySize, manufacturer);
		if (status) {
			kDebug() << "get manufacturer failed, status = " << (int)status;
			return false;
		}
		QStringList manufacturerList;
		manufacturerList << i18n("Manufacturer") << manufacturer;
		new QTreeWidgetItem(tree, manufacturerList);
		return true;
	} else {
		return false;
	}
#else
	return false;
#endif
}

bool GetInfo_SCSI(QTreeWidget*) {
	return false;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
#ifdef Q_WS_X11
	return GetInfo_XServer_Generic(tree);
#else
	return false;
#endif
}
