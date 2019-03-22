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

#ifndef OSRELEASE_H
#define OSRELEASE_H

#include <QString>
#include <QStringList>

/**
 * @brief The OSRelease class parses /etc/os-release files
 *
 * https://www.freedesktop.org/software/systemd/man/os-release.html
 *
 * os-release is a free software standard for describing an operating system.
 * This class parses and models os-release files.
 */
class OSRelease
{
public:
    /**
     * Constructs a new OSRelease instance. Parsing happens in the constructor
     * and the data is not cached across instances.
     *
     * @note For parsing simplicity neither trailing comments nor multiple '='
     *   characters are allowed.
     *
     * @param filePath The path to the os-release file. By default the first
     *   available file of the paths specified in the os-release manpage is
     *   parsed.
     */
    explicit OSRelease(const QString &filePath = defaultFilePath());
    ~OSRelease();

    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#NAME= */
    QString name() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#VERSION= */
    QString version() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#ID= */
    QString id() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#ID_LIKE= */
    QStringList idLike() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#VERSION_CODENAME= */
    QString versionCodename() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#VERSION_ID= */
    QString versionId() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#PRETTY_NAME= */
    QString prettyName() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#ANSI_COLOR= */
    QString ansiColor() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#CPE_NAME= */
    QString cpeName() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#HOME_URL= */
    QString homeUrl() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#HOME_URL= */
    QString documentationUrl() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#HOME_URL= */
    QString supportUrl() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#HOME_URL= */
    QString bugReportUrl() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#HOME_URL= */
    QString privacyPolicyUrl() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#BUILD_ID= */
    QString buildId() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#VARIANT= */
    QString variant() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#VARIANT_ID= */
    QString variantId() const;
    /** @see https://www.freedesktop.org/software/systemd/man/os-release.html#LOGO= */
    QString logo() const;

private:
    static QString defaultFilePath();

    class Private;
    Private *const d = nullptr;
};

#endif // OSRELEASE_H
