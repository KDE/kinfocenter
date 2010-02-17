/*
 *  solidproc.h
 *
 *  Copyright (C) 2008 Ivo Anjo <knuckles@gmail.com>
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

#ifndef KCONTROL_SOLIDPROC_H
#define KCONTROL_SOLIDPROC_H

#include <kcmodule.h>

#include <solid/device.h>

#include "ui_solidproc.h"


class KCMSolidProc: public KCModule, public Ui::Solidproc
{
    Q_OBJECT

    public:
        explicit KCMSolidProc(QWidget *parent = 0, const QVariantList &list = QVariantList() );

    protected slots:
        virtual void rowSelected(int newRow);

    protected:
        QList<Solid::Device> solidProcList;
};

#endif
