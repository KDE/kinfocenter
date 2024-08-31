// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include <iostream>

#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QVulkanFunctions>
#include <QVulkanInstance>

QString vkDeviceName()
{
    QVulkanInstance instance;
    if (!instance.create()) {
        qWarning() << "Failed to create vulkan instance";
        return {};
    }
    auto functions = instance.functions();

    uint32_t count = 0;
    functions->vkEnumeratePhysicalDevices(instance.vkInstance(), &count, nullptr);
    if (count == 0) {
        qWarning("No vulkan devices");
        return {};
    }

    qDebug() << count;

    QVarLengthArray<VkPhysicalDevice, 4> devices(count);
    VkResult error = functions->vkEnumeratePhysicalDevices(instance.vkInstance(), &count, devices.data());
    if (error != VK_SUCCESS || count == 0) {
        qWarning("Failed to enumerate vulkan devices: %d", error);
        return {};
    }

    for (const auto &device : devices) {
        VkPhysicalDeviceProperties properties;
        functions->vkGetPhysicalDeviceProperties(device, &properties);
        qDebug("Physical device %d: '%s' %d.%d.%d (api %d.%d.%d vendor 0x%X device 0x%X type %d)",
               0,
               properties.deviceName,
               VK_VERSION_MAJOR(properties.driverVersion),
               VK_VERSION_MINOR(properties.driverVersion),
               VK_VERSION_PATCH(properties.driverVersion),
               VK_VERSION_MAJOR(properties.apiVersion),
               VK_VERSION_MINOR(properties.apiVersion),
               VK_VERSION_PATCH(properties.apiVersion),
               properties.vendorID,
               properties.deviceID,
               properties.deviceType);
    }
    // return QString::fromUtf8(properties.deviceName);
    return {};
}

QString glDeviceName()
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    surface.create();
    const auto deferDone = qScopeGuard([&context] {
        context.doneCurrent();
    });
    if (!context.create()) {
        qWarning() << "Failed create QOpenGLContext";
        return {};
    }

    if (!context.makeCurrent(&surface)) {
        qWarning() << "Failed to make QOpenGLContext current";
        return {};
    }

    return QString::fromUtf8(reinterpret_cast<const char *>(context.functions()->glGetString(GL_RENDERER)));
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QString deviceName = vkDeviceName();
    if (deviceName.isEmpty()) {
        deviceName = glDeviceName();
    }

    std::cout << deviceName.toStdString() << "\n";

    return 0;
}
