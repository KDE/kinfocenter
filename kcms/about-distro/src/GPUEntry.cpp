/*
    SPDX-FileCopyrightText: 2016 Rohan Garg <rohan@kde.org>
    SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "GPUEntry.h"

#include <KLocalizedString>

#include "FancyString.h"

using namespace std::chrono_literals;
using namespace Qt::StringLiterals;

GPUEntry::GPUEntry(std::optional<int> deviceIndex, const Device &device)
    : Entry(deviceIndex.has_value() ? ki18nc("@label %1 is the GPU index number", "Graphics Processor %1:").subs(QString::number(deviceIndex.value()))
                                    : ki18n("Graphics Processor:"),
            FancyString::fromRenderer(device.name))
    , m_device(device)
{
}
