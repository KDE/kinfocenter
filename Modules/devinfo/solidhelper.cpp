
/*
 *  solidhelper.cpp
 *
 *  Copyright (C) 2009 David Hubner <hubnerd@ntlworld.com>
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
 *
 */

#include "solidhelper.h"

Solid::DeviceInterface::Type SolidHelper::deviceType(const Solid::Device *dev)
{
  const Solid::DeviceInterface::Type needHardware[] = { 
    Solid::DeviceInterface::Processor,
    Solid::DeviceInterface::StorageDrive,
    Solid::DeviceInterface::AudioInterface,
    Solid::DeviceInterface::Video,
    Solid::DeviceInterface::SerialInterface,
    Solid::DeviceInterface::Battery, 
    Solid::DeviceInterface::PortableMediaPlayer,
    Solid::DeviceInterface::Camera,
    Solid::DeviceInterface::StorageVolume
  };
  
  for(unsigned int i=0;i<(sizeof(needHardware)/sizeof(Solid::DeviceInterface::Type));i++) { 
    if(dev->isDeviceInterface(needHardware[i]) == true) return needHardware[i];
  }
  return Solid::DeviceInterface::Unknown;
}
