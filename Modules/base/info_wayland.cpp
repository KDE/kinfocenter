/*
Copyright 2014  Martin Gräßlin <mgraesslin@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "info_wayland.h"

#include <KLocalizedString>
#include <QSocketNotifier>
#include <QTreeWidgetItem>

#include <wayland-client.h>
#include <wayland-client.h>

namespace Wayland {

static void outputHandleGeometry(void *data, wl_output *output, int32_t x, int32_t y,
                                 int32_t physicalWidth, int32_t physicalHeight, int32_t subPixel,
                                 const char *make, const char *model, int32_t transform)
{
    Q_UNUSED(output)
    QTreeWidgetItem *parent = reinterpret_cast<QTreeWidgetItem*>(data);
    QTreeWidgetItem *o = new QTreeWidgetItem(parent, QStringList() << i18n("Output"));
    new QTreeWidgetItem(o, QStringList() << i18n("Manufacturer") << QString::fromUtf8(make));
    new QTreeWidgetItem(o, QStringList() << i18n("Model") << QString::fromUtf8(model));
    new QTreeWidgetItem(o, QStringList() << i18n("Physical Width") << QString::number(physicalWidth));
    new QTreeWidgetItem(o, QStringList() << i18n("Physical Height") << QString::number(physicalHeight));
    new QTreeWidgetItem(o, QStringList() << i18n("X") << QString::number(x));
    new QTreeWidgetItem(o, QStringList() << i18n("Y") << QString::number(y));
    new QTreeWidgetItem(o, QStringList() << i18n("Subpixel") << QString::number(subPixel));
    QString transformText;
    switch (transform) {
    case 0:
        transformText = i18n("Normal");
        break;
    case 1:
        transformText = QStringLiteral("90");
        break;
    case 2:
        transformText = QStringLiteral("180");
        break;
    case 3:
        transformText = QStringLiteral("270");
        break;
    case 4:
        transformText = i18n("Flipped");
        break;
    case 5:
        transformText = i18n("Flipped 90");
        break;
    case 6:
        transformText = i18n("Flipped 180");
        break;
    case 7:
        transformText = i18n("Flipped 270");
        break;
    }
    new QTreeWidgetItem(o, QStringList() << i18n("Transform") << transformText);
}

static void outputHandleMode(void *data, wl_output *output, uint flags, int32_t width, int32_t height, int32_t refresh)
{
    Q_UNUSED(data)
    Q_UNUSED(output)
    Q_UNUSED(flags)
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(refresh)
}

static void outputHandleDone(void *data, wl_output *output)
{
    Q_UNUSED(data)
    Q_UNUSED(output)
}

static void outputHandleScale(void *data, wl_output *output, int32_t scale)
{
    Q_UNUSED(data)
    Q_UNUSED(output)
    Q_UNUSED(scale)
}

static const struct wl_output_listener s_outputListener = {
    outputHandleGeometry,
    outputHandleMode,
    outputHandleDone,
    outputHandleScale
};

static void seatHandleCapabilities(void *data, wl_seat *seat, uint32_t capabilities)
{
    Q_UNUSED(seat)
    QTreeWidgetItem *parent = reinterpret_cast<QTreeWidgetItem*>(data);
    if ((capabilities & WL_SEAT_CAPABILITY_POINTER)) {
        new QTreeWidgetItem(parent, QStringList() << i18n("Pointer"));
    }
    if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
        new QTreeWidgetItem(parent, QStringList() << i18n("Keyboard"));
    }
    if ((capabilities & WL_SEAT_CAPABILITY_TOUCH)) {
        new QTreeWidgetItem(parent, QStringList() << i18n("Touch"));
    }
}

static void seatHandleName(void *data, wl_seat *seat, const char *name)
{
    Q_UNUSED(seat)
    QTreeWidgetItem *parent = reinterpret_cast<QTreeWidgetItem*>(data);
    new QTreeWidgetItem(parent, QStringList() << i18n("Name") << QString::fromUtf8(name));
}

static const struct wl_seat_listener s_seatListener = {
    seatHandleCapabilities,
    seatHandleName
};

static void registryHandleGlobal(void *data, struct wl_registry *registry,
                                        uint32_t name, const char *interface, uint32_t version)
{
    WaylandModule *wayland = reinterpret_cast<WaylandModule*>(data);

    new QTreeWidgetItem(wayland->interfacesItem(), QStringList() << QString::fromUtf8(interface) << QString::number(version));

    if (strcmp(interface, "wl_output") == 0) {
        QTreeWidgetItem *outputItems = new QTreeWidgetItem(wayland->root(), QStringList() << i18n("Outputs"));
        wl_output *output = reinterpret_cast<wl_output *>(wl_registry_bind(registry, name, &wl_output_interface, 1));
        wl_output_add_listener(output, &s_outputListener, outputItems);
        wayland->flush();
    }
    if (strcmp(interface, "wl_seat") == 0) {
        QTreeWidgetItem *seatItem = new QTreeWidgetItem(wayland->root(), QStringList() << i18n("Seat"));
        wl_seat *seat = reinterpret_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
        wl_seat_add_listener(seat, &s_seatListener, seatItem);
        wayland->flush();
    }
}

static void registryHandleGlobalRemove(void *data, wl_registry *registry, uint32_t name)
{
    Q_UNUSED(data)
    Q_UNUSED(registry)
    Q_UNUSED(name)
}

static const struct wl_registry_listener s_registryListener = {
    registryHandleGlobal,
    registryHandleGlobalRemove
};

} // namespace Wayland

WaylandModule::WaylandModule(QTreeWidget *parent)
    : QObject(parent)
    , m_display(nullptr)
    , m_registry(nullptr)
    , m_tree(parent)
    , m_interfacesItem(nullptr)
    , m_compositorItem(nullptr)
{
    init();
}

WaylandModule::~WaylandModule()
{
    if (m_registry) {
        wl_registry_destroy(m_registry);
    }
    if (m_display) {
        wl_display_flush(m_display);
        wl_display_disconnect(m_display);
    }
}

void WaylandModule::init()
{
    m_display = wl_display_connect(nullptr);
    if (!m_display) {
        return;
    }

    m_tree->setHeaderLabels(QStringList() << i18n("Information") << i18n("Value"));
    m_tree->setSortingEnabled(false);

    m_compositorItem = new QTreeWidgetItem(m_tree, QStringList() << i18n("Compositor Information"));
    m_compositorItem->setIcon(0, QIcon::fromTheme(QStringLiteral("wayland")));
    m_compositorItem->setExpanded(true);

    new QTreeWidgetItem(m_compositorItem, QStringList() << i18n("Name of the Display") << qgetenv("WAYLAND_DISPLAY"));

    m_interfacesItem = new QTreeWidgetItem(m_compositorItem, QStringList() << i18n("Interfaces") << i18n("Interface Version"));
    m_interfacesItem->setExpanded(true);

    m_registry = wl_display_get_registry(m_display);
    if (!m_registry) {
        return;
    }
    wl_registry_add_listener(m_registry, &Wayland::s_registryListener, this);

    int fd = wl_display_get_fd(m_display);
    QSocketNotifier *notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &WaylandModule::readEvents);

    flush();
}

void WaylandModule::flush()
{
    wl_display_dispatch_pending(m_display);
    wl_display_flush(m_display);
}

void WaylandModule::readEvents()
{
    wl_display_flush(m_display);
    wl_display_dispatch(m_display);
}

bool WaylandModule::isValid() const
{
    return m_registry != nullptr;
}
