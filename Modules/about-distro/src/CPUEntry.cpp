/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "CPUEntry.h"

#include <QMap>

#include <solid/device.h>
#include <solid/processor.h>

CPUEntry::CPUEntry()
    : Entry(KLocalizedString(), QString())
{
    const QList<Solid::Device> list = Solid::Device::listFromType(Solid::DeviceInterface::Processor);

    label = ki18np("Processor:", "Processors:").subs(list.count());

    // Format processor string
    // Group by processor name
    QMap<QString, int> processorMap;
    for (const Solid::Device &device : list) {
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

    value = names.join(QLatin1String(", "));
}
