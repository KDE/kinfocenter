/*
 * Copyright 2019  Jonah Brüchert <jbb@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QObject>

#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

class HardwareInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString processors READ processors CONSTANT)
    QString processors() const;

    Q_PROPERTY(QString processorCount READ processorCount CONSTANT)
    int processorCount() const;

    Q_PROPERTY(QString memory READ memory CONSTANT)
    QString memory() const;

    Q_PROPERTY(QString gpu READ gpu CONSTANT)
    QString gpu() const;

public:
    HardwareInfo();
};

#endif // HARDWAREINFO_H
