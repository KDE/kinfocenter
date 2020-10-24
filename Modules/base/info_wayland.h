/*
SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>

SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef INFO_WAYLAND_H
#define INFO_WAYLAND_H

#include <QObject>

class QTreeWidget;
class QThread;

namespace KWayland
{
namespace Client
{
class ConnectionThread;
class Registry;
}
}

class WaylandModule : public QObject
{
    Q_OBJECT
public:
    WaylandModule(QTreeWidget *lBox);
    ~WaylandModule();

private:
    void init();
    QTreeWidget *m_tree;
    QThread *m_thread;
    KWayland::Client::ConnectionThread *m_connection;
    KWayland::Client::Registry *m_registry = nullptr;
};

#endif // INFO_WAYLAND_H
