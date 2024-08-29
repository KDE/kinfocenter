/*
    SPDX-FileCopyrightText: 2016 Rohan Garg <rohan@kde.org>
    SPDX-FileCopyrightText: 2020-2021 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "GPUEntry.h"

#include <QDebug>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <KLocalizedString>

#include "FancyString.h"

extern "C" {
#include <pci/pci.h>
#include <xf86drm.h>
}

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

std::optional<QStringList> gpus()
{
    if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
        return QStringList{u"GP106 [GeForce GTX 1060 6GB]"_s, u"HD Graphics 630"_s};
    }

    int device_count = drmGetDevices2(0, nullptr, 0);
    if (device_count <= 0) {
        return {};
    }

    std::vector<drmDevice *> devices(device_count);
    if (drmGetDevices2(0, devices.data(), narrow<int>(devices.size())) < 0) {
        return {};
    }
    auto cleanDevices = qScopeGuard([&devices] {
        drmFreeDevices(devices.data(), narrow<int>(devices.size()));
    });

    QStringList deviceNames;
    for (const auto &device : devices) {
        if (device->bustype != DRM_BUS_PCI) {
            continue;
        }

        drmPciDeviceInfoPtr pci = device->deviceinfo.pci;
        pci_access *access = pci_alloc();
        if (!access) {
            qWarning() << "Failed to allocate drmPciDeviceInfoPtr" << strerror(errno);
            continue;
        }
        auto accessCleanup = qScopeGuard([access] {
            pci_cleanup(access);
        });

        constexpr auto bufferSize = 256; // libpci basically always allocates 128 for name lookups. Let's double that to be on the safe side.
        std::array<char, bufferSize> buffer{};
        const char *name = pci_lookup_name(access, buffer.data(), buffer.size(), PCI_LOOKUP_DEVICE, pci->vendor_id, pci->device_id);
        deviceNames.append(QString::fromUtf8(name));
    }

    return deviceNames;
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

    renderer = FancyString::fromUgly(renderer);
    // It seems the renderer value may have excess information in parentheses ->
    // strip that. Elide would probably be nicer, a bit meh with QWidgets though.
    renderer = renderer.mid(0, renderer.indexOf('('));
    // Leads to trailing space in my case, don't know whether that is happening
    // everywhere, though. Thus removing trailing spaces separately.
    renderer = renderer.trimmed();

    return renderer;
}
} // namespace

GPUEntry::GPUEntry()
    : Entry(ki18n("Graphics Processor:"), QString())
{
    if (auto optionalGpus = gpus(); optionalGpus.has_value()) {
        m_value = optionalGpus->join('\n');
        m_value = FancyString::fromUgly(m_value);
    } else { // fall back to old logic of using the GL_RENDERER
        m_value = fancyOpenGLRenderer();
    }
}
