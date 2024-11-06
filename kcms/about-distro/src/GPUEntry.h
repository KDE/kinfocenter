/*
    SPDX-FileCopyrightText: 2016 Rohan Garg <rohan@kde.org>
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef GPUENTRY_H
#define GPUENTRY_H

#include <QVulkanInstance>

#include "Entry.h"

class GPUEntry : public Entry
{
public:
    struct Device {
        QString name;
        VkPhysicalDeviceType type;
    };

    GPUEntry(std::optional<int> deviceIndex, const Device &device);

private:
    Device m_device;
};

#endif // GPUENTRY_H
