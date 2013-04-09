/*
  Copyright (C) 2012 Harald Sitter <apachelogger@ubuntu.com>

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

#include "LSBRelease.h"

#include <QtCore/QFile>

#include <KDebug>
#include <KLocalizedString>

LSBRelease::LSBRelease()
{
    QFile file("/etc/lsb-release");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        kDebug() << "Could not open /etc/lsb-release, aborting LSB";
        QString unkown = i18nc("@label", "Unknown");
        m_id = unkown;
        m_release = unkown;
        m_codename = unkown;
        m_description = unkown;
        return;
    }

    // List of valid column identifiers
    QString line;
    QStringList comps;
    while (!file.atEnd()) {
        line = file.readLine();
        comps = line.split(QChar('='));
#warning unkown set?
        Q_ASSERT(comps.size() == 2);
        QString key = comps.at(0);
        QString value = comps.at(1).trimmed();
        if (key == QLatin1String("DISTRIB_ID"))
            m_id = value;
        else if (key == QLatin1String("DISTRIB_RELEASE"))
            m_release = value;
        else if (key == QLatin1String("DISTRIB_CODENAME"))
            m_codename = value;
        else if (key == QLatin1String("DISTRIB_DESCRIPTION"))
            m_description = value;
        else
            Q_ASSERT(false); // Should not happen!
    }

#warning unkown set?
    if (m_id.isEmpty() || m_release.isEmpty())
        Q_ASSERT(false); // meh.

    // For branding reasons we always talk about Kubuntu
    if (m_id == QLatin1String("Ubuntu"))
        m_id = QLatin1String("Kubuntu");
}
