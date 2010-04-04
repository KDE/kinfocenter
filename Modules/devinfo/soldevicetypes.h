
/*
 *  soldevicetypes.h
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

#ifndef __SOLDEVICETYPES__
#define __SOLDEVICETYPES__

//kde 
#include <kdiskfreespaceinfo.h>

#include "soldevice.h"
#include "infopanel.h"

class QVListLayout;

class SolProcessorDevice : public SolDevice
{
  
  public:
    SolProcessorDevice(const Solid::DeviceInterface::Type &);
    SolProcessorDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultDeviceText();
    void setDefaultListing(const Solid::DeviceInterface::Type &); 
};


class SolStorageDevice : public SolDevice
{
  
  public:   
    enum storageChildren { CREATECHILDREN , NOCHILDREN };
    
    SolStorageDevice(const Solid::DeviceInterface::Type &);
    SolStorageDevice(QTreeWidgetItem *, const Solid::Device &, const storageChildren & = CREATECHILDREN);
    QVListLayout *infoPanelLayout();
      
  private:
    void setDefaultDeviceText();
    void setDefaultListing(const Solid::DeviceInterface::Type &); 
};

class SolNetworkDevice : public SolDevice
{
  
  public:
    SolNetworkDevice(const Solid::DeviceInterface::Type &);
    SolNetworkDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    void refreshName();
    
  private:
    void setDefaultDeviceText();
    void setDefaultDeviceIcon();
    void setDefaultListing(const Solid::DeviceInterface::Type &); 
};

class SolVolumeDevice : public SolDevice 
{

  public:
    SolVolumeDevice(const Solid::DeviceInterface::Type &);
    SolVolumeDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolAudioDevice : public SolDevice 
{

  public:
    SolAudioDevice(const Solid::DeviceInterface::Type &);
    SolAudioDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    void addItem(Solid::Device);
    
  private:
    enum SubMenus { ALSA=0, OSS };
    
    void setDefaultListing(const Solid::DeviceInterface::Type &);
    void listOss();
    void listAlsa();
    void createSubItems(const SubMenus &);
    
    SolDevice *alsaSubItem;
    SolDevice *ossSubItem;
};

class SolButtonDevice : public SolDevice 
{

  public:
    SolButtonDevice(const Solid::DeviceInterface::Type &);
    SolButtonDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultDeviceIcon();
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolMediaPlayerDevice : public SolDevice 
{

  public:
    SolMediaPlayerDevice(const Solid::DeviceInterface::Type &);
    SolMediaPlayerDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();

  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolCameraDevice : public SolDevice 
{

  public:
    SolCameraDevice(const Solid::DeviceInterface::Type &);
    SolCameraDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolBatteryDevice : public SolDevice 
{

  public:
    SolBatteryDevice(const Solid::DeviceInterface::Type &);
    SolBatteryDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolAcAdapterDevice : public SolDevice 
{

  public:
    SolAcAdapterDevice(const Solid::DeviceInterface::Type &);
    SolAcAdapterDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolDvbDevice : public SolDevice 
{

  public:
    SolDvbDevice(const Solid::DeviceInterface::Type &);
    SolDvbDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolSerialDevice : public SolDevice 
{

  public:
    SolSerialDevice(const Solid::DeviceInterface::Type &);
    SolSerialDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();

  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolSmartCardDevice : public SolDevice 
{

  public:
    SolSmartCardDevice(const Solid::DeviceInterface::Type &);
    SolSmartCardDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};

class SolVideoDevice : public SolDevice 
{

  public:
    SolVideoDevice(const Solid::DeviceInterface::Type &);
    SolVideoDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};


#endif // __SOLDEVICETYPES__
