
/*  nicsignals.cpp
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

#include "nicsignals.h"

NicSignals::NicSignals()
{
  connectToNicSignals();
}

void NicSignals::connectToNicSignals() 
{  
  const QList<Solid::Control::NetworkInterface *> nicList = Solid::Control::NetworkManager::networkInterfaces();
  
  foreach(const Solid::Control::NetworkInterface *nic, nicList) {
    connect(nic,SIGNAL(connectionStateChanged(int,int,int)),this,SLOT(nicChangedSignal(int,int,int)));
  }
}

void NicSignals::nicChangedSignal(const int &ns,const int &os,const int &reason) 
{ 
  Q_UNUSED(os); Q_UNUSED(reason);
  if(ns==8 || ns==2) emit nicActivatedOrDisconnected();
}
