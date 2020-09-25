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


#include <KQuickAddons/ConfigModule>
#include "distroinfo.h"
#include "softwareinfo.h"
#include "hardwareinfo.h"

#ifndef INFO_H
#define INFO_H

class KCMAbout : public KQuickAddons::ConfigModule {
    Q_OBJECT

    Q_PROPERTY(DistroInfo* distroInfo READ distroInfo NOTIFY distroInfoChanged)
    Q_PROPERTY(SoftwareInfo* softwareInfo READ softwareInfo NOTIFY softwareInfoChanged)
    Q_PROPERTY(HardwareInfo* hardwareInfo READ hardwareInfo NOTIFY hardwareInfoChanged)
    DistroInfo* distroInfo() const;
    SoftwareInfo* softwareInfo() const;
    HardwareInfo* hardwareInfo() const;

public:
    KCMAbout(QObject* parent, const QVariantList& args);

    Q_INVOKABLE void copyToClipboard(const QString& text) const;

Q_SIGNALS:
    void distroInfoChanged();
    void softwareInfoChanged();
    void hardwareInfoChanged();

private:
    DistroInfo* m_distroInfo;
    SoftwareInfo* m_softwareInfo;
    HardwareInfo* m_hardwareInfo;
};

#endif // INFO_H
