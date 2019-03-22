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

// Sets a QString var
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

// Sets a QStringList var (i.e. splits a string value)
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

class Q_DECL_HIDDEN OSRelease::Private
{
public:
    Private(const QString &filePath)
        : name(QStringLiteral("Linux"))
        , id(QStringLiteral("linux"))
        , prettyName(QStringLiteral("Linux"))
    {
        // Default values for non-optional fields set above ^.

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
        QStringList parts;
        while (!file.atEnd()) {
            // Trimmed to handle indented comment lines properly
            line = QString::fromLatin1(file.readLine()).trimmed();

            if (line.startsWith(QLatin1Char('#'))) {
                // Comment line
                continue;
            }

            parts = line.split(QLatin1Char('='));

            if (parts.size() != 2 || line.contains(QChar('#'))) {
                // Invalid line...
                // For the purposes of simple parsing we'll not support >2 =
                // or >1 # characters.
                // The former makes splitting and the latter makes comment
                // stripping difficult.
                continue;
            }

            QString key = parts.at(0);
            QString value = parts.at(1).trimmed();

            if (QString *var = stringHash.value(key, nullptr)) {
                setVar(var, value);
                continue;
            }

            // ID_LIKE is a list and parsed as such (rather than a QString).
            if (key == QLatin1String("ID_LIKE")) {
                setVar(&idLike, value);
                continue;
            }

        // os-release explicitly allows for vendor specific aditions. We have no
        // interest in those right now.
        }
    }

    QString name;
    QString version;
    QString id;
    QStringList idLike;
    QString versionCodename;
    QString versionId;
    QString prettyName;
    QString ansiColor;
    QString cpeName;
    QString homeUrl;
    QString documentationUrl;
    QString supportUrl;
    QString bugReportUrl;
    QString privacyPolicyUrl;
    QString buildId;
    QString variant;
    QString variantId;
    QString logo;
};

OSRelease::OSRelease(const QString &filePath)
    : d(new Private(filePath))
{
}

OSRelease::~OSRelease()
{
    delete d;
}

QString OSRelease::name() const
{
    return d->name;
}

QString OSRelease::version() const
{
    return d->version;
}

QString OSRelease::id() const
{
    return d->id;
}

QStringList OSRelease::idLike() const
{
    return d->idLike;
}

QString OSRelease::versionCodename() const
{
    return d->versionCodename;
}

QString OSRelease::versionId() const
{
    return d->versionId;
}

QString OSRelease::prettyName() const
{
    return d->prettyName;
}

QString OSRelease::ansiColor() const
{
    return d->ansiColor;
}

QString OSRelease::cpeName() const
{
    return d->cpeName;
}

QString OSRelease::homeUrl() const
{
    return d->homeUrl;
}

QString OSRelease::documentationUrl() const
{
    return d->documentationUrl;
}

QString OSRelease::supportUrl() const
{
    return d->supportUrl;
}

QString OSRelease::bugReportUrl() const
{
    return d->bugReportUrl;
}

QString OSRelease::privacyPolicyUrl() const
{
    return d->privacyPolicyUrl;
}

QString OSRelease::buildId() const
{
    return d->buildId;
}

QString OSRelease::variant() const
{
    return d->variant;
}

QString OSRelease::variantId() const
{
    return d->variantId;
}

QString OSRelease::logo() const
{
    return d->logo;
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
