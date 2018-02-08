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
#include <QTreeWidgetItem>
#include <QThread>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/event_queue.h>
#include <KWayland/Client/keyboard.h>
#include <KWayland/Client/output.h>
#include <KWayland/Client/seat.h>
#include <KWayland/Client/registry.h>

WaylandModule::WaylandModule(QTreeWidget *parent)
    : QObject(parent)
    , m_tree(parent)
    , m_thread(new QThread(this))
    , m_connection(new KWayland::Client::ConnectionThread)
{
    init();
}

WaylandModule::~WaylandModule()
{
    delete m_registry;
    m_connection->deleteLater();
    m_thread->quit();
    m_thread->wait();
}

void WaylandModule::init()
{
    using namespace KWayland::Client;
    m_connection->moveToThread(m_thread);
    m_thread->start();

    m_tree->setHeaderLabels(QStringList() << i18n("Information") << i18n("Value"));
    m_tree->setSortingEnabled(false);

    auto compositorItem = new QTreeWidgetItem(m_tree, QStringList() << i18n("Compositor Information"));
    compositorItem->setIcon(0, QIcon::fromTheme(QStringLiteral("wayland")));
    compositorItem->setExpanded(true);

    new QTreeWidgetItem(compositorItem, QStringList() << i18n("Name of the Display") << qgetenv("WAYLAND_DISPLAY"));

    auto interfacesItem = new QTreeWidgetItem(compositorItem, QStringList() << i18n("Interfaces") << i18n("Interface Version"));
    interfacesItem->setExpanded(true);

    connect(m_connection, &ConnectionThread::connected, this,
        [this, compositorItem, interfacesItem] {
            Registry *registry = new Registry(this);
            EventQueue *queue = new EventQueue(registry);
            queue->setup(m_connection);
            registry->setEventQueue(queue);
            connect(registry, &Registry::interfaceAnnounced, this,
                [this, interfacesItem] (const QByteArray &interface, quint32 name, quint32 version) {
                    Q_UNUSED(name)
                    new QTreeWidgetItem(interfacesItem, QStringList() << interface << QString::number(version));
                }
            );
            connect(registry, &Registry::seatAnnounced, this,
                [this, registry, compositorItem] (quint32 name, quint32 version) {
                    QTreeWidgetItem *seatItem = new QTreeWidgetItem(compositorItem, QStringList() << i18n("Seat") << QString());
                    seatItem->setExpanded(true);
                    Seat *seat = registry->createSeat(name, version, registry);
                    connect(seat, &Seat::nameChanged, this,
                        [this, seat, seatItem] {
                            new QTreeWidgetItem(seatItem, QStringList() << i18n("Name") << seat->name());
                        }
                    );
                    connect(seat, &Seat::hasPointerChanged, this,
                        [this, seat, seatItem] {
                            if (seat->hasPointer()) {
                                new QTreeWidgetItem(seatItem, QStringList() << i18n("Pointer"));
                            }
                        }
                    );
                    connect(seat, &Seat::hasKeyboardChanged, this,
                        [this, seat, seatItem] {
                            if (seat->hasKeyboard()) {
                                auto i = new QTreeWidgetItem(seatItem, QStringList{i18n("Keyboard")});
                                i->setExpanded(true);
                                auto e = new QTreeWidgetItem(i, QStringList{i18n("Repeat enabled")});
                                auto r = new QTreeWidgetItem(i, QStringList{i18n("Repeat rate (characters per second)")});
                                auto d = new QTreeWidgetItem(i, QStringList{i18n("Repeat delay (msec)")});
                                auto k = seat->createKeyboard(seat);
                                connect(k, &Keyboard::keyRepeatChanged, this,
                                        [this, k, e, r, d] {
                                            e->setText(1, k->isKeyRepeatEnabled() ? i18n("Yes") : i18n("No"));
                                            r->setText(1, QString::number(k->keyRepeatRate()));
                                            d->setText(1, QString::number(k->keyRepeatDelay()));
                                        }
                                );
                            }
                        }
                    );
                    connect(seat, &Seat::hasTouchChanged, this,
                        [this, seat, seatItem] {
                            if (seat->hasTouch()) {
                                new QTreeWidgetItem(seatItem, QStringList() << i18n("Touch"));
                            }
                        }
                    );
                }
            );
            QTreeWidgetItem *outputItem = new QTreeWidgetItem(compositorItem, QStringList() << i18n("Outputs"));
            outputItem->setExpanded(true);
            connect(registry, &Registry::outputAnnounced, this,
                [this, registry, outputItem] (quint32 name, quint32 version) {
                    Output *output = registry->createOutput(name, version, registry);
                    connect(output, &Output::changed, this,
                        [this, output, outputItem] {
                            output->deleteLater();
                            const QSize s = output->physicalSize();
                            const QPoint p = output->globalPosition();

                            auto o = new QTreeWidgetItem(outputItem, QStringList() << QString::number(outputItem->childCount()));
                            o->setExpanded(true);

                            new QTreeWidgetItem(o, QStringList() << i18n("Manufacturer") << output->manufacturer());
                            new QTreeWidgetItem(o, QStringList() << i18n("Model") << output->model());
                            new QTreeWidgetItem(o, QStringList() << i18n("Physical Size") << QStringLiteral("%1x%2").arg(s.width()).arg(s.height()));
                            new QTreeWidgetItem(o, QStringList() << i18n("Global Position") << QStringLiteral("%1/%2").arg(p.x()).arg(p.y()));

                            QString subPixel;
                            switch (output->subPixel()) {
                            case Output::SubPixel::None:
                                subPixel = i18n("None");
                                break;
                            case Output::SubPixel::HorizontalRGB:
                                subPixel = i18n("Horizontal RGB");
                                break;
                            case Output::SubPixel::HorizontalBGR:
                                subPixel = i18n("Horizontal BGR");
                                break;
                            case Output::SubPixel::VerticalRGB:
                                subPixel = i18n("Vertical RGB");
                                break;
                            case Output::SubPixel::VerticalBGR:
                                subPixel = i18n("Vertical BGR");
                                break;
                            case Output::SubPixel::Unknown:
                            default:
                                subPixel = i18n("Unknown");
                                break;
                            }
                            new QTreeWidgetItem(o, QStringList() << i18n("Subpixel") << subPixel);

                            QString transform;
                            switch (output->transform()) {
                            case Output::Transform::Rotated90:
                                transform = QStringLiteral("90");
                                break;
                            case Output::Transform::Rotated180:
                                transform = QStringLiteral("180");
                                break;
                            case Output::Transform::Rotated270:
                                transform = QStringLiteral("270");
                                break;
                            case Output::Transform::Flipped:
                                transform = i18n("Flipped");
                                break;
                            case Output::Transform::Flipped90:
                                transform = i18n("Flipped 90");
                                break;
                            case Output::Transform::Flipped180:
                                transform = i18n("Flipped 180");
                                break;
                            case Output::Transform::Flipped270:
                                transform = i18n("Flipped 270");
                                break;
                            case Output::Transform::Normal:
                            default:
                                transform = i18n("Normal");
                                break;
                            }
                            new QTreeWidgetItem(o, QStringList() << i18n("Transform") << transform);
                            new QTreeWidgetItem(o, QStringList() << i18nc("The scale factor of the output", "Scale") << QString::number(output->scale()));

                            // add Modes
                            auto modesItem = new QTreeWidgetItem(o, QStringList() << i18n("Modes"));
                            int i = 0;
                            for (const auto &mode : output->modes()) {
                                auto modeItem = new QTreeWidgetItem(modesItem, QStringList() << QString::number(i++));
                                modeItem->setExpanded(true);
                                new QTreeWidgetItem(modeItem, QStringList() << i18n("Size")
                                                                            << QStringLiteral("%1x%2").arg(mode.size.width()).arg(mode.size.height()));
                                new QTreeWidgetItem(modeItem, QStringList() << i18n("Refresh Rate")
                                                                            << QString::number(mode.refreshRate));
                                new QTreeWidgetItem(modeItem, QStringList() << i18n("Preferred")
                                                                            << (mode.flags.testFlag(Output::Mode::Flag::Preferred) ? i18n("yes") : i18n("no")));
                                new QTreeWidgetItem(modeItem, QStringList() << i18n("Current")
                                                                            << (mode.flags.testFlag(Output::Mode::Flag::Current) ? i18n("yes") : i18n("no")));
                            }
                        }
                    );
                }
            );
            m_registry = registry;
            registry->create(m_connection);
            registry->setup();
        },
        Qt::QueuedConnection
    );

    m_connection->initConnection();
}
