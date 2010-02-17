/*
 *  Copyright (C) 2008 Nicolas Ternisien <nicolas.ternisien@gmail.com>
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

#include "kcm_partition.h"

#include <QStringList>

#include <KPluginFactory>
#include <KPluginLoader>

#include <kaboutdata.h>
#include <kdialog.h>
#include <kdebug.h>

#include <QLayout>
#include <QPainter>

#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>

#include <klocale.h>
#include <kglobal.h>
#include <kseparator.h>
#include <kdiskfreespace.h>

#include <solid/device.h>
#include <solid/deviceinterface.h>

#include "usedSizeWidget.h"

K_PLUGIN_FACTORY(KCMPartitionFactory,
		registerPlugin<KCMPartition>();
)
K_EXPORT_PLUGIN(KCMPartitionFactory("kcm_partition"))

KCMPartition::KCMPartition(QWidget *parent, const QVariantList &) :
	KCModule(KCMPartitionFactory::componentData(), parent) {

	KAboutData *about = new KAboutData(I18N_NOOP("kcm_partition"), 0,
			ki18n("KDE Partitions Information Control Module"),
			0, KLocalizedString(), KAboutData::License_GPL,
			ki18n( "(c) 2008 Nicolas Ternisien"));

	about->addAuthor(ki18n("Nicolas Ternisien"), KLocalizedString(), "nicolas.ternisien@gmail.com");
	setAboutData(about);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	tree = new QTreeWidget(this);
	layout->addWidget(tree);
	tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	tree->setAllColumnsShowFocus(true);
	tree->setRootIsDecorated(true);
	tree->setAlternatingRowColors(true);
	tree->setSortingEnabled(true);
	tree->setWhatsThis(i18n("This list displays partitions of your system.") );

	QStringList headers;
	headers << i18n("Mount Point") << i18n("Label") << i18n("Type") << i18n("Total Size") << i18n("Free Size") << i18n("Used Size");
	tree->setHeaderLabels(headers);

}

KCMPartition::~KCMPartition() {

}

void KCMPartition::load() {
	kDebug() << "Loading partition information..." << endl;
	
	QMap<QString, QTreeWidgetItem*> rootDevicesItem;


	QList<Solid::Device> devices = Solid::Device::listFromType(Solid::DeviceInterface::StorageDrive, QString());

	//Root Devices (Hard Disks, CD-Rom drives,...)
	foreach(const Solid::Device &device, devices) {
		if(device.is<Solid::StorageDrive>() == false) {
			continue;
		}
		
		const Solid::StorageDrive* drive = device.as<Solid::StorageDrive>();
		
		QStringList itemContent;
		
		itemContent << i18nc("Device (Vendor)", "%1 (%2)", device.product(), device.vendor()) << findDriveType(drive) << findBusType(drive);
		
		QTreeWidgetItem* item = new QTreeWidgetItem(tree, itemContent);
		item->setIcon(0, KIcon(device.icon()));
		item->setExpanded(true);
		
		rootDevicesItem.insert(device.udi(), item);
	}

	//Storage volumes
	QList<Solid::Device> storageVolumes = Solid::Device::listFromType(Solid::DeviceInterface::StorageVolume, QString());
	QList<QString> accessPaths;

	foreach(const Solid::Device &device, storageVolumes) {
		
		if(device.is<Solid::StorageVolume>() == false) {
			continue;
		}
		
		const Solid::StorageVolume* volume = device.as<Solid::StorageVolume>();

		QStringList itemContent;
		
		if (device.is<Solid::StorageAccess>()) {
			const Solid::StorageAccess* access = device.as<Solid::StorageAccess>();
			itemContent << access->filePath();
			
			accessPaths.append(access->filePath());
		}
		else {
			itemContent << i18nc("no storage volume", "none");
		}
		
		itemContent << volume->label() << volume->fsType() << KGlobal::locale()->formatByteSize(volume->size());
		
		QTreeWidgetItem* item;
		if (rootDevicesItem.contains(device.parentUdi())) {
			QTreeWidgetItem* root = rootDevicesItem.value(device.parentUdi());
			
			item = new QTreeWidgetItem(root, itemContent);
		}
		else {
			item = new QTreeWidgetItem(tree, itemContent);
		}

		item->setIcon(0, KIcon(device.icon()));
		item->setTextAlignment(TOTAL_SIZE_INDEX, Qt::AlignRight);
		item->setTextAlignment(FREE_SIZE_INDEX, Qt::AlignRight);
	
		UsedSizeWidget* usedSizeWidget = new UsedSizeWidget(tree);
		usedSizeWidget->setObjectName(device.udi());
		tree->setItemWidget(item, USED_SIZE_INDEX, usedSizeWidget);
		
		/*
		UsedSizeWidget* otherUsedSizeWidget = (UsedSizeWidget*) tree->itemWidget(item, USED_SIZE_INDEX);
		if (otherUsedSizeWidget==NULL) {
			kError() << "BUG BUG BUG !!! Just added used size widget of " << itemContent << " is null !!!" << endl;
		}
		*/

	}
	
	resizeContent();
	
	tree->sortItems(0, Qt::AscendingOrder);
	
	//Do this later to be sure all UsedSizeWidget will be ready
	foreach(const QString &accessPath, accessPaths) {
		connect(KDiskFreeSpace::findUsageInfo(accessPath), SIGNAL(foundMountPoint (const QString&, quint64, quint64, quint64)), this, SLOT(mountPointInfo(const QString&, quint64, quint64, quint64)) );
	}

}

