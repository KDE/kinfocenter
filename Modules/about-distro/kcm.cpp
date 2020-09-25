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

#include "kcm.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <QClipboard>
#include <QGuiApplication>


K_PLUGIN_CLASS_WITH_JSON(KCMAbout, "kcm_about_distro.json")

KCMAbout::KCMAbout(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args),
    m_distroInfo(new DistroInfo),
    m_softwareInfo(new SoftwareInfo),
    m_hardwareInfo(new HardwareInfo)
{
    KAboutData* about = new KAboutData("kcm_about_distro", i18n("Info"),
                                       "1.0", QString(), KAboutLicense::LGPL);
    about->addAuthor(i18n("Jonah Brüchert"), QString(), "jbb@kaidan.im");
    setAboutData(about);
    setButtons(Apply | Default);

    qDebug() << "Info module loaded.";

}

void KCMAbout::copyToClipboard(const QString& text) const
{
    QGuiApplication::clipboard()->setText(text);
}

DistroInfo* KCMAbout::distroInfo() const
{
    return m_distroInfo;
}

SoftwareInfo* KCMAbout::softwareInfo() const
{
    return m_softwareInfo;
}

HardwareInfo* KCMAbout::hardwareInfo() const
{
    return m_hardwareInfo;
}

#include "kcm.moc"