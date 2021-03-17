/*
 *  devicelisting.cpp
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

#include "devicelisting.h"

//Solid
#include <solid/devicenotifier.h>

#include <QContextMenuEvent>
#include <QMenu>

//Local
#include "infopanel.h"
#include "soldevicetypes.h"
#include "devinfo.h"
#include "solidhelper.h"
//#include "nicsignals.h"

DeviceListing::DeviceListing(QWidget *parent, InfoPanel *info, DevInfoPlugin *stat)
    : QTreeWidget(parent)
    , iPanel(info)
    , status(stat)
{
//     // Check nic changes
//    nicSig = new NicSignals();
//    connect(nicSig,SIGNAL(nicActivatedOrDisconnected()),this,SLOT(networkingChangedSlot()));
//
    // Check if selection changed
    connect(this, &DeviceListing::currentItemChanged, this, &DeviceListing::currentItemChangedSlot);

    // Check if item is added
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this,
            &DeviceListing::deviceAddedSlot);

    // Check if item is removed
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this,
            &DeviceListing::deviceRemovedSlot);

    setWhatsThis(i18nc("Device Listing Whats This",
                       "Shows all the devices that are currently listed."));

    createMenuActions();
    setHeaderLabels(QStringList(i18n("Devices")));
    populateListing();
    setSortingEnabled(true);
}

DeviceListing::~DeviceListing()
{
    //delete nicSig;
    clear();
}

void DeviceListing::createMenuActions()
{
    colAct = new QAction(i18n("Collapse All"), this);
    connect(colAct, &QAction::triggered, this, &DeviceListing::collapseAllDevicesSlot);

    expAct = new QAction(i18n("Expand All"), this);
    connect(expAct, &QAction::triggered, this, &DeviceListing::expandAllDevicesSlot);

    allAct = new QAction(i18n("Show All Devices"), this);
    connect(allAct, &QAction::triggered, this, &DeviceListing::showAllDevicesSlot);

    relAct = new QAction(i18n("Show Relevant Devices"), this);
    connect(relAct, &QAction::triggered, this, &DeviceListing::showRelevantDevicesSlot);
}

void DeviceListing::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    menu.addAction(colAct);
    menu.addAction(expAct);
    menu.addAction(allAct);
    menu.addAction(relAct);
    menu.exec(event->globalPos());
}

QTreeWidgetItem *DeviceListing::createListItems(const Solid::DeviceInterface::Type &type)
{
    switch (type) {
    case Solid::DeviceInterface::Processor:
        return new SolProcessorDevice(type);
    case Solid::DeviceInterface::StorageDrive:
        return new SolStorageDevice(type);
    case Solid::DeviceInterface::Camera:
        return new SolCameraDevice(type);
    case Solid::DeviceInterface::PortableMediaPlayer:
        return new SolMediaPlayerDevice(type);
    case Solid::DeviceInterface::Battery:
        return new SolBatteryDevice(type);
    default:
        return new SolDevice(type, i18nc("unknown device type", "Unknown"));
    }
}

void DeviceListing::populateListing(const show showStatus)
{
    const Solid::DeviceInterface::Type needHardware[] = {
        Solid::DeviceInterface::Processor,
        Solid::DeviceInterface::StorageDrive,
        Solid::DeviceInterface::Battery,
        Solid::DeviceInterface::PortableMediaPlayer,
        Solid::DeviceInterface::Camera,
    };

    clear();

    for (unsigned int i = 0; i < (sizeof(needHardware)/sizeof(Solid::DeviceInterface::Type)); i++) {
        QTreeWidgetItem *tmpDevice = createListItems(needHardware[i]);
        deviceMap[needHardware[i]] = static_cast<SolDevice *>(tmpDevice);

        if ((tmpDevice->childCount() > 0) || (showStatus == ALL)) {
            addTopLevelItem(tmpDevice);
        }
    }
}

void DeviceListing::currentItemChangedSlot(QTreeWidgetItem *listItemIn, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    SolDevice *listItem = static_cast<SolDevice *>(listItemIn);
    if (listItem && listItem->isDeviceSet()) {
        iPanel->setTopInfo(listItem->deviceIcon(), listItem->device());

        QVListLayout *bottomLay = listItem->infoPanelLayout();
        if (!bottomLay) {
            return;
        }

        iPanel->setBottomInfo(bottomLay);
    } else {
        status->updateStatus(i18nc("no device UDI", "None"));
    }
}

void DeviceListing::deviceAddedSlot(const QString &udi)
{
    SolidHelper *solhelp = new SolidHelper();

    Solid::Device dev(udi);
    if (!dev.isValid()) {
        // Probably the device already disappeared again.
        return;
    }

    Solid::DeviceInterface::Type deviceType = solhelp->deviceType(&dev);
    QTreeWidgetItem *parent = getTreeWidgetItemFromUdi(this, dev.parentUdi());

    // Incase of bad index
    if (deviceMap[deviceType] == nullptr) {
        QTreeWidgetItem *topItem = topLevelItem(0);
        if (!topItem) {
            delete solhelp;
            return;
        }
        deviceMap[deviceType] = static_cast<SolDevice *>(topItem);
    }

    switch (deviceType) {
    case Solid::DeviceInterface::Processor:
        new SolProcessorDevice(deviceMap[deviceType], dev);
        break;
    case Solid::DeviceInterface::Camera:
        new SolCameraDevice(deviceMap[deviceType], dev);
        break;
    case Solid::DeviceInterface::PortableMediaPlayer:
        new SolMediaPlayerDevice(deviceMap[deviceType], dev);
        break;
    case Solid::DeviceInterface::Battery:
        new SolBatteryDevice(deviceMap[deviceType], dev);
        break;
    case Solid::DeviceInterface::StorageDrive:
        new SolStorageDevice(deviceMap[deviceType], dev, SolStorageDevice::NOCHILDREN);
        break;
    case Solid::DeviceInterface::StorageVolume:
        if (parent == nullptr) {
            break;
        }
        new SolVolumeDevice(parent, dev);
        break;
    default:
        break;
    }
    delete solhelp;
}

void DeviceListing::deviceRemovedSlot(const QString &udi)
{
    const QTreeWidgetItem *item = getTreeWidgetItemFromUdi(this, udi);
    if (item == nullptr) {
        return;
    }

    delete item;
}

void DeviceListing::collapseAllDevicesSlot()
{
    collapseAll();
}

void DeviceListing::expandAllDevicesSlot()
{
    expandAll();
}

void DeviceListing::showAllDevicesSlot()
{
    populateListing(ALL);
}

void DeviceListing::showRelevantDevicesSlot()
{
    populateListing(RELEVANT);
}
