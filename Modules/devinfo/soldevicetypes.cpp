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

#include <QProgressBar>

#include <kcapacitybar.h>

#include <KFormat>
// ---- Processor

SolProcessorDevice::SolProcessorDevice(QTreeWidgetItem *parent, const Solid::Device &device)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::Processor;
    setDefaultDeviceText();
}

SolProcessorDevice::SolProcessorDevice(const Solid::DeviceInterface::Type &type)
    : SolDevice(type)
{
    deviceTypeHolder = Solid::DeviceInterface::Processor;

    setDeviceIcon(QIcon::fromTheme(QStringLiteral("cpu")));
    setDeviceText(i18n("Processors"));
    setDefaultListing(type);
}

void SolProcessorDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolProcessorDevice>(this, QString(), type);
}

void SolProcessorDevice::setDefaultDeviceText()
{
    const Solid::Processor *prodev = interface<const Solid::Processor>();

    if (!prodev) {
        return;
    }
    setText(0, i18n("Processor %1", QString::number(prodev->number())));
}

QVListLayout *SolProcessorDevice::infoPanelLayout()
{
    QStringList labels;
    const Solid::Processor *prodev = interface<const Solid::Processor>();

    if (!prodev) {
        return nullptr;
    }
    deviceInfoLayout = new QVListLayout();

    QStringList extensions;
    const Solid::Processor::InstructionSets insSets = prodev->instructionSets();

    if (insSets & Solid::Processor::IntelMmx) {
        extensions << i18n("Intel MMX");
    }
    if (insSets & Solid::Processor::IntelSse) {
        extensions << i18n("Intel SSE");
    }
    if (insSets & Solid::Processor::IntelSse2) {
        extensions << i18n("Intel SSE2");
    }
    if (insSets & Solid::Processor::IntelSse3) {
        extensions << i18n("Intel SSE3");
    }
    if (insSets & Solid::Processor::IntelSse4) {
        extensions << i18n("Intel SSE4");
    }
    if (insSets & Solid::Processor::Amd3DNow) {
        extensions << i18n("AMD 3DNow");
    }
    if (insSets & Solid::Processor::AltiVec) {
        extensions << i18n("ATI IVEC");
    }
    if (extensions.isEmpty()) {
        extensions << i18nc("no instruction set extensions", "None");
    }

    labels << i18n("Processor Number: ")
           << InfoPanel::friendlyString(QString::number(prodev->number()))
           << i18n("Max Speed: ")
           << InfoPanel::friendlyString(QString::number(prodev->maxSpeed()))
           << i18n("Supported Instruction Sets: ")
           << extensions.join(QStringLiteral("\n"));

    deviceInfoLayout->applyQListToLayout(labels);
    return deviceInfoLayout;
}

// ---- Storage

SolStorageDevice::SolStorageDevice(QTreeWidgetItem *parent, const Solid::Device &device,
                                   const storageChildren &c)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::StorageDrive;
    setDefaultDeviceText();

    if (c == CREATECHILDREN) {
        createDeviceChildren<SolVolumeDevice>(this,
                                              device.udi(), Solid::DeviceInterface::StorageVolume);
    }
}

SolStorageDevice::SolStorageDevice(const Solid::DeviceInterface::Type &type)
    : SolDevice(type)
{
    deviceTypeHolder = Solid::DeviceInterface::StorageDrive;

    setDeviceIcon(QIcon::fromTheme(QStringLiteral("drive-harddisk")));
    setDeviceText(i18n("Storage Drives"));
    setDefaultListing(type);
}

void SolStorageDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolStorageDevice>(this, QString(), type);
}

