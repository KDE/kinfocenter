/***************************************************************************
 *   Copyright (C) 2015 Pinak Ahuja <pinak.ahuja@gmail.com>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef KCM_FILEINDEXER_H
#define KCM_FILEINDEXER_H

#include <KQuickAddons/ConfigModule>

class KCMFileIndexer : public KQuickAddons::ConfigModule
{
    Q_OBJECT

public:
    explicit KCMFileIndexer(QObject *parent, const QVariantList &args);
    virtual ~KCMFileIndexer() = default;
};

Q_DECLARE_METATYPE(QList<QPointF>)

#endif // KCM_FILEINDEXER_H