QString KCMPartition::findBusType(const Solid::StorageDrive* drive) const {
	switch (drive->bus()) {
	case Solid::StorageDrive::Ide:
		return i18n("IDE");
	case Solid::StorageDrive::Usb:
		return i18n("USB");
	case Solid::StorageDrive::Ieee1394:
		return i18n("IEEE 1394");
	case Solid::StorageDrive::Scsi:
		return i18n("SCSI");
	case Solid::StorageDrive::Sata:
		return i18n("S-ATA");
	case Solid::StorageDrive::Platform:
		return i18n("Platform");
	}

	return i18n("Unknown Bus");
}

QString KCMPartition::findDriveType(const Solid::StorageDrive* drive) const {
	switch (drive->driveType()) {
	case Solid::StorageDrive::HardDisk:
		return i18n("Hard Disk");
	case Solid::StorageDrive::CdromDrive:
		return i18n("CD/DVD Drive");
	case Solid::StorageDrive::Floppy:
		return i18n("Floppy Drive");
	case Solid::StorageDrive::Tape:
		return i18n("Tape");
	case Solid::StorageDrive::CompactFlash:
		return i18n("Compact Flash");
	case Solid::StorageDrive::MemoryStick:
		return i18n("Memory Stick");
	case Solid::StorageDrive::SmartMedia:
		return i18n("Smart Media");
	case Solid::StorageDrive::SdMmc:
		return i18n("SD/MMC");
	case Solid::StorageDrive::Xd:
		return i18n("Xd");
	}

	return i18n("Unknown Type");
}

QString KCMPartition::quickHelp() const {
	return i18n("This display shows information about partitions and hard disks of your system.");
}

void KCMPartition::mountPointInfo(const QString& mountPoint, quint64 kibSize, quint64 kibUsed, quint64 kibAvail) {
	
	QTreeWidgetItemIterator it(tree, QTreeWidgetItemIterator::All);
	while ( *it != NULL ) {
		QTreeWidgetItem* currentItem = *it;

		if (currentItem->text(0) == mountPoint) {
			currentItem->setText(TOTAL_SIZE_INDEX, KGlobal::locale()->formatByteSize(kibSize*1000));
			currentItem->setText(FREE_SIZE_INDEX, KGlobal::locale()->formatByteSize(kibAvail*1000));
			
			UsedSizeWidget* usedSizeWidget = (UsedSizeWidget*) tree->itemWidget(currentItem, USED_SIZE_INDEX);
		
			if (usedSizeWidget!=NULL) {
				usedSizeWidget->setUsedSize(mountPoint, kibSize*1000, kibUsed*1000, kibAvail*1000);
			}
			/*
			else {
				kError() << "BUG BUG BUG AGAIN !!! Used Size Widget null for " << mountPoint << endl;
			}
			*/
			
			resizeContent();
			
			break;
		}
		
		++it;
	}

	resizeContent();
	

}

void KCMPartition::resizeContent() const {
	for(int i=0; i<tree->columnCount(); ++i) {
		tree->resizeColumnToContents(i);
	}
}

#include "kcm_partition.moc"
