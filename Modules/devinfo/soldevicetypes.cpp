/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "soldevicetypes.h"

#include <solid/camera.h>
#include <solid/deviceinterface.h>
#include <solid/portablemediaplayer.h>
#include <solid/storageaccess.h>
#include <solid/storagedrive.h>
#include <solid/storagevolume.h>

#include <QProgressBar>
#include <QStorageInfo>

// kde
#include <KCapacityBar>
#include <KFormat>

#include "qvlistlayout.h"

// ---- Storage

SolStorageDevice::SolStorageDevice(QTreeWidgetItem *parent, const Solid::Device &device, const storageChildren &c)
    : SolDevice(parent, device)
{
    deviceTypeHolder = Solid::DeviceInterface::StorageDrive;
    setDefaultDeviceText();

    if (c == CREATECHILDREN) {
        createDeviceChildren<SolVolumeDevice>(this, device.udi(), Solid::DeviceInterface::StorageVolume);
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

    labels << i18n("Bus: ") << bus << i18n("Hotpluggable?") << InfoPanel::convertTf(stodev->isHotpluggable()) << i18n("Removable?")
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

    labels << i18n("File System Type: ") << InfoPanel::friendlyString(voldev->fsType()) << i18n("Label: ")
           << InfoPanel::friendlyString(voldev->label(), i18n("Not Set")) << i18n("Volume Usage: ") << usage << i18n("UUID: ")
           << InfoPanel::friendlyString(voldev->uuid());

    if (accdev) {
        const QString mountPoint = accdev->filePath();

        labels << QStringLiteral("--") << i18n("Mounted At: ") << InfoPanel::friendlyString(mountPoint, i18n("Not Mounted"));

        if (!mountPoint.isEmpty()) {
            QStorageInfo storageInfo(mountPoint);

            labels << i18n("Volume Space:");

            usageBar = new KCapacityBar();
            qint64 size = 0;
            if (storageInfo.isValid() && storageInfo.isReady() && (size = storageInfo.bytesTotal()) > 0) {
                const auto freeSpace = storageInfo.bytesFree();
                const auto usedSpace = size - freeSpace;
                const auto usedPercent = static_cast<int>((usedSpace * 100) / size);
                usageBar->setValue(usedPercent);
                usageBar->setText(i18nc("Available space out of total partition size (percent used)",
                                        "%1 free of %2 (%3% used)",
                                        KFormat().formatByteSize(freeSpace),
                                        KFormat().formatByteSize(size),
                                        usedPercent));
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

    labels << i18n("Supported Drivers: ") << mpdev->supportedDrivers() << i18n("Supported Protocols: ") << mpdev->supportedProtocols();

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

    labels << i18n("Supported Drivers: ") << camdev->supportedDrivers() << i18n("Supported Protocols: ") << camdev->supportedProtocols();

    deviceInfoLayout->applyQListToLayout(labels);
    return deviceInfoLayout;
}