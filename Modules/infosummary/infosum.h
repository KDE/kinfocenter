
/*
 *  devinfo.h
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

#ifndef __INFOSUM__
#define __INFOSUM__

//kde
#include <KCModule>
#include <KLocale>
#include <KAboutData>
#include <KDebug>
#include <kdeversion.h>

//Solid
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/processor.h>
#include <solid/predicate.h>
#include <solid/storagedrive.h>
#include <solid/storagevolume.h>
#include <solid/storageaccess.h>

//Plugin
#include <kpluginfactory.h>
#include <kpluginloader.h>

//QT
#include <QGridLayout>
#include <QScrollArea>

//local 
#include "defaultboxwidget.h"
#include "progressboxwidget.h"
#include "osdepinfo.h"


class InfoSumPlugin : public KCModule
{
  Q_OBJECT
  
  public:
    InfoSumPlugin(QWidget *parent, const QVariantList &);
    ~InfoSumPlugin();
    
  private:
    void createDisplay();
    void createOsBox();
    void createCpuBox();
    void createMemBox();
    void createHdBox();
    void appendInfo(QString info);
    void appendBreak();
    
    QVBoxLayout *m_layout;
    QString m_exportInfo;
};

#endif //__INFOSUM__
