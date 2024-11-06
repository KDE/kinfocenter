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

Hint GPUEntry::localizedHint(Language language) const
{
    switch (m_device.type) {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return {.m_text = localize(ki18nc("@label GPU type", "integrated"), language), .m_color = Hint::Color::One};
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return {.m_text = localize(ki18nc("@label GPU type", "discrete"), language), .m_color = Hint::Color::Two};
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return {.m_text = localize(ki18nc("@label GPU type. GPU of a virtual machine", "virtual"), language), .m_color = Hint::Color::Three};
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        break;
    }
    return {};
}
