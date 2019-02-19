/*
  Copyright (C) 2014-2019 Harald Sitter <sitter@kde.org>

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
    if (value_.at(0) == QLatin1Char('"') && value_.at(value_.size()-1) == QLatin1Char('"')) {
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

OSRelease::OSRelease(const QString &filePath)
{
    // Set default values for non-optional fields.
    name = QStringLiteral("Linux");
    id = QStringLiteral("linux");
    prettyName = QStringLiteral("Linux");

    if (filePath.isEmpty()) {
        return;
    }

    QHash<QString, QString *> stringHash = {
        { QStringLiteral("NAME"), &name },
        { QStringLiteral("VERSION"), &version },
        { QStringLiteral("ID"), &id },
        // idLike is not a QString, special handling below!
        { QStringLiteral("VERSION_CODENAME"), &versionCodename },
        { QStringLiteral("VERSION_ID"), &versionId },
        { QStringLiteral("PRETTY_NAME"), &prettyName },
        { QStringLiteral("ANSI_COLOR"), &ansiColor },
        { QStringLiteral("CPE_NAME"), &cpeName },
        { QStringLiteral("HOME_URL"), &homeUrl },
        { QStringLiteral("DOCUMENTATION_URL"), &documentationUrl },
        { QStringLiteral("SUPPORT_URL"), &supportUrl },
        { QStringLiteral("BUG_REPORT_URL"), &bugReportUrl },
        { QStringLiteral("PRIVACY_POLICY_URL"), &privacyPolicyUrl },
        { QStringLiteral("BUILD_ID"), &buildId },
        { QStringLiteral("VARIANT"), &variant },
        { QStringLiteral("VARIANT_ID"), &variantId },
        { QStringLiteral("LOGO"), &logo }
    };

    QFile file(filePath);
    // NOTE: The os-release specification defines default values for specific
    //       fields which means that even if we can not read the os-release file
    //       we have sort of expected default values to use.
    // TODO: it might still be handy to indicate to the outside whether
    //       fallback values are being used or not.
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    QStringList comps;
    while (!file.atEnd()) {
        line = QString::fromLatin1(file.readLine());

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

        if (QString *var = stringHash.value(key, nullptr)) {
            setVar(var, value);
        }

        // ID_LIKE is a list and parsed as such (rather than a QString).
        if (key == QLatin1String("ID_LIKE")) {
            setVar(&idLike, value);
        }

        // os-release explicitly allows for vendor specific aditions. We have no
        // interest in those right now.
    }
}

QString OSRelease::defaultFilePath()
{
    if (QFile::exists(QStringLiteral("/etc/os-release"))) {
        return QStringLiteral("/etc/os-release");
    } else if (QFile::exists(QStringLiteral("/usr/lib/os-release"))) {
        return QStringLiteral("/usr/lib/os-release");
    } else {
        return QString();
    }
}
