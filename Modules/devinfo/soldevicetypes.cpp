
/*
 *  soldevicetypes.cpp
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

#include "soldevicetypes.h"

#include <kcapacitybar.h>
// ---- Processor

SolProcessorDevice::SolProcessorDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::Processor;
    setDefaultDeviceText();
}

SolProcessorDevice::SolProcessorDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::Processor;
  
  setDeviceIcon(KIcon("cpu"));
  setDeviceText(i18n("Processors"));
  setDefaultListing(type);
}

void SolProcessorDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
  createDeviceChildren<SolProcessorDevice>(this,QString(),type);
}

void SolProcessorDevice::setDefaultDeviceText() 
{
  const Solid::Processor *prodev = interface<const Solid::Processor>(); 
  
  if(!prodev) return;
  setText(0,i18n("Processor %1", QString::number(prodev->number())));
}

QVListLayout *SolProcessorDevice::infoPanelLayout() 
{
  QStringList labels;
  const Solid::Processor *prodev = interface<const Solid::Processor>(); 
  
  if(!prodev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QStringList extensions;
  const Solid::Processor::InstructionSets insSets = prodev->instructionSets();
  
  if (insSets & Solid::Processor::IntelMmx) extensions << i18n("Intel MMX");
  if (insSets & Solid::Processor::IntelSse) extensions << i18n("Intel SSE");
  if (insSets & Solid::Processor::IntelSse2) extensions << i18n("Intel SSE2");
  if (insSets & Solid::Processor::IntelSse3) extensions << i18n("Intel SSE3");
  if (insSets & Solid::Processor::IntelSse4) extensions << i18n("Intel SSE4");
  if (insSets & Solid::Processor::Amd3DNow) extensions << i18n("AMD 3DNow");
  if (insSets & Solid::Processor::AltiVec) extensions << i18n("ATI IVEC");
  if(extensions.isEmpty()) extensions << i18nc("no instruction set extensions", "None");
  
  labels << i18n("Processor Number: ")
  << InfoPanel::friendlyString(QString::number(prodev->number())) 
  << i18n("Max Speed: ") 
  << InfoPanel::friendlyString(QString::number(prodev->maxSpeed()))
  << i18n("Supported Instruction Sets: ")
  << extensions.join("\n");
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// ---- Storage

SolStorageDevice::SolStorageDevice(QTreeWidgetItem *parent, const Solid::Device &device, const storageChildren &c) :
  SolDevice(parent, device)
{
  deviceTypeHolder = Solid::DeviceInterface::StorageDrive;
  setDefaultDeviceText();
   
  if(c == CREATECHILDREN) 
  {
    createDeviceChildren<SolVolumeDevice>(this,device.udi(),Solid::DeviceInterface::StorageVolume);
  }
}

SolStorageDevice::SolStorageDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::StorageDrive;
  
  setDeviceIcon(KIcon("drive-harddisk"));
  setDeviceText(i18n("Storage Drives"));
  setDefaultListing(type);
}

void SolStorageDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolStorageDevice>(this,QString(),type);
}

void SolStorageDevice::setDefaultDeviceText() 
{  
  const Solid::StorageDrive *stodev = interface<const Solid::StorageDrive>();
  if(!stodev) return;
  
  QString storageType;
  switch (stodev->driveType())
  {
    case Solid::StorageDrive::HardDisk: 
      storageType = i18n("Hard Disk Drive");  break;
    case Solid::StorageDrive::CompactFlash:
      storageType = i18n("Compact Flash Reader");  break;
    case Solid::StorageDrive::SmartMedia:
      storageType = i18n("Smart Media Reader");  break;
    case Solid::StorageDrive::SdMmc:
      storageType = i18n("SD/MMC Reader"); break;
    case Solid::StorageDrive::CdromDrive:
      storageType = i18n("Optical Drive"); break;
    case Solid::StorageDrive::MemoryStick:
      storageType = i18n("Memory Stick Reader"); break;
    case Solid::StorageDrive::Xd:
      storageType = i18n("xD Reader"); break;
    default:
      storageType = i18n("Unknown Drive"); 
  }
  
  QString deviceText = storageType;
  setDeviceText(deviceText);
}

QVListLayout *SolStorageDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::StorageDrive *stodev = interface<const Solid::StorageDrive>(); 
  
  if(!stodev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QString bus;
  switch(stodev->bus())
  {
    case Solid::StorageDrive::Ide:
      bus = i18n("IDE"); break;
    case Solid::StorageDrive::Usb:
      bus = i18n("USB"); break;
    case Solid::StorageDrive::Ieee1394:
      bus = i18n("IEEE1394"); break;
    case Solid::StorageDrive::Scsi:
      bus = i18n("SCSI"); break;
    case Solid::StorageDrive::Sata:
      bus = i18n("SATA"); break;
    case Solid::StorageDrive::Platform:
      bus = i18nc("platform storage bus", "Platform"); break;
    default:
      bus = i18nc("unknown storage bus", "Unknown"); 
  }
  
  labels << i18n("Bus: ")
  << bus
  << i18n("Hotpluggable?")
  << InfoPanel::convertTf(stodev->isHotpluggable())
  << i18n("Removable?") 
  << InfoPanel::convertTf(stodev->isRemovable());
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// ---- Network

SolNetworkDevice::SolNetworkDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::NetworkInterface;
  
  setDefaultDeviceText();
  setDefaultDeviceIcon();
}

SolNetworkDevice::SolNetworkDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::NetworkInterface;
  
  setDeviceIcon(KIcon("network-wired"));
  setDeviceText(i18n("Network Interfaces"));
  setDefaultListing(type);
}

void SolNetworkDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolNetworkDevice>(this,QString(),type);
}

void SolNetworkDevice::setDefaultDeviceText() 
{    
  QString actTog = i18n("Connected");
  const Solid::NetworkInterface *netdev = interface<const Solid::NetworkInterface>(); 
  if(!netdev) return;
  
  QString deviceText = netdev->ifaceName()
  + " (" 
  + (netdev->isWireless() ? i18n("Wireless") : i18n("Wired")) 
  + ") ";
  
  setDeviceText(deviceText);
}

void SolNetworkDevice::setDefaultDeviceIcon()
{  
  const Solid::NetworkInterface *netdev = interface<const Solid::NetworkInterface>(); 
  if(!netdev) return;
  
  if(netdev->isWireless() == true)
  {
    setDeviceIcon(KIcon("network-wireless"));
  } else {
    setDeviceIcon(KIcon("network-wired"));
  }
}

QVListLayout *SolNetworkDevice::infoPanelLayout() 
{ 
  QStringList labels;
  const Solid::NetworkInterface *netdev = interface<const Solid::NetworkInterface>(); 
  
  if(!netdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  labels << i18n("Hardware Address: ")
  << InfoPanel::friendlyString(netdev->hwAddress())
  << i18n("Wireless?")
  << InfoPanel::convertTf(netdev->isWireless());
  
  deviceInfoLayout->applyQListToLayout(labels); 
  return deviceInfoLayout;
}

void SolNetworkDevice::refreshName() 
{ 
  setDefaultDeviceText();
}

// ---- Volume

SolVolumeDevice::SolVolumeDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::StorageVolume;
}

SolVolumeDevice::SolVolumeDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::StorageVolume;
  
  setDefaultListing(type);
}

void SolVolumeDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolVolumeDevice>(this,QString(),type);
}

QVListLayout *SolVolumeDevice::infoPanelLayout() 
{
  QStringList labels;
  KCapacityBar *usageBar = NULL;
  
  const Solid::StorageVolume *voldev = interface<const Solid::StorageVolume>();
  const Solid::StorageAccess *accdev = interface<const Solid::StorageAccess>();
  
  if(!voldev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QString usage;
  switch(voldev->usage()) 
  {
    case Solid::StorageVolume::Unused:
      usage = i18n("Unused"); break;
    case Solid::StorageVolume::FileSystem:
      usage = i18n("File System"); break;
    case Solid::StorageVolume::PartitionTable:
      usage = i18n("Partition Table"); break;   
    case Solid::StorageVolume::Raid:
      usage = i18n("Raid"); break;
    case Solid::StorageVolume::Encrypted:
      usage = i18n("Encrypted"); break;
    default: 
      usage = i18nc("unknown volume usage", "Unknown");
  }
  
  labels << i18n("File System Type: ") 
  << InfoPanel::friendlyString(voldev->fsType())
  << i18n("Label: ")
  << InfoPanel::friendlyString(voldev->label(),i18n("Not Set"))
  << i18n("Volume Usage: ")
  << usage
  << i18n("UUID: ")
  << InfoPanel::friendlyString(voldev->uuid());
  
  if(accdev) 
  {  
    labels << "--"
    << i18n("Mounted At: ") 
    << InfoPanel::friendlyString(accdev->filePath(),i18n("Not Mounted"));
    
    if(!accdev->filePath().isEmpty()) 
    {  
      KDiskFreeSpaceInfo mountSpaceInfo = KDiskFreeSpaceInfo::freeSpaceInfo(accdev->filePath());
      
      labels << i18n("Volume Space:");
      
      usageBar = new KCapacityBar();
      usageBar->setValue(static_cast<int>((mountSpaceInfo.used() * 100) / mountSpaceInfo.size()));
      usageBar->setText(
            i18nc("Available space out of total partition size (percent used)",
                  "%1 free of %2 (%3% used)",
                  KGlobal::locale()->formatByteSize(mountSpaceInfo.available()),
                  KGlobal::locale()->formatByteSize(mountSpaceInfo.size()),
                  usageBar->value()));
    }

  }
  
  deviceInfoLayout->applyQListToLayout(labels);
  if(usageBar) deviceInfoLayout->addWidget(usageBar);
    
  return deviceInfoLayout;
}
  
// -- Audio

SolAudioDevice::SolAudioDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::AudioInterface;
}

SolAudioDevice::SolAudioDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::AudioInterface;
  
  setDeviceIcon(KIcon("audio-card"));
  setDeviceText(i18n("Audio Interfaces"));
  setDefaultListing(type);
}

void SolAudioDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  Q_UNUSED(type);
  alsaSubItem = NULL; ossSubItem = NULL;
  
  listAlsa();
  listOss();
}

void SolAudioDevice::listAlsa() 
{ 
  const Solid::Predicate alsaPred =
    Solid::Predicate(Solid::DeviceInterface::AudioInterface,"driver", "Alsa",Solid::Predicate::Equals);
  const QList<Solid::Device> list = Solid::Device::listFromQuery(alsaPred, QString());
  
  if(list.count() <= 0) return;
  
  createSubItems(ALSA);
  foreach(const Solid::Device &dev, list) addItem(dev);
}

void SolAudioDevice::listOss() 
{  
  const Solid::Predicate ossPred =
    Solid::Predicate(Solid::DeviceInterface::AudioInterface,"driver", "OpenSoundSystem",Solid::Predicate::Equals);
  const QList<Solid::Device> list = Solid::Device::listFromQuery(ossPred, QString());
  
  if(list.count() <= 0) return;
  
  createSubItems(OSS);
  foreach(const Solid::Device &dev, list) addItem(dev);
}

void SolAudioDevice::createSubItems(const SubMenus &menus)
{  
  if(menus == ALSA)
  {
    alsaSubItem = new SolDevice(this);
    alsaSubItem->setDeviceIcon(KIcon("audio-card"));
    alsaSubItem->setText(0,i18n("Alsa Interfaces"));
  } else {
    ossSubItem = new SolDevice(this);
    ossSubItem->setDeviceIcon(KIcon("audio-card"));
    ossSubItem->setText(0,i18n("Open Sound System Interfaces"));
  }
}

void SolAudioDevice::addItem(Solid::Device dev) 
{  
   const Solid::AudioInterface *auddev = interface<const Solid::AudioInterface>(dev);
   if(!auddev) return;
  
    switch(auddev->driver())
    {
      case Solid::AudioInterface::Alsa:
	if(!alsaSubItem) createSubItems(ALSA);
	new SolAudioDevice(alsaSubItem,dev);
	break;
      case Solid::AudioInterface::OpenSoundSystem:
	if(!ossSubItem) createSubItems(OSS);
	new SolAudioDevice(ossSubItem,dev);
	break;
      default:
	new SolAudioDevice(this,dev);
    }
}

QVListLayout *SolAudioDevice::infoPanelLayout() 
{
  QStringList labels;
  const Solid::AudioInterface *auddev = interface<const Solid::AudioInterface>(); 
  
  if(!auddev) return NULL;
  deviceInfoLayout = new QVListLayout();

  QString AiType;
  switch(auddev->deviceType()) 
  {
    case Solid::AudioInterface::AudioControl:
      AiType = i18n("Control"); break;
    case Solid::AudioInterface::AudioInput:
      AiType = i18n("Input"); break;
    case Solid::AudioInterface::AudioOutput:
      AiType = i18n("Output"); break;
    default:
      AiType = i18nc("unknown audio interface type", "Unknown");
  }
  
  QString ScType;
  switch(auddev->soundcardType())
  {
    case Solid::AudioInterface::InternalSoundcard:
      ScType = i18n("Internal Soundcard"); break;
    case Solid::AudioInterface::UsbSoundcard:
      ScType = i18n("USB Soundcard"); break;
    case Solid::AudioInterface::FirewireSoundcard:
      ScType = i18n("Firewire Soundcard"); break;
    case Solid::AudioInterface::Headset:
      ScType = i18n("Headset"); break;
    case Solid::AudioInterface::Modem:
      ScType = i18n("Modem"); break;
    default:
      ScType = i18nc("unknown sound card type", "Unknown"); 
  }
  
  labels << i18n("Audio Interface Type: ")
  << AiType
  << i18n("Soundcard Type: ")
  << ScType;
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// Button

SolButtonDevice::SolButtonDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::Button;
  
  setDefaultDeviceIcon();
}

SolButtonDevice::SolButtonDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::Button;
  
  setDeviceIcon(KIcon("insert-button"));
  setDeviceText(i18n("Device Buttons"));
  setDefaultListing(type);
}

void SolButtonDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolButtonDevice>(this,QString(),type);
}

void SolButtonDevice::setDefaultDeviceIcon() 
{
  setDeviceIcon(KIcon("insert-button"));
}

QVListLayout *SolButtonDevice::infoPanelLayout() 
{
  QStringList labels;
  const Solid::Button *butdev = interface<const Solid::Button>();
  
  if(!butdev) return NULL;
  deviceInfoLayout = new QVListLayout();
 
  QString type;
  switch(butdev->type()) 
  {
    case Solid::Button::LidButton:
      type = i18n("Lid Button"); break;
    case Solid::Button::PowerButton:
      type = i18n("Power Button"); break;
    case Solid::Button::SleepButton:
      type = i18n("Sleep Button"); break;
    case Solid::Button::TabletButton:
      type = i18n("Tablet Button"); break;
    default:
      type = i18n("Unknown Button"); 
  }
    
  labels << i18n("Button type: ")
  << type
  << i18n("Has State?")
  << InfoPanel::convertTf(butdev->hasState());
    
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// Media Player

SolMediaPlayerDevice::SolMediaPlayerDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::PortableMediaPlayer;
}

SolMediaPlayerDevice::SolMediaPlayerDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::PortableMediaPlayer;

  setDeviceIcon(KIcon("multimedia-player"));
  setDeviceText(i18n("Multimedia Players"));
  setDefaultListing(type);
}

void SolMediaPlayerDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolMediaPlayerDevice>(this,QString(),type);
}

QVListLayout *SolMediaPlayerDevice::infoPanelLayout() 
{
  QStringList labels;
  const Solid::PortableMediaPlayer *mpdev = interface<const Solid::PortableMediaPlayer>(); 
  
  if(!mpdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  labels << i18n("Supported Drivers: ")
  << mpdev->supportedDrivers()
  << i18n("Supported Protocols: ")
  << mpdev->supportedProtocols();

  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// Camera

SolCameraDevice::SolCameraDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::Camera;
}

SolCameraDevice::SolCameraDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::Camera;

  setDeviceIcon(KIcon("camera-web"));
  setDeviceText(i18n("Cameras"));
  setDefaultListing(type);
}

void SolCameraDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolCameraDevice>(this,QString(),type);
}

QVListLayout *SolCameraDevice::infoPanelLayout() 
{
  QStringList labels;
  const Solid::Camera *camdev = interface<const Solid::Camera>(); 

  if(!camdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  labels << i18n("Supported Drivers: ")
  << camdev->supportedDrivers()
  << i18n("Supported Protocols: ")
  << camdev->supportedProtocols();
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}
  
// Battery

SolBatteryDevice::SolBatteryDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::Battery;
}

SolBatteryDevice::SolBatteryDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::Battery;

  setDeviceIcon(KIcon("battery"));
  setDeviceText(i18n("Batteries"));
  setDefaultListing(type);
}

void SolBatteryDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolBatteryDevice>(this,QString(),type);
}

QVListLayout *SolBatteryDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::Battery *batdev = interface<const Solid::Battery>(); 

  if(!batdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QString type;
  switch(batdev->type()) 
  {
      case Solid::Battery::PdaBattery:
	type = i18n("PDA"); break;
      case Solid::Battery::UpsBattery:
	type = i18n("UPS"); break;
      case Solid::Battery::MouseBattery:
	type = i18n("Mouse"); break;
      case Solid::Battery::PrimaryBattery:
	type = i18n("Primary"); break;
      case Solid::Battery::KeyboardBattery:
	type = i18n("Keyboard"); break;
      case Solid::Battery::KeyboardMouseBattery:
	type = i18n("Keyboard + Mouse"); break;
      case Solid::Battery::CameraBattery:
	type = i18n("Camera"); break;
      default:
	type = i18nc("unknown battery type", "Unknown");
  }
  
  QString state;
    switch(batdev->type())
    {
      case Solid::Battery::Charging:
	state = i18n("Charging"); break;
      case Solid::Battery::Discharging:
	state = i18n("Discharging"); break;
      default:
	state = i18n("No Charge"); 
    }
  
  labels << i18n("Battery Type: ")
  << type
  << i18n("Charge Status: ")
  << state;
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}
  
// Ac Adapter

SolAcAdapterDevice::SolAcAdapterDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::AcAdapter;
}

SolAcAdapterDevice::SolAcAdapterDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::AcAdapter;

  setDeviceIcon(KIcon("kde"));
  setDeviceText(i18n("AC Adapters"));
  setDefaultListing(type);
}

void SolAcAdapterDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolAcAdapterDevice>(this,QString(),type);
}

QVListLayout *SolAcAdapterDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::AcAdapter *acdev = interface<const Solid::AcAdapter>(); 
  
  if(!acdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  labels << i18n("Is plugged in?")
  << InfoPanel::convertTf(acdev->isPlugged());
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// DVB

SolDvbDevice::SolDvbDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::DvbInterface;
}

SolDvbDevice::SolDvbDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::DvbInterface;

  setDeviceIcon(KIcon("kde"));
  setDeviceText(i18n("Digital Video Broadcasting Devices"));
  setDefaultListing(type);
}

void SolDvbDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolDvbDevice>(this,QString(),type);
}

QVListLayout *SolDvbDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::DvbInterface *dvbdev = interface<const Solid::DvbInterface>(); 
  
  if(!dvbdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QString type;
  switch(dvbdev->deviceType()) 
  {
    case Solid::DvbInterface::DvbAudio:
      type = i18n("Audio"); break;
    case Solid::DvbInterface::DvbCa:
      type = i18n("Conditional access system"); break;
    case Solid::DvbInterface::DvbDemux:
      type = i18n("Demux"); break;
    case Solid::DvbInterface::DvbDvr:
      type = i18n("Digital video recorder"); break;
    case Solid::DvbInterface::DvbFrontend:
      type = i18n("Front end"); break;
    case Solid::DvbInterface::DvbNet:
      type = i18n("Network"); break;
    case Solid::DvbInterface::DvbOsd:
      type = i18n("On-Screen display"); break;
    case Solid::DvbInterface::DvbSec:
      type = i18n("Security and content protection"); break;
    case Solid::DvbInterface::DvbVideo:
      type = i18n("Video"); break;
    case Solid::DvbInterface::DvbUnknown:
    default:
      type = i18nc("unknown device type", "Unknown"); 
  }
  
  labels << i18n("Device Type: ")
  << type;
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// Serial

SolSerialDevice::SolSerialDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::SerialInterface;
}

SolSerialDevice::SolSerialDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::SerialInterface;
  
  setDeviceIcon(KIcon("printer"));
  setDeviceText(i18n("Serial Devices"));
  setDefaultListing(type);
}

void SolSerialDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolSerialDevice>(this,QString(),type);
}

QVListLayout *SolSerialDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::SerialInterface *serdev = interface<const Solid::SerialInterface>(); 
  
  if(!serdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QString type;
  switch(serdev->serialType())
  {
    case Solid::SerialInterface::Platform:
      type = i18nc("platform serial interface type", "Platform"); break;
    case Solid::SerialInterface::Usb:
      type = i18n("USB"); break;
    default:
      type = i18nc("unknown serial interface type", "Unknown"); 
  }
  
  QString port = i18nc("unknown port", "Unknown");
  if(serdev->port() != -1) port = QString::number(serdev->port());

  labels << i18n("Serial Type: ")
  << type
  << i18n("Port: ")
  << port;
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

//  Smartcard

SolSmartCardDevice::SolSmartCardDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::SmartCardReader;
}

SolSmartCardDevice::SolSmartCardDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::SmartCardReader;

  setDeviceIcon(KIcon("kde"));
  setDeviceText(i18n("Smart Card Devices"));
  setDefaultListing(type);
}

void SolSmartCardDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolSmartCardDevice>(this,QString(),type);
}

QVListLayout *SolSmartCardDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::SmartCardReader *scdev = interface<const Solid::SmartCardReader>(); 
  
  if(!scdev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  QString type;
  switch(scdev->readerType())
  {
    case Solid::SmartCardReader::CardReader:
      type = i18n("Card Reader"); break;
    case Solid::SmartCardReader::CryptoToken:
      type = i18n("Crypto Token"); break;
    default:
      type = i18nc("unknown smart card type", "Unknown"); 
  }
 
  labels << i18n("Smart Card Type: ")
  << type;
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}

// Video

SolVideoDevice::SolVideoDevice(QTreeWidgetItem *parent, const Solid::Device &device) :
  SolDevice(parent, device) 
{
  deviceTypeHolder = Solid::DeviceInterface::Video;
}

SolVideoDevice::SolVideoDevice(const Solid::DeviceInterface::Type &type) :
  SolDevice(type)
{ 
  deviceTypeHolder = Solid::DeviceInterface::Video;
  
  setDeviceIcon(KIcon("video-display"));
  setDeviceText(i18n("Video Devices"));
  setDefaultListing(type);
}

void SolVideoDevice::setDefaultListing(const Solid::DeviceInterface::Type &type) 
{ 
  createDeviceChildren<SolVideoDevice>(this,QString(),type);
}

QVListLayout *SolVideoDevice::infoPanelLayout() 
{  
  QStringList labels;
  const Solid::Video *viddev = interface<const Solid::Video>(); 
  
  if(!viddev) return NULL;
  deviceInfoLayout = new QVListLayout();
  
  labels << i18n("Supported Drivers: ")
  << viddev->supportedDrivers()
  << i18n("Supported Protocols: ")
  << viddev->supportedProtocols();
  
  deviceInfoLayout->applyQListToLayout(labels);
  return deviceInfoLayout;
}