void SolStorageDevice::setDefaultDeviceText()
{
    const Solid::StorageDrive *stodev = interface<const Solid::StorageDrive>();
    if (!stodev) {
        return;
    }

    QString storageType;
    switch (stodev->driveType()) {
    case Solid::StorageDrive::HardDisk:
        storageType = i18n("Hard Disk Drive");
        break;
    case Solid::StorageDrive::CompactFlash:
        storageType = i18n("Compact Flash Reader");
        break;
    case Solid::StorageDrive::SmartMedia:
        storageType = i18n("Smart Media Reader");
        break;
    case Solid::StorageDrive::SdMmc:
        storageType = i18n("SD/MMC Reader");
        break;
    case Solid::StorageDrive::CdromDrive:
        storageType = i18n("Optical Drive");
        break;
    case Solid::StorageDrive::MemoryStick:
        storageType = i18n("Memory Stick Reader");
        break;
    case Solid::StorageDrive::Xd:
        storageType = i18n("xD Reader");
        break;
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

    if (!stodev) {
        return nullptr;
    }
    deviceInfoLayout = new QVListLayout();

    QString bus;
    switch (stodev->bus()) {
    case Solid::StorageDrive::Ide:
        bus = i18n("IDE");
        break;
    case Solid::StorageDrive::Usb:
        bus = i18n("USB");
        break;
    case Solid::StorageDrive::Ieee1394:
        bus = i18n("IEEE1394");
        break;
    case Solid::StorageDrive::Scsi:
        bus = i18n("SCSI");
        break;
    case Solid::StorageDrive::Sata:
        bus = i18n("SATA");
        break;
    case Solid::StorageDrive::Platform:
        bus = i18nc("platform storage bus", "Platform");
        break;
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

// ---- Volume

SolVolumeDevice::SolVolumeDevice(QTreeWidgetItem *parent, const Solid::Device &device)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::StorageVolume;
}

SolVolumeDevice::SolVolumeDevice(const Solid::DeviceInterface::Type &type)
    : SolDevice(type)
{
    deviceTypeHolder = Solid::DeviceInterface::StorageVolume;

    setDefaultListing(type);
}

void SolVolumeDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolVolumeDevice>(this, QString(), type);
}

QVListLayout *SolVolumeDevice::infoPanelLayout()
{
    QStringList labels;
    KCapacityBar *usageBar = nullptr;

    const Solid::StorageVolume *voldev = interface<const Solid::StorageVolume>();
    const Solid::StorageAccess *accdev = interface<const Solid::StorageAccess>();

    if (!voldev) {
        return nullptr;
    }
    deviceInfoLayout = new QVListLayout();

    QString usage;
    switch (voldev->usage()) {
    case Solid::StorageVolume::Unused:
        usage = i18n("Unused");
        break;
    case Solid::StorageVolume::FileSystem:
        usage = i18n("File System");
        break;
    case Solid::StorageVolume::PartitionTable:
        usage = i18n("Partition Table");
        break;
    case Solid::StorageVolume::Raid:
        usage = i18n("Raid");
        break;
    case Solid::StorageVolume::Encrypted:
        usage = i18n("Encrypted");
        break;
    default:
        usage = i18nc("unknown volume usage", "Unknown");
    }

    labels << i18n("File System Type: ")
           << InfoPanel::friendlyString(voldev->fsType())
           << i18n("Label: ")
           << InfoPanel::friendlyString(voldev->label(), i18n("Not Set"))
           << i18n("Volume Usage: ")
           << usage
           << i18n("UUID: ")
           << InfoPanel::friendlyString(voldev->uuid());

    if (accdev) {
        labels << QStringLiteral("--")
               << i18n("Mounted At: ")
               << InfoPanel::friendlyString(accdev->filePath(), i18n("Not Mounted"));

        if (!accdev->filePath().isEmpty()) {
            KDiskFreeSpaceInfo mountSpaceInfo
                = KDiskFreeSpaceInfo::freeSpaceInfo(accdev->filePath());

            labels << i18n("Volume Space:");

            usageBar = new KCapacityBar();
            if (mountSpaceInfo.size() > 0) {
                usageBar->setValue(static_cast<int>((mountSpaceInfo.used() * 100)
                                                    / mountSpaceInfo.size()));
                usageBar->setText(
                    i18nc("Available space out of total partition size (percent used)",
                          "%1 free of %2 (%3% used)",
                          KFormat().formatByteSize(mountSpaceInfo.available()),
                          KFormat().formatByteSize(mountSpaceInfo.size()),
                          usageBar->value()));
            } else {
                usageBar->setValue(0);
                usageBar->setText(i18n("No data available"));
            }
        }
    }

    deviceInfoLayout->applyQListToLayout(labels);
    if (usageBar) {
        deviceInfoLayout->addWidget(usageBar);
    }

    return deviceInfoLayout;
}

// Media Player

SolMediaPlayerDevice::SolMediaPlayerDevice(QTreeWidgetItem *parent, const Solid::Device &device)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::PortableMediaPlayer;
}

SolMediaPlayerDevice::SolMediaPlayerDevice(const Solid::DeviceInterface::Type &type)
    : SolDevice(type)
{
    deviceTypeHolder = Solid::DeviceInterface::PortableMediaPlayer;

    setDeviceIcon(QIcon::fromTheme(QStringLiteral("multimedia-player")));
    setDeviceText(i18n("Multimedia Players"));
    setDefaultListing(type);
}

void SolMediaPlayerDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolMediaPlayerDevice>(this, QString(), type);
}

