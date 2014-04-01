
/*
 *  soldevice.h
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

#ifndef SOLDEVICE
#define SOLDEVICE

//QT
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QDebug>

//Solid
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/processor.h>
#include <solid/storagedrive.h>
#include <solid/storagevolume.h>
#include <solid/storageaccess.h>
#include <solid/audiointerface.h>
#include <solid/portablemediaplayer.h>
#include <solid/camera.h>
#include <solid/battery.h>
#include <solid/serialinterface.h>
#include <solid/video.h>
#include <solid/smartcardreader.h>
#include <solid/predicate.h>

//KDE
#include <KLocalizedString>

// Local
#include "qvlistlayout.h"

class QVListLayout;

class SolDevice : public QTreeWidgetItem
{
  
  public:
    SolDevice(const Solid::DeviceInterface::Type &);
    SolDevice(const Solid::DeviceInterface::Type &, const QString &);
    SolDevice(QTreeWidgetItem *);
    SolDevice(QTreeWidgetItem *, const Solid::Device &);
    
    QIcon deviceIcon() const;
    Solid::Device *device();
    Solid::DeviceInterface::Type deviceType() const;
    
    template <class IFace> const IFace *interface() 
    {
      if(deviceSet)
      {
	IFace *dev = tiedDevice.as<const IFace>();
	if(!dev)
	{
	  qDebug() << "Device unable to be cast to correct device";
	}
	return dev;
      } else {
	return NULL;
      }
    }
    
    template <class IFace> const IFace *interface(const Solid::Device &device) 
    {
	IFace *dev = device.as<const IFace>();
	if(!dev) {
	  qDebug() << "Device unable to be cast to correct device";
	}
	return dev;
    }
    
    template <class IFace> void createDeviceChildren(
      QTreeWidgetItem *treeParent, const QString &parentUid, const Solid::DeviceInterface::Type &type) 
    {  
      const QList<Solid::Device> list = Solid::Device::listFromType(type, parentUid);

      foreach(const Solid::Device &dev, list)
      {
	new IFace(treeParent,dev);
      }
    }   

    void setDeviceIcon(const QIcon &);
    void setDeviceToolTip(const QString &);

    virtual QVListLayout *infoPanelLayout();
    virtual void addItem(Solid::Device dev) { new SolDevice(this,dev); };
    virtual void refreshName() { setDefaultDeviceText(); };
   
    QString udi() const;
    bool isDeviceSet();
    
  protected:
    
    void setDeviceText(const QString &);
    
    virtual void setDefaultDeviceToolTip();
    virtual void setDefaultDeviceText(); 
    virtual void setDefaultDeviceIcon();
    virtual void setDefaultListing(const Solid::DeviceInterface::Type &); 
    
    bool deviceSet;
    QVListLayout *deviceInfoLayout;
    Solid::DeviceInterface::Type deviceTypeHolder;
    Solid::Device tiedDevice;
};

#endif //SOLDEVICE
