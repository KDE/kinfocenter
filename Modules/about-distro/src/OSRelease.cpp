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

#include "OSRelease.h"

#include <QDebug>
#include <QFile>

#include <KLocalizedString>
#include <KShell>

static void setVar(QString *var, const QString &value) 
{
    // Values may contain quotation marks, strip them as we have no use for them.
    KShell::Errors error;
    QStringList args = KShell::splitArgs(value, KShell::NoOptions, &error);
    if (error != KShell::NoError) { // Failed to parse.
        return;
    }
    *var = args.join(QLatin1Char(' '));
}

static void setVar(QStringList *var, const QString &value)
{
    // Instead of passing the verbatim value we manually strip any initial quotes
    // and then run it through KShell. At this point KShell will actually split
    // by spaces giving us the final QStringList.
    // NOTE: Splitting like this does not actually allow escaped substrings to
    //       be handled correctly, so "kitteh \"french fries\"" would result in
    //       three list entries. I'd argue that if someone makes an id like that
    //       they are at fault for the bogus parsing here though as id explicitly
    //       is required to not contain spaces even if more advanced shell escaping
    //       is also allowed...
    QString value_ = value;
    if (value_.at(0) == QChar('"') && value_.at(value_.size()-1) == QChar('"')) {
        value_.remove(0, 1);
        value_.remove(-1, 1);
    }
    KShell::Errors error;
    QStringList args = KShell::splitArgs(value_, KShell::NoOptions, &error);
    if (error != KShell::NoError) { // Failed to parse.
        return;
    }
    *var = args;
}

OSRelease::OSRelease()
{
    // Set default values for non-optional fields.
    name = QStringLiteral("Linux");
    id = QStringLiteral("linux");
    prettyName = QStringLiteral("Linux");

    QString fileName;

    if (QFile::exists(QStringLiteral("/etc/os-release"))) {
        fileName = QStringLiteral("/etc/os-release");
    } else if (QFile::exists(QStringLiteral("/usr/lib/os-release"))) {
        fileName = QStringLiteral("/usr/lib/os-release");
    } else {
        return;
    }


    QFile file(fileName);
    // NOTE: The os-release specification defines default values for specific
    //       fields which means that even if we can not read the os-release file
    //       we have sort of expected default values to use.
    // TODO: it might still be handy to indicate to the outside whether
    //       fallback values are being used or not.
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    QStringList comps;
    while (!file.atEnd()) {
        line = file.readLine();

        if (line.startsWith(QLatin1Char('#'))) {
            // Comment line
            continue;
        }

        comps = line.split(QLatin1Char('='));

        if (comps.size() != 2) {
            // Invalid line.
            continue;
        }

        QString key = comps.at(0);
        QString value = comps.at(1).trimmed();
        if (key == QLatin1String("NAME"))
            setVar(&name, value);
        else if (key == QLatin1String("VERSION"))
            setVar(&version, value);
        else if (key == QLatin1String("ID"))
            setVar(&id, value);
        else if (key == QLatin1String("ID_LIKE"))
            setVar(&idLike, value);
        else if (key == QLatin1String("VERSION_ID"))
            setVar(&versionId, value);
        else if (key == QLatin1String("PRETTY_NAME"))
            setVar(&prettyName, value);
        else if (key == QLatin1String("ANSI_COLOR"))
            setVar(&ansiColor, value);
        else if (key == QLatin1String("CPE_NAME"))
            setVar(&cpeName, value);
        else if (key == QLatin1String("HOME_URL"))
            setVar(&homeUrl, value);
        else if (key == QLatin1String("SUPPORT_URL"))
            setVar(&supportUrl, value);
        else if (key == QLatin1String("BUG_REPORT_URL"))
            setVar(&bugReportUrl, value);
        else if (key == QLatin1String("BUILD_ID"))
            setVar(&buildId, value);
        // os-release explicitly allows for vendor specific aditions. We have no
        // interest in those right now.
    }
}
