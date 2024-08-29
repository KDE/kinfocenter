// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include <iostream>

#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QVulkanInstance>

using namespace Qt::StringLiterals;

QString glDeviceName()
{
    // NOTE: The simulation is actually not working for DRI_PRIME because mesa prints an error on stdout, breaking the JSON output.
    if (qEnvironmentVariableIntValue("KINFOCENTER_SIMULATION") == 1) {
        switch (qEnvironmentVariableIntValue("DRI_PRIME")) {
        default:
            return u"AMD Radeon RX 7900 XTX (radeonsi, navi31, LLVM 18.1.8, DRM 3.57, 6.10.7-arch1-1)"_s;
        case 1:
            return u"AMD Radeon Graphics (radeonsi, raphael_mendocino, LLVM 18.1.8, DRM 3.57, 6.10.7-arch1-1)"_s;
        case 2:
            return u"Intel(R) UHD Graphics (CML GT2)"_s;
        case 3:
            return u"llvmpipe (LLVM 18.1.6, 256 bits)"_s;
        }
    }

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

    const QJsonArray devicesArray{QJsonObject{
        {u"name"_s, glDeviceName()},
        {u"vkDeviceType"_s, VK_PHYSICAL_DEVICE_TYPE_OTHER},
    }};

    std::cout << QJsonDocument(devicesArray).toJson(QJsonDocument::Compact).toStdString() << '\n';

    return 0;
}
