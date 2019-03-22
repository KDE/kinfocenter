/*
  Copyright (C) 2019 Harald Sitter <sitter@kde.org>

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

#include <QTest>

#include "OSRelease.h"

class OSReleaseTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase() {}

    void testParse()
    {
        auto r = OSRelease(QFINDTESTDATA("data/os-release"));
        QCOMPARE(r.name(), "Name");
        QCOMPARE(r.version(), "100.5");
        QCOMPARE(r.id(), "theid");
        QCOMPARE(r.idLike(), QStringList({"otherid", "otherotherid"}));
        QCOMPARE(r.versionCodename(), "versioncodename");
        QCOMPARE(r.versionId(), "500.1");
        QCOMPARE(r.prettyName(), "Pretty Name");
        QCOMPARE(r.ansiColor(), "1;34");
        QCOMPARE(r.cpeName(), "cpe:/o:foo:bar:100");
        QCOMPARE(r.homeUrl(), "https://url.home");
        QCOMPARE(r.documentationUrl(), "https://url.docs");
        QCOMPARE(r.supportUrl(), "https://url.support");
        QCOMPARE(r.bugReportUrl(), "https://url.bugs");
        QCOMPARE(r.privacyPolicyUrl(), "https://url.privacy");
        QCOMPARE(r.buildId(), "105.5");
        QCOMPARE(r.variant(), "Test Edition");
        QCOMPARE(r.variantId(), "test");
        QCOMPARE(r.logo(), "start-here-test");
        QCOMPARE(r.extraKeys(), QStringList({"DEBIAN_BTS"}));
        QCOMPARE(r.extraValue("DEBIAN_BTS"), "debbugs://bugs.debian.org/");
    }
};

QTEST_MAIN(OSReleaseTest)

#include "OSReleaseTest.moc"
