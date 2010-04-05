
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

#include "nicview.h"

K_PLUGIN_FACTORY(devInfoModuleFactory, registerPlugin<NicViewPlugin>();)
K_EXPORT_PLUGIN(devInfoModuleFactory("kcm_nicview"))

NicViewPlugin::NicViewPlugin(QWidget *parent, const QVariantList &)  : 
  KCModule(devInfoModuleFactory::componentData(), parent)
{
  
  const KAboutData *about =
  new KAboutData(I18N_NOOP("kcm_nicview"), 0, ki18n("Network Interface Information"),
                  "0.70", KLocalizedString(), KAboutData::License_GPL,
                  ki18n("(c) 2010 David Hubner"));
		  
  setAboutData(about);

  setButtons(NoAdditionalButton);
}

NicViewPlugin::~NicViewPlugin() 
{  
}
  
