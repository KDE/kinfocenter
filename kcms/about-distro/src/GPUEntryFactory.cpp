// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "GPUEntryFactory.h"

#include <QDebug>
#include <QFile>
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

extern "C" {
#include <xf86drm.h>
}

#include "FancyString.h"
#include "GPUEntry.h"

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

bool isNvidiaLoaded()
{
    QFile file("/proc/modules");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open /proc/modules";
        return false;
    }

    while (!file.atEnd()) {
        const auto line = file.readLine();
        if (line.startsWith("nvidia"_L1)) {
            return true;
        }
    }

    return false;
}

QJsonDocument readFromProcess(const QString &executable, int deviceIndex)
{
    QProcess process;

    auto processEnvironment = QProcessEnvironment::systemEnvironment();
    if (deviceIndex > 0) {
        if (isNvidiaLoaded()) {
            // nvidia docs are unclear if __NV_PRIME_RENDER_OFFLOAD is a bool or an index. We only support 0 and 1 until someone turns up with a 3rd gpu, so
            // we can test if it is an index.
            const auto supportedIndex = deviceIndex <= 1;
            Q_ASSERT(supportedIndex);
            if (!supportedIndex) {
                qWarning() << "Unsupported device index" << deviceIndex;
                return {};
            }
            processEnvironment.insert(u"__NV_PRIME_RENDER_OFFLOAD"_s, QString::number(deviceIndex));
            processEnvironment.insert(u"__GLX_VENDOR_LIBRARY_NAME"_s, u"nvidia"_s);
        } else { // assume mesa
            processEnvironment.insert("DRI_PRIME", QString::number(deviceIndex));
        }
    }
    process.setProcessEnvironment(processEnvironment);

    process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    process.setProgram(executable);
    process.setArguments({u"--platform"_s, qGuiApp->platformName()});
    process.start();
    process.waitForFinished(std::chrono::milliseconds(2s).count());
    auto output = process.readAllStandardOutput();
    return QJsonDocument::fromJson(output);
}

QStringList searchPaths()
{
    static auto paths = KLibexec::kdeFrameworksPaths(QLatin1String(LIBEXECDIR_FROM_KCM)) << QLatin1String(KDE_INSTALL_FULL_LIBEXECDIR);
    return paths;
}

int drmDeviceCount()
{
    static auto count = [] {
        if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
            return 3; // NOTE: this is intentionally off by one. We don't see llvmpipe in drmGetDevices2!
        }
        return drmGetDevices2(0, nullptr, 0);
    }();
    return count;
}

std::vector<GPUEntry::Device> vulkanDevices()
{
    if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
        return {
            {u"AMD Radeon RX 7900 XTX\nMesa 1.2.3 (RADV NAVI31)]"_s, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU},
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

    std::vector<GPUEntry::Device> deviceList;
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

void stripLlvmpipe(std::vector<GPUEntry::Device> &devices)
{
    for (auto it = devices.begin(); it != devices.end();) {
        if (it->name.contains("llvmpipe"_L1)) {
            qDebug() << "excess llvmpipe detected, ignoring";
            it = devices.erase(it);
        } else {
            ++it;
        }
    }
}

bool devicesAddUpAfterStripping(std::vector<GPUEntry::Device> &devices, bool finalSource)
{
    if (finalSource && devices.size() <= 1) {
        // Single devices always get displayed so long as we are allowed to contain llvmpipe. llvmpipe always introduces
        // a device mismatch.
        // finalSource is false for all GPU sources except the last fallback.
        //   i.e. vulkan=false > opengl=true
        // Which means vulkan will stumble if only llvmpipe is present, but opengl will still show it.
        return true;
    }

    if (devices.size() != drmDeviceCount()) {
        stripLlvmpipe(devices);
        if (devices.size() != drmDeviceCount()) {
            return false;
        }
    }

    return true;
}

std::optional<std::vector<GPUEntry::Device>> vulkanGPUs()
{
    auto devices = vulkanDevices();

    if (!devicesAddUpAfterStripping(devices, false)) {
        qWarning() << "GPU count mismatch (from vulkan). Are you maybe missing vulkan drivers?" << devices.size() << drmDeviceCount();
        return {};
    }

    return devices;
}

std::optional<std::vector<GPUEntry::Device>> openglGPUs()
{
    auto openglHelperExecutable = QStandardPaths::findExecutable("kinfocenter-opengl-helper", searchPaths());
    qDebug() << "Looking at" << searchPaths() << openglHelperExecutable;

    QJsonArray array;
    for (auto i = 0; i < drmDeviceCount(); ++i) {
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

    std::vector<GPUEntry::Device> devices;
    devices.reserve(array.size());
    for (const auto &device : array) {
        const auto obj = device.toObject();
        const QString name = FancyString::fromRenderer(obj.value("name").toString());
        if (name.isEmpty()) {
            qWarning() << "Empty name for device";
            return {};
        }
        devices.push_back(GPUEntry::Device{name, VK_PHYSICAL_DEVICE_TYPE_OTHER});
    }

    if (!devicesAddUpAfterStripping(devices, true)) {
        qWarning() << "GPU count mismatch (from opengl)" << devices.size() << drmDeviceCount();
        return {};
    }

    return devices;
}

std::optional<std::vector<GPUEntry::Device>> gpus()
{
    if (drmDeviceCount() == 0) {
        return {};
    }

    if (auto optionalGpus = vulkanGPUs(); optionalGpus.has_value() && !optionalGpus->empty()) {
        return optionalGpus;
    }

    if (auto optionalGpus = openglGPUs(); optionalGpus.has_value() && !optionalGpus->empty()) {
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

std::vector<Entry *> GPUEntryFactory::factorize()
{
    if (auto optionalGpus = gpus(); optionalGpus.has_value()) {
        std::vector<Entry *> entries;
        entries.reserve(optionalGpus->size());
        int i = 1;
        for (const auto &gpu : optionalGpus.value()) {
            std::optional<int> deviceIndex = std::nullopt;
            if (optionalGpus->size() > 1) {
                deviceIndex = i;
                i++;
            }
            entries.push_back(new GPUEntry(deviceIndex, gpu));
        }
        return entries;
    }

    // fall back to old logic of using the singular GL_RENDERER of the app
    return {new GPUEntry({}, GPUEntry::Device{.name = fancyOpenGLRenderer(), .type = VK_PHYSICAL_DEVICE_TYPE_OTHER})};
}
