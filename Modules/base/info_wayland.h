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

#ifndef INFO_WAYLAND_H
#define INFO_WAYLAND_H

#include <QObject>

struct wl_display;
struct wl_registry;
class QTreeWidget;
class QTreeWidgetItem;

class WaylandModule : public QObject
{
    Q_OBJECT
public:
    WaylandModule(QTreeWidget *lBox);
    ~WaylandModule();
    void flush();
    wl_display *display() const {
        return m_display;
    }

    QTreeWidgetItem *interfacesItem() const {
        return m_interfacesItem;
    }

    QTreeWidgetItem *root() const {
        return m_compositorItem;
    }

    bool isValid() const;

private:
    void init();
    void readEvents();
    wl_display *m_display;
    wl_registry *m_registry;
    QTreeWidget *m_tree;
    QTreeWidgetItem *m_interfacesItem;
    QTreeWidgetItem *m_compositorItem;
};

#endif // INFO_WAYLAND_H