QVListLayout *SolMediaPlayerDevice::infoPanelLayout()
{
    QStringList labels;
    const Solid::PortableMediaPlayer *mpdev = interface<const Solid::PortableMediaPlayer>();

    if (!mpdev) {
        return nullptr;
    }
    deviceInfoLayout = new QVListLayout();

    labels << i18n("Supported Drivers: ")
           << mpdev->supportedDrivers()
           << i18n("Supported Protocols: ")
           << mpdev->supportedProtocols();

    deviceInfoLayout->applyQListToLayout(labels);
    return deviceInfoLayout;
}

// Camera

SolCameraDevice::SolCameraDevice(QTreeWidgetItem *parent, const Solid::Device &device)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::Camera;
}

SolCameraDevice::SolCameraDevice(const Solid::DeviceInterface::Type &type)
    : SolDevice(type)
{
    deviceTypeHolder = Solid::DeviceInterface::Camera;

    setDeviceIcon(QIcon::fromTheme(QStringLiteral("camera-web")));
    setDeviceText(i18n("Cameras"));
    setDefaultListing(type);
}

void SolCameraDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolCameraDevice>(this, QString(), type);
}

QVListLayout *SolCameraDevice::infoPanelLayout()
{
    QStringList labels;
    const Solid::Camera *camdev = interface<const Solid::Camera>();

    if (!camdev) {
        return nullptr;
    }
    deviceInfoLayout = new QVListLayout();

    labels << i18n("Supported Drivers: ")
           << camdev->supportedDrivers()
           << i18n("Supported Protocols: ")
           << camdev->supportedProtocols();

    deviceInfoLayout->applyQListToLayout(labels);
    return deviceInfoLayout;
}

// Battery

SolBatteryDevice::SolBatteryDevice(QTreeWidgetItem *parent, const Solid::Device &device)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::Battery;
}

SolBatteryDevice::SolBatteryDevice(const Solid::DeviceInterface::Type &type)
    : SolDevice(type)
{
    deviceTypeHolder = Solid::DeviceInterface::Battery;

    setDeviceIcon(QIcon::fromTheme(QStringLiteral("battery")));
    setDeviceText(i18n("Batteries"));
    setDefaultListing(type);
}

void SolBatteryDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolBatteryDevice>(this, QString(), type);
}

QVListLayout *SolBatteryDevice::infoPanelLayout()
{
    QStringList labels;
    const Solid::Battery *batdev = interface<const Solid::Battery>();

    if (!batdev) {
        return nullptr;
    }
    deviceInfoLayout = new QVListLayout();

    QString type;
    switch (batdev->type()) {
    case Solid::Battery::PdaBattery:
        type = i18n("PDA");
        break;
    case Solid::Battery::UpsBattery:
        type = i18n("UPS");
        break;
    case Solid::Battery::PrimaryBattery:
        type = i18n("Primary");
        break;
    case Solid::Battery::MouseBattery:
        type = i18n("Mouse");
        break;
    case Solid::Battery::KeyboardBattery:
        type = i18n("Keyboard");
        break;
    case Solid::Battery::KeyboardMouseBattery:
        type = i18n("Keyboard + Mouse");
        break;
    case Solid::Battery::CameraBattery:
        type = i18n("Camera");
        break;
    case Solid::Battery::PhoneBattery:
        type = i18n("Phone");
        break;
    case Solid::Battery::MonitorBattery:
        type = i18nc("Screen", "Monitor");
        break;
    case Solid::Battery::GamingInputBattery:
        type = i18nc("Wireless game pad or joystick battery", "Gaming Input");
        break;
    default:
        type = i18nc("unknown battery type", "Unknown");
    }

    QString state;
    switch (batdev->chargeState()) {
    case Solid::Battery::Charging:
        state = i18n("Charging");
        break;
    case Solid::Battery::Discharging:
        state = i18n("Discharging");
        break;
    case Solid::Battery::FullyCharged:
        state = i18n("Fully Charged");
        break;
    default:
        state = i18n("No Charge");
    }

    labels << i18n("Battery Type: ")
           << type
           << i18n("Charge Status: ")
           << state
           << i18n("Charge Percent: ");

    deviceInfoLayout->applyQListToLayout(labels);

    QProgressBar *chargePercent = new QProgressBar();
    chargePercent->setMaximum(100);
    chargePercent->setValue(batdev->chargePercent());
    chargePercent->setEnabled(batdev->isPresent());

    deviceInfoLayout->addWidget(chargePercent);

    return deviceInfoLayout;
}
