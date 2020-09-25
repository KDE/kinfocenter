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


#include "hardwareinfo.h"

#include <KCoreAddons>
#include <KFormat>

#include <Solid/Device>
#include <Solid/Processor>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>

#include <KLocalizedString>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#elif defined(Q_OS_FREEBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

HardwareInfo::HardwareInfo(QObject *parent)
    : QObject(parent)
{
}

int HardwareInfo::processorCount() const {
    return Solid::Device::listFromType(Solid::DeviceInterface::Processor).count();
}

QString HardwareInfo::processors() const {
    const auto list = Solid::Device::listFromType(Solid::DeviceInterface::Processor);

    // Format processor string
    // Group by processor name
    QMap<QString, int> processorMap;
    for (const auto &device : list) {
        const QString name = device.product();
        auto it = processorMap.find(name);
        if (it == processorMap.end()) {
            processorMap.insert(name, 1);
        } else {
            ++it.value();
        }
    }
    // Create a formatted list of grouped processors
    QStringList names;
    names.reserve(processorMap.count());
    for (auto it = processorMap.constBegin(); it != processorMap.constEnd(); ++it) {
        const int count = it.value();
        QString name = it.key();
        name.replace(QStringLiteral("(TM)"), QChar(8482));
        name.replace(QStringLiteral("(R)"), QChar(174));
        name = name.simplified();
        names.append(QStringLiteral("%1 × %2").arg(count).arg(name));
    }

    const QString processorLabel = names.join(QLatin1String(", "));

    return processorLabel;
}

QString HardwareInfo::memory() const
{
    qlonglong totalRam = -1;
#ifdef Q_OS_LINUX
    struct sysinfo info{};
    if (sysinfo(&info) == 0)
        // manpage "sizes are given as multiples of mem_unit bytes"
        totalRam = qlonglong(info.totalram) * info.mem_unit;
#elif defined(Q_OS_FREEBSD)
    /* Stuff for sysctl */
    size_t len;

    unsigned long memory;
    len = sizeof(memory);
    sysctlbyname("hw.physmem", &memory, &len, NULL, 0);

    totalRam = memory;
#endif

    return KFormat().formatByteSize(totalRam);
}

QString HardwareInfo::gpu() const
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    surface.create();
    if (!context.create()) {
        qWarning() << "Failed create QOpenGLContext";
        return QString();
    }

    if (context.makeCurrent(&surface)) {
        QString value = QString::fromLatin1(reinterpret_cast<const char*>(
                                          context.functions()->glGetString(GL_RENDERER)));
        value.replace(QStringLiteral("(TM)"), QChar(8482));
        value.replace(QStringLiteral("(R)"), QChar(174));
        value = value.simplified();
        // It seems the renderer value may have excess information in parentheses ->
        // strip that. Elide would probably be nicer, a bit meh with QWidgets though.
        value = value.mid(0, value.indexOf('('));
        // Leads to trailing space in my case, don't know whether that is happening
        // everyhere, though. Thus removing trailing spaces separately.
        value = value.trimmed();
        context.doneCurrent();
        return value;
    } else {
        qWarning() << "Failed to make QOpenGLContext current";
        return QString();
    }
}
