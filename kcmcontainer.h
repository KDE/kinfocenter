
/*
 *  kcmcontainer.h
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

#ifndef __KCMCONTAINER__
#define __KCMCONTAINER__

//KDE
#include <KCModuleInfo>
#include <KCModuleProxy>
#include <KCModule>
#include <KAboutData>
#include <KIcon>

//QT
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

class KcmContainer : public QScrollArea
{  
  public:
    KcmContainer(QWidget *);
    ~KcmContainer();
    
    void setKcm(const KCModuleInfo &);
    const QString helpPath() const;
    const KCModule::Buttons buttons() const;
    const KAboutData *kcmAboutData() const;
    const QString exportText();
    QString currentModulesName();
    
  private:
    void setContainerLayout();
    void setKcmTitle(const KCModuleInfo &);
    
    QLabel *m_titleLabel;
    QWidget *m_centerWidget;
    KCModuleProxy *m_mod;
    KCModuleInfo m_modInfo;
};

#endif //__KCMCONTAINER__