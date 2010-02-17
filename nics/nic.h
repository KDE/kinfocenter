/*
 * nic.h
 *
 *  Copyright (C) 2001 Alexander Neundorf <neundorf@kde.org>
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
 */

#ifndef KCONTROL_NIC_H
#define KCONTROL_NIC_H

#include <kcmodule.h>

class QPushButton;
class QTreeWidget;

class KCMNic:public KCModule
{
   Q_OBJECT
   public:
      explicit KCMNic(QWidget *parent=0, const QVariantList &list = QVariantList( ));

   protected Q_SLOTS:
      void update();

   protected:
      QTreeWidget *m_list;
      QPushButton *m_updateButton;
};

#endif

