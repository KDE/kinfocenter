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


#include "softwareinfo.h"
#include <sys/utsname.h>

#include <KCoreAddons>
#include <QDebug>
#include <QStandardPaths>
#include <KDesktopFile>
#include <KConfigGroup>
#include <KLocalizedString>

SoftwareInfo::SoftwareInfo(QObject *parent)
    : QObject(parent)
{
}

QString SoftwareInfo::kernelRelease() const
{
    struct utsname utsName{};
    uname(&utsName);

    return QString::fromLatin1(utsName.release);
}

QString SoftwareInfo::frameworksVersion() const
{
    return KCoreAddons::versionString();
}

QString SoftwareInfo::qtVersion() const
{
    return QString::fromLatin1(qVersion());
}

QString SoftwareInfo::plasmaVersion() const
{
    const QStringList &filePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                             QStringLiteral("xsessions/plasma.desktop"));

    if (filePaths.length() < 1) {
        return QString();
    }

    // Despite the fact that there can be multiple desktop files we simply take
    // the first one as users usually don't have xsessions/ in their $HOME
    // data location, so the first match should (usually) be the only one and
    // reflect the plasma session run.
    KDesktopFile desktopFile(filePaths.first());
    return desktopFile.desktopGroup().readEntry("X-KDE-PluginInfo-Version", QString());
}

QString SoftwareInfo::osType() const
{
    const int bits = QT_POINTER_SIZE == 8 ? 64 : 32;

    return QString::number(bits);
}
