/*
  Copyright (C) 2014 Harald Sitter <apachelogger@ubuntu.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OSRELEASE_H
#define OSRELEASE_H

#include <QString>
#include <QStringList>

class OSRelease
{
public:
    OSRelease();

    QString name;
    QString version;
    QString id;
    QStringList idLike;
    QString versionId;
    QString prettyName;
    QString ansiColor;
    QString cpeName;
    // TODO: url struct or map?
    QString homeUrl;
    QString supportUrl;
    QString bugReportUrl;
    QString buildId;
};

#endif // OSRELEASE_H
