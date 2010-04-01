
/*
 *  kcmcontainer.cpp
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

#include "kcmcontainer.h"

KcmContainer::KcmContainer(QWidget *parent) : QScrollArea(parent), m_mod(NULL)
{  
  setWidgetResizable( true );
  setFrameStyle( QFrame::NoFrame );
  
  setContainerLayout();
}

KcmContainer::~KcmContainer() 
{  
  if(m_mod!=NULL) delete m_mod;
}

void KcmContainer::setContainerLayout() 
{  
  m_centerWidget = new QWidget(this);
  setWidget(m_centerWidget); 
  
  QVBoxLayout *centerWidgetLayout = new QVBoxLayout(m_centerWidget);
   
  QFont bFont;
  bFont.setBold(true);
   
  m_titleLabel = new QLabel();
  m_titleLabel->setFont(bFont);
  m_titleLabel->setFixedHeight(15);
   
  centerWidgetLayout->addWidget(m_titleLabel);  
}

void KcmContainer::setKcm(const KCModuleInfo &info)
{    
  delete m_centerWidget;
  setContainerLayout();    
  
  setKcmTitle(info);
  
  m_mod = new KCModuleProxy(info);
  m_modInfo = info;
   
  m_centerWidget->layout()->addWidget(m_mod);
}

void KcmContainer::setKcmTitle(const KCModuleInfo &info) 
{
  const QString kcmTitle = info.moduleName();
  const QString kcmComment = info.comment();
  
  m_titleLabel->setText(kcmTitle + "  (" + kcmComment + ")");
}

const QString KcmContainer::helpPath() const 
{  
  if(m_mod == NULL) return QString();  
  return m_modInfo.docPath();
}

const KCModule::Buttons KcmContainer::buttons() const
{
  return m_mod->buttons();
}

const KAboutData *KcmContainer::kcmAboutData() const 
{
  return m_mod->aboutData();
}

const QString KcmContainer::exportText()
{
  return "";
  //return m_mod->realModule()->exportText(); 
}
  