
/*
 *  nicsignals.h
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

#ifndef __NICSIGNALS__
#define __NICSIGNALS__

//Qt
#include <QObject>

//KDE
#include <kdebug.h>
#include <solid/control/networkmanager.h>
#include <solid/control/networkinterface.h>

class NicSignals : public QObject
{
  Q_OBJECT
  
  public:
    NicSignals();
    
  private:
    void connectToNicSignals();

  signals:
    void nicActivatedOrDisconnected();
    
  public slots:
    void nicChangedSignal(const int &,const int &,const int &);
    
};

#endif // __NICSIGNALS __
