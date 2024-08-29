/*
    SPDX-FileCopyrightText: 2016 Rohan Garg <rohan@kde.org>
    SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "GPUEntry.h"

#include <QDebug>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QProcess>
#include <QStandardPaths>
#include <QVulkanFunctions>
#include <QVulkanInstance>

#include <KLibexec>
#include <KLocalizedString>

#include "FancyString.h"

extern "C" {
#include <xf86drm.h>
}

using namespace std::chrono_literals;
using namespace Qt::StringLiterals;

namespace
{
template<typename Output, typename Input>
constexpr Output narrow(Input i)
{
    Output o = static_cast<Input>(i);
    if (i != Input(o)) {
        std::abort();
    }
    return o;
}

QJsonDocument readFromProcess(const QString &executable, std::optional<int> deviceIndex = std::nullopt)
{
    QProcess process;
    if (deviceIndex.has_value() && deviceIndex.value() > 0) {
        auto processEnvironment = QProcessEnvironment::systemEnvironment();
        processEnvironment.insert("DRI_PRIME", QString::number(deviceIndex.value()));
        process.setProcessEnvironment(processEnvironment);
    }
    process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    process.setProgram(executable);
    process.setArguments({u"--platform"_s, qGuiApp->platformName()});
    process.start();
    process.waitForFinished(std::chrono::milliseconds(2s).count());
    auto output = process.readAllStandardOutput();
    return QJsonDocument::fromJson(output);
}

int deviceCount()
{
    static auto count = [] {
        if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
            return 3; // NOTE: this is intentionally off by one. We don't see llvmpipe in drmGetDevices2!
        }
        return drmGetDevices2(0, nullptr, 0);
    }();
    return count;
}

QStringList searchPaths()
{
    static auto paths = KLibexec::kdeFrameworksPaths(QLatin1String(LIBEXECDIR_FROM_KCM)) << QLatin1String(KDE_INSTALL_FULL_LIBEXECDIR);
    return paths;
}

struct Device {
    QString name;
    VkPhysicalDeviceType type;
};

QList<Device> vulkanDevices()
{
    if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
        return {
            {u"AMD Radeon RX 7900 XTX (RADV NAVI31)]"_s, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU},
            {u"AMD Radeon Graphics (RADV RAPHAEL_MENDOCINO)"_s, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU},
            {u"Intel(R) UHD Graphics (CML GT2)"_s, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU},
            {u"llvmpipe (LLVM 18.1.6, 256 bits)"_s, VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU},
        };
    }

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

    QVarLengthArray<VkPhysicalDevice, 4> devices(count);
    VkResult error = functions->vkEnumeratePhysicalDevices(instance.vkInstance(), &count, devices.data());
    if (error != VK_SUCCESS || count == 0) {
        qWarning("Failed to enumerate vulkan devices: %d", error);
        return {};
    }

    QList<Device> deviceList;
    deviceList.reserve(devices.count());
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

        deviceList.emplace_back(QString::fromUtf8(properties.deviceName), properties.deviceType);
    }
    return deviceList;
}

std::optional<QStringList> gpusFromJsonArray(const QJsonArray &array)
{
    QStringList gpus;
    gpus.reserve(array.size());
    for (const auto &device : array) {
        const auto obj = device.toObject();
        const QString name = FancyString::fromRenderer(obj.value("name").toString());
        if (name.isEmpty()) {
            qWarning() << "Empty name for device";
            return {};
        }
        gpus << name;
    }
    if (gpus.count() > 1 && gpus.count() != deviceCount()) {
        for (auto it = gpus.begin(); it != gpus.end();) {
            if (it->contains("llvmpipe"_L1)) {
                qDebug() << "opengl: excess llvmpipe detected, ignoring";
                it = gpus.erase(it);
            } else {
                ++it;
            }
        }
        if (gpus.count() != deviceCount()) {
            qWarning() << "GPU count mismatch (from JSON)" << gpus.count() << deviceCount();
            return {};
        }
    }
    return gpus;
}

std::optional<QStringList> vulkanGPUs()
{
    const auto devices = vulkanDevices();

    QStringList gpus;
    gpus.reserve(devices.size());

    const auto typeToString = [](VkPhysicalDeviceType type) {
        switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return i18nc("@label GPU type", "Integrated");
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return i18nc("@label GPU type", "Discrete");
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return i18nc("@label GPU type. GPU of a VM", "Virtual");
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
            break;
        }
        return QString();
    };

    for (const auto &device : devices) {
        const QString name = FancyString::fromRenderer(device.name);
        const QString type = typeToString(device.type);
        if (name.isEmpty()) {
            qWarning() << "Empty name for device" << device.type;
            return {};
        }
        if (type.isEmpty()) {
            gpus.push_back(name);
        } else {
            gpus.push_back(i18nc("@label %1 is a GPU name and %2 a type such as 'Integrated'", "%1 [%2]", name, type));
        }
    }

    if (gpus.count() > 1 && gpus.count() != deviceCount()) {
        for (auto it = gpus.begin(); it != gpus.end();) {
            if (it->contains("llvmpipe"_L1)) {
                qDebug() << "vulkan: excess llvmpipe detected, ignoring";
                it = gpus.erase(it);
            } else {
                ++it;
            }
        }
        if (gpus.count() != deviceCount()) {
            qWarning() << "GPU count mismatch (from vulkan)" << gpus.count() << deviceCount();
            return {};
        }
    }

    return gpus;
}

std::optional<QStringList> openglGPUs()
{
    auto openglHelperExecutable = QStandardPaths::findExecutable("kinfocenter-opengl-helper", searchPaths());
    qDebug() << "Looking at" << searchPaths() << openglHelperExecutable;

    QJsonArray array;
    for (auto i = 0; i < deviceCount(); ++i) {
        auto document = readFromProcess(openglHelperExecutable, i);
        if (!document.isArray()) {
            qWarning() << "Failed to read GPU info from opengl helper for device" << i;
            return {};
        }

        const auto incomingArray = document.array();
        for (const auto &device : incomingArray) {
            array.append(device);
        }
    }

    return gpusFromJsonArray(array);
}

std::optional<QStringList> gpus()
{
    if (deviceCount() == 0) {
        return {};
    }

    if (auto optionalGpus = vulkanGPUs(); optionalGpus.has_value()) {
        return optionalGpus;
    }

    if (auto optionalGpus = openglGPUs(); optionalGpus.has_value()) {
        return optionalGpus;
    }

    return {};
}

QString fancyOpenGLRenderer()
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    surface.create();
    if (!context.create()) {
        qWarning() << "Failed create QOpenGLContext";
        return {};
    }

    if (!context.makeCurrent(&surface)) {
        qWarning() << "Failed to make QOpenGLContext current";
        return {};
    }

    auto renderer = QString::fromUtf8(reinterpret_cast<const char *>(context.functions()->glGetString(GL_RENDERER)));
    context.doneCurrent();

    return FancyString::fromRenderer(renderer);
}
} // namespace

GPUEntry::GPUEntry()
    : Entry(ki18n("Graphics Processor:"), QString())
{
    if (auto optionalGpus = gpus(); optionalGpus.has_value()) {
        m_value = optionalGpus->join('\n');
    } else { // fall back to old logic of using the GL_RENDERER
        m_value = fancyOpenGLRenderer();
    }
}
