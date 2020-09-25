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

#ifndef SOFTWAREINFO_H
#define SOFTWAREINFO_H

class SoftwareInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString kernelRelease READ kernelRelease CONSTANT)
    QString kernelRelease() const;
    
    Q_PROPERTY(QString frameworksVersion READ frameworksVersion CONSTANT)
    QString frameworksVersion() const;

    Q_PROPERTY(QString qtVersion READ qtVersion CONSTANT)
    QString qtVersion() const;

    Q_PROPERTY(QString plasmaVersion READ plasmaVersion CONSTANT)
    QString plasmaVersion() const;

    Q_PROPERTY(QString osType READ osType CONSTANT)
    QString osType() const;

public:
    SoftwareInfo();
};

#endif // SOFTWAREINFO_H
