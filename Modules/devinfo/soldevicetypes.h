
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

#ifndef SOLDEVICETYPES
#define SOLDEVICETYPES

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


class SolVolumeDevice : public SolDevice 
{

  public:
    SolVolumeDevice(const Solid::DeviceInterface::Type &);
    SolVolumeDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
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

class SolVideoDevice : public SolDevice 
{

  public:
    SolVideoDevice(const Solid::DeviceInterface::Type &);
    SolVideoDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout();
    
  private:
    void setDefaultListing(const Solid::DeviceInterface::Type &);
};


#endif //SOLDEVICETYPES
