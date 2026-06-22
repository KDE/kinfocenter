// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2026 Harald Sitter <sitter@kde.org>

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <iostream>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVulkanInstance> // for VK_PHYSICAL_DEVICE_TYPE_OTHER

using namespace Qt::StringLiterals;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    auto eglQueryDevicesEXT = reinterpret_cast<PFNEGLQUERYDEVICESEXTPROC>(eglGetProcAddress("eglQueryDevicesEXT"));
    auto eglQueryDeviceStringEXT = reinterpret_cast<PFNEGLQUERYDEVICESTRINGEXTPROC>(eglGetProcAddress("eglQueryDeviceStringEXT"));

    EGLint numDevices = 0;
    if (eglQueryDevicesEXT(0, nullptr, &numDevices) != EGL_TRUE) {
        qWarning() << "Failed to query number of EGL devices";
        return 1;
    }
    QVarLengthArray<EGLDeviceEXT, 2> eglDevices(numDevices);
    if (eglQueryDevicesEXT(numDevices, eglDevices.data(), &numDevices) != EGL_TRUE) {
        qWarning() << "Failed to query EGL devices";
        return 1;
    }

    QJsonArray devicesArray;
    for (const auto &device : eglDevices) {
        const auto extensions = QByteArrayView(eglQueryDeviceStringEXT(device, EGL_EXTENSIONS));
        if (extensions.contains("EGL_MESA_device_software"_ba)) { // this is llvmpipe and will not have a name, just skip it
            continue;
        }

        const auto name = QString::fromUtf8(eglQueryDeviceStringEXT(device, EGL_RENDERER_EXT));
        if (name.isEmpty()) {
            // nvidia is naturally being difficult and sometimes returns no renderer (even though the extensions say it should).
            // Notably happens with the KDE e.V's
            //   01:00.0 VGA compatible controller [0300]: NVIDIA Corporation GA107 [GeForce RTX 3050 6GB] [10de:2584] (rev a1)
            // which appears to report as a EGL_NV_device_cuda|EGL_EXT_device_drm and separately as a regular EGL_EXT_device_drm device.
            // The former has a name, the latter just produces stderr output.
            //
            // The GPUEntryFactory discards arrays with unexpected empty names so we kick them out beforehand.
            // Should the array not have names for all drm devices the factory will fall back to other solutions anyway so we can safely remove stuff
            // and rely on the factory to figure out if we removed "too much".
            continue;
        }

        const auto vkType = [&]() -> VkPhysicalDeviceType { // for practical reasons we only send vulkan types from the helpers
#if defined(EGL_DEVICE_TYPE_OTHER_EXT)
            auto eglQueryDeviceAttribEXT = reinterpret_cast<PFNEGLQUERYDEVICEATTRIBEXTPROC>(eglGetProcAddress("eglQueryDeviceAttribEXT"));
            if (EGLAttrib attribute{}; eglQueryDeviceAttribEXT(device, EGL_DEVICE_EXT, &attribute) == EGL_TRUE) {
                switch (attribute) {
                case EGL_DEVICE_TYPE_OTHER_EXT:
                    return VK_PHYSICAL_DEVICE_TYPE_OTHER;
                case EGL_DEVICE_TYPE_INTEGRATED_GPU_EXT:
                    return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
                case EGL_DEVICE_TYPE_DISCRETE_GPU_EXT:
                    return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
                case EGL_DEVICE_TYPE_CPU_EXT:
                    return VK_PHYSICAL_DEVICE_TYPE_CPU;
                }
            }
            qDebug() << "Failed to query EGL device type, falling back to VK_PHYSICAL_DEVICE_TYPE_OTHER.";
#endif
            return VK_PHYSICAL_DEVICE_TYPE_OTHER;
        }();

        devicesArray.append(QJsonObject{
            {u"name"_s, name},
            {u"vkDeviceType"_s, vkType},
        });
    }

    std::cout << QJsonDocument(devicesArray).toJson(QJsonDocument::Compact).toStdString() << '\n';
    return 0;
}
