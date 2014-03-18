
/*
 *  infosum.cpp
 *
 *  Copyright (C) 2010 David Hubner <hubnerd@ntlworld.com>
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

#include "infosum.h"
#include <KFormat>
#include <KLocalizedString>
#include <plasma/version.h>

K_PLUGIN_FACTORY(devInfoModuleFactory, registerPlugin<InfoSumPlugin>();)
K_EXPORT_PLUGIN(devInfoModuleFactory("kcm_infosummary"))

InfoSumPlugin::InfoSumPlugin(QWidget *parent, const QVariantList &)  : 
  KCModule(parent)
{
  
  const KAboutData *about =
  new KAboutData(i18n("kcm_infosummary"), QString(), i18n("Hardware Summary Information"),
                  QStringLiteral("0.10"), QString(), KAboutData::License_GPL,
                  i18n("(c) 2010 David Hubner"));
		  
  setAboutData(about);
  createDisplay();
}

InfoSumPlugin::~InfoSumPlugin() 
{  
}
  
void InfoSumPlugin::createDisplay()
{  
  m_layout = new QVBoxLayout(this);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setSpacing(10);
  
  createOsBox();
  createCpuBox();
  //createMemBox(); // Awaiting Mem class from kdereview 
  createHdBox();
  m_layout->addStretch();
}

void InfoSumPlugin::createOsBox()
{  
  DefaultBoxWidget *osWidget = new DefaultBoxWidget();
  osWidget->setIcon(QIcon::fromTheme(QStringLiteral("kde")));
  osWidget->setLabelTitles(i18n("OS Version"),i18n("KDE SC Version"), i18n("Hostname"));
  
  OsDepInfo *osInfo = new OsDepInfo();
  
  osWidget->setLabelOne(osInfo->osVersion());
  osWidget->setLabelTwo(QString(PLASMA_VERSION_STRING));
  osWidget->setLabelThree(osInfo->hostName());
  osWidget->setWhatsThis(i18nc("OS whats this","This shows information about your Operating System"));
  
  m_layout->addWidget(osWidget);
  
  delete osInfo;
}

void InfoSumPlugin::createCpuBox() 
{
  const QList<Solid::Device> list = Solid::Device::listFromType(Solid::DeviceInterface::Processor);
 
  foreach(const Solid::Device &dev, list)
  {
    const Solid::Processor *prodev = dev.as<const Solid::Processor>();
    if(!prodev) return;
    
    DefaultBoxWidget *cpuWidget = new DefaultBoxWidget();
    cpuWidget->setLabelTitles(i18n("Processor"), i18n("Processor Number"), i18n("Processor Max Speed"));
    cpuWidget->setIcon(QIcon::fromTheme(QStringLiteral("cpu")));
    
    cpuWidget->setLabelOne(dev.product());
    cpuWidget->setLabelTwo(QString::number(prodev->number()));
    cpuWidget->setLabelThree(QString::number(prodev->maxSpeed()));   
    cpuWidget->setWhatsThis(i18nc("CPU whats this","This shows information about a specific CPU in your computer"));
    
    m_layout->addWidget(cpuWidget);
  }
}
 
void InfoSumPlugin::createMemBox()
{
  ProgressBoxWidget *memWidget = new ProgressBoxWidget();
  
  memWidget->setIcon(QIcon::fromTheme(QStringLiteral("memory")));
  memWidget->setLabelTitles(i18n("Memory Amount"),i18n("Memory Used/Free"));
  m_layout->addWidget(memWidget);
}

void InfoSumPlugin::createHdBox() 
{
  const Solid::Predicate stoPred =
    Solid::Predicate(Solid::DeviceInterface::StorageDrive,"driveType", "HardDisk",Solid::Predicate::Equals);
  const QList<Solid::Device> list = Solid::Device::listFromQuery(stoPred, QString());
  
  foreach(const Solid::Device &dev, list)
  {
    const Solid::StorageDrive *stodev = dev.as<const Solid::StorageDrive>();
    if(!stodev) return;
   
    DefaultBoxWidget *stoWidget = new DefaultBoxWidget();
    stoWidget->setIcon(QIcon::fromTheme(QStringLiteral("drive-harddisk")));
    
    stoWidget->setLabelTitles(i18n("Drive Title"),i18n("Storage Size"),i18n("Bus"));
    stoWidget->setLabelOne(dev.product());
    stoWidget->setLabelTwo(KFormat().formatByteSize(stodev->size()));
    
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
	bus = i18n("Platform"); break;
      default:
	bus = i18nc("Unknown device","Unknown"); 
    }
    
    stoWidget->setLabelThree(bus);
    stoWidget->setWhatsThis(i18nc("Hard Drive Whats This","This shows information about a specific hard drive in your computer"));
    
    m_layout->addWidget(stoWidget);
  }
}

#include "infosum.moc"
