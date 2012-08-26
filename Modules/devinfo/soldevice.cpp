
/*
 *  soldevice.cpp
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

#include "soldevice.h"

// Con

SolDevice::SolDevice(const Solid::DeviceInterface::Type &type) : 
  QTreeWidgetItem(), deviceSet(0) 
{ 
  deviceTypeHolder=type;
  setText(0,Solid::DeviceInterface::typeToString(type));
}

SolDevice::SolDevice(QTreeWidgetItem *parent) : 
  QTreeWidgetItem(parent), deviceSet(0)
{
  deviceTypeHolder = Solid::DeviceInterface::Unknown;
}

SolDevice::SolDevice(const Solid::DeviceInterface::Type &type, const QString &typeName) : 
  QTreeWidgetItem(), deviceSet(0)
{
  deviceTypeHolder=type;
  setText(0,typeName);
  
  setDefaultListing(type);
}

SolDevice::SolDevice(QTreeWidgetItem *parent, const Solid::Device &device) : 
  QTreeWidgetItem(parent), tiedDevice(device)
{
  deviceTypeHolder = Solid::DeviceInterface::Unknown;
  
  deviceSet = device.isValid();
  setDefaultDeviceText();
  setDefaultDeviceIcon();
  setDefaultDeviceToolTip();
}

//Sets

void SolDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
  createDeviceChildren<SolDevice>(this,QString(),type);
}

void SolDevice::setDefaultDeviceText() 
{  
  QString ddtString = i18nc("unknown device", "Unknown");
  
  if(deviceSet) ddtString = tiedDevice.product();
  setText(0,ddtString);
}

void SolDevice::setDefaultDeviceIcon() 
{ 
  KIcon ddiString = KIcon("kde");
  
  if(deviceSet) ddiString = KIcon(tiedDevice.icon());
  setDeviceIcon(ddiString);
}

void SolDevice::setDefaultDeviceToolTip() 
{
  QString ddttString = i18nc("Default device tooltip","A Device");
  
  if(deviceSet) ddttString = tiedDevice.description();
  setDeviceToolTip(ddttString);
}

void SolDevice::setDeviceIcon(const KIcon &icon)
{ 
   setIcon(0,icon);
}

void SolDevice::setDeviceText(const QString &text) 
{  
  setText(0,text);
}

void SolDevice::setDeviceToolTip(const QString &toolTipText) 
{ 
  setToolTip(0,toolTipText);
}

// Gets

QVListLayout *SolDevice::infoPanelLayout() 
{  
  deviceInfoLayout = new QVListLayout();
  return deviceInfoLayout;
}

QIcon SolDevice::deviceIcon() const 
{  
  return icon(0);
}

Solid::DeviceInterface::Type SolDevice::deviceType() const 
{
  return deviceTypeHolder;
}

Solid::Device *SolDevice::device() 
{  
   return &tiedDevice;
}

QString SolDevice::udi() const 
{ 
  return tiedDevice.udi();
}

// Is

bool SolDevice::isDeviceSet() 
{  
  return deviceSet;
}
