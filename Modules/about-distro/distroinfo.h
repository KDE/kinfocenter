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
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QStringList idLike READ idLike CONSTANT)
    Q_PROPERTY(QString versionCodename READ versionCodename CONSTANT)
    Q_PROPERTY(QString versionId READ versionId CONSTANT)
    Q_PROPERTY(QString prettyName READ prettyName CONSTANT)
    Q_PROPERTY(QString ansiColor READ ansiColor CONSTANT)
    Q_PROPERTY(QString cpeName READ cpeName CONSTANT)
    Q_PROPERTY(QString homeUrl READ homeUrl CONSTANT)
    Q_PROPERTY(QString documentationUrl READ documentationUrl CONSTANT)
    Q_PROPERTY(QString supportUrl READ supportUrl CONSTANT)
    Q_PROPERTY(QString bugReportUrl READ bugReportUrl CONSTANT)
    Q_PROPERTY(QString privacyPolicyUrl READ privacyPolicyUrl CONSTANT)
    Q_PROPERTY(QString buildId READ buildId CONSTANT)
    Q_PROPERTY(QString variant READ variant CONSTANT)
    Q_PROPERTY(QString variantId READ variantId CONSTANT)
    Q_PROPERTY(QString logo READ logo CONSTANT)

public:
    DistroInfo(QObject *parent = nullptr);

    PROPERTY(QString, name)
    PROPERTY(QString, version)
    PROPERTY(QString, id)
    PROPERTY(QStringList, idLike)
    PROPERTY(QString, versionCodename)
    PROPERTY(QString, versionId)
    PROPERTY(QString, prettyName)
    PROPERTY(QString, ansiColor)
    PROPERTY(QString, cpeName)
    PROPERTY(QString, homeUrl)
    PROPERTY(QString, documentationUrl)
    PROPERTY(QString, supportUrl)
    PROPERTY(QString, bugReportUrl)
    PROPERTY(QString, privacyPolicyUrl)
    PROPERTY(QString, buildId)
    PROPERTY(QString, variant)
    PROPERTY(QString, variantId)
    PROPERTY(QString, logo)

private:
    KOSRelease m_osrelease;

};

#endif // DISTROINFO_H
