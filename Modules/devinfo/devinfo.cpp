
/*
 *  devinfo.cpp
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

#include "devinfo.h"
#include <QSplitter>

K_PLUGIN_FACTORY(devInfoModuleFactory, registerPlugin<DevInfoPlugin>();)
K_EXPORT_PLUGIN(devInfoModuleFactory("kcmdevinfo"))

DevInfoPlugin::DevInfoPlugin(QWidget *parent, const QVariantList &)  : 
  KCModule(devInfoModuleFactory::componentData(), parent)
{
  
  const KAboutData *about =
  new KAboutData(I18N_NOOP("kcmdevinfo"), 0, ki18n("KDE Solid Based Device Viewer"),
                  "0.70", KLocalizedString(), KAboutData::License_GPL,
                  ki18n("(c) 2010 David Hubner"));
		  
  setAboutData(about);
  
  //Layout
  layout = new QGridLayout(this);
  
  //top 
  QSplitter *split = new QSplitter(Qt::Horizontal, this);
  split->setChildrenCollapsible(false);

  InfoPanel *info = new InfoPanel(split, this);
  DeviceListing *devList = new DeviceListing(split, info, this);
  
  split->setStretchFactor(1,1);
 
  //bottom
  QWidget *bottom = new QWidget(this);  
  bottom->setMaximumHeight(30);
  QHBoxLayout *bottomLayout = new QHBoxLayout(bottom);

  QFont boldFont;
  boldFont.setBold(true);
  
  QLabel *udiLabel = new QLabel(i18n("UDI: "));
  udiLabel->setFont(boldFont);
  udiLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
   
  udiStatus = new QLabel(this);
  udiStatus->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  udiStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
  udiStatus->setWhatsThis(i18nc("Udi Whats This","Shows the current device's UDI (Unique Device Identifier)"));

  //Adding
  split->addWidget(devList);  
  split->addWidget(info);
  layout->addWidget(split,0,0);
  
  bottomLayout->addWidget(udiLabel);
  bottomLayout->addWidget(udiStatus);
  layout->addWidget(bottom,1,0,1,0);
  
  //Misc
  setButtons(Help);
  updateStatus(i18n("None"));
}

DevInfoPlugin::~DevInfoPlugin() 
{  
  delete layout; 
}
  
void DevInfoPlugin::updateStatus(const QString message) 
{  
  udiStatus->setText(message);
}
