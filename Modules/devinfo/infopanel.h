
/*
 *  infopanel.h
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

#ifndef INFOPANEL
#define INFOPANEL

// QT
#include <QWidget>
#include <QGroupBox>
#include <QLabel>

//Solid
#include <solid/device.h>
#include <solid/processor.h>
#include <solid/deviceinterface.h>
#include <solid/networkinterface.h>
#include <solid/storagedrive.h>

//KDE
#include <kicon.h>
#include <kdebug.h>
#include <klocale.h>

//Local
#include "qvlistlayout.h"
#include "devinfo.h"

class DevInfoPlugin;

class InfoPanel : public QGroupBox
{
  Q_OBJECT

  public:
    InfoPanel(QWidget *, DevInfoPlugin *);
    ~InfoPanel();
    
    void setTopInfo(const QIcon &, Solid::Device *);
    void setBottomInfo(QVListLayout *lay);
    static QString friendlyString(const QString &, const QString & = i18nc("name of something is not known", "Unknown"));
    static QString convertTf(const bool &);
    
  private:
    void setTopWidgetLayout(const bool & = false);
    void setBottomWidgetLayout(QVListLayout *, const bool & = false); 
    void setInfoPanelLayout();
    void removeItems(QWidget *);
    void setDefaultText();
    
    QLabel *setDevicesIcon(const QIcon &);
    QVBoxLayout *setAlignedLayout(QWidget *parent, const int & = 0);
    
    QWidget *top;
    QWidget *bottom;
    QVBoxLayout *vLayout;
    Solid::Device *dev;
    DevInfoPlugin *status;
};

#endif //INFOPANEL
