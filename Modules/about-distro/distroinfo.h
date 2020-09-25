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

#include <KOSRelease>
#include <QObject>

#define PROPERTY(type, name) \
    type name() const { return m_osrelease.name(); }\

#ifndef DISTROINFO_H
#define DISTROINFO_H

class DistroInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    PROPERTY(QString, name)

    Q_PROPERTY(QString version READ version CONSTANT)
    PROPERTY(QString, version)

    Q_PROPERTY(QString id READ id CONSTANT)
    PROPERTY(QString, id)

    Q_PROPERTY(QStringList idLike READ idLike CONSTANT)
    PROPERTY(QStringList, idLike)

    Q_PROPERTY(QString versionCodename READ versionCodename CONSTANT)
    PROPERTY(QString, versionCodename)

    Q_PROPERTY(QString versionId READ versionId CONSTANT)
    PROPERTY(QString, versionId)

    Q_PROPERTY(QString prettyName READ prettyName CONSTANT)
    PROPERTY(QString, prettyName)

    Q_PROPERTY(QString ansiColor READ ansiColor CONSTANT)
    PROPERTY(QString, ansiColor)

    Q_PROPERTY(QString cpeName READ cpeName CONSTANT)
    PROPERTY(QString, cpeName)

    Q_PROPERTY(QString homeUrl READ homeUrl CONSTANT)
    PROPERTY(QString, homeUrl)

    Q_PROPERTY(QString documentationUrl READ documentationUrl CONSTANT)
    PROPERTY(QString, documentationUrl)

    Q_PROPERTY(QString supportUrl READ supportUrl CONSTANT)
    PROPERTY(QString, supportUrl)

    Q_PROPERTY(QString bugReportUrl READ bugReportUrl CONSTANT)
    PROPERTY(QString, bugReportUrl)

    Q_PROPERTY(QString privacyPolicyUrl READ privacyPolicyUrl CONSTANT)
    PROPERTY(QString, privacyPolicyUrl)

    Q_PROPERTY(QString buildId READ buildId CONSTANT)
    PROPERTY(QString, buildId)

    Q_PROPERTY(QString variant READ variant CONSTANT)
    PROPERTY(QString, variant)

    Q_PROPERTY(QString variantId READ variantId CONSTANT)
    PROPERTY(QString, variantId)

    Q_PROPERTY(QString logo READ logo CONSTANT)
    PROPERTY(QString, logo)

public:
    DistroInfo();

private:
    KOSRelease m_osrelease;

};

#endif // DISTROINFO_H
