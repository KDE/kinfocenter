// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "GPUEntryFactory.h"

#include <algorithm>

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

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        if (line.startsWith("nvidia"_L1)) {
            return true;
        }
        line = in.readLine();
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

size_t drmDeviceCount()
{
    static auto count = [] {
        if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
            return narrow<size_t>(3); // NOTE: this is intentionally off by one. We don't see llvmpipe in drmGetDevices2!
        }
        int ret = drmGetDevices2(0, nullptr, 0);
        if (ret < 0) {
            qWarning() << "drmGetDevices2() failed with " << ret;
            drmError(ret, "drmGetDevices2()");
            return narrow<size_t>(0);
        }

        return narrow<size_t>(ret);
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

    auto vulkanHelperExecutable = QStandardPaths::findExecutable("kinfocenter-vulkan-helper", searchPaths());
    qDebug() << "Looking at" << searchPaths() << vulkanHelperExecutable;

    QJsonDocument document = readFromProcess(vulkanHelperExecutable, 0);
    if (!document.isArray()) {
        qWarning() << "Failed to read GPU info from vulkan helper";
        return {};
    }

    const QJsonArray array = document.array();
    std::vector<GPUEntry::Device> deviceList;
    deviceList.reserve(array.size());
    for (const auto &device : array) {
        const auto obj = device.toObject();
        const QString name = FancyString::fromRenderer(obj.value("name").toString());
        if (name.isEmpty()) {
            qWarning() << "Empty name for device";
            return {};
        }
        const VkPhysicalDeviceType type = static_cast<VkPhysicalDeviceType>(obj.value("type").toInt(VK_PHYSICAL_DEVICE_TYPE_OTHER));
        deviceList.emplace_back(GPUEntry::Device{name, type});
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

bool isGoodDeviceData(std::vector<GPUEntry::Device> &devices, bool finalSource)
{
    if (finalSource && devices.size() <= 1) {
        // Single devices always get displayed so long as we are allowed to contain llvmpipe. llvmpipe always introduces
        // a device mismatch.
        // finalSource is false for all GPU sources except the last fallback.
        //   i.e. vulkan=false > opengl=true
        // Which means vulkan will stumble if only llvmpipe is present, but opengl will still show it.
        return true;
    }

    // If we are not on the final source (i.e. vulkan) but have no type information on anything, then we don't lose
    // anything by checking another source (i.e. opengl). This then allows us to get opengl's information in case vulkan
    // isn't supported on this hardware.
    // https://bugs.kde.org/show_bug.cgi?id=499882
    if (!finalSource) {
        // Only applies when llvmpipe is in the list. Otherwise we should only have gotten real hardware.
        const auto atLeastOnellvmpipe = std::ranges::any_of(devices, [](const auto &dev) {
            return dev.name.contains("llvmpipe"_L1);
        });
        // If then also all types are OTHER then we also have no useful type information.
        const auto onlyOtherType = std::ranges::all_of(devices, [](const auto &dev) {
            return dev.type == VK_PHYSICAL_DEVICE_TYPE_OTHER;
        });
        if (atLeastOnellvmpipe && onlyOtherType) {
            // Combined this tells us that we may be getting better data from opengl...
            return false;
        }
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

    if (!isGoodDeviceData(devices, false)) {
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
    for (size_t i = 0; i < drmDeviceCount(); ++i) {
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

    if (!isGoodDeviceData(devices, true)) {
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
