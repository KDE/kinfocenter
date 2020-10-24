/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "solidhelper.h"

// Solid
#include <solid/device.h>

Solid::DeviceInterface::Type SolidHelper::deviceType(const Solid::Device *dev)
{
    const Solid::DeviceInterface::Type needHardware[] = {
        Solid::DeviceInterface::Processor,
        Solid::DeviceInterface::StorageDrive,
        Solid::DeviceInterface::Battery,
        Solid::DeviceInterface::PortableMediaPlayer,
        Solid::DeviceInterface::Camera,
        Solid::DeviceInterface::StorageVolume,
    };

    for (unsigned int i = 0, total = (sizeof(needHardware) / sizeof(Solid::DeviceInterface::Type)); i < total; ++i) {
        if (dev->isDeviceInterface(needHardware[i]) == true) {
            return needHardware[i];
        }
    }
    return Solid::DeviceInterface::Unknown;
}
