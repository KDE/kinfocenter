/*
 * main.cpp
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * https://www.qt.io/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <KLocalizedString>
#include <KPluginFactory>

/* we have to include the info.cpp-file, to get the DEFINES about possible properties.
   example: we need the "define INFO_DMA_AVAILABLE" */
#include "info.h"

#include "os_current.h"

#define CREATE_FACTORY(type, name)                                                                                                                             \
    class K##type##InfoWidget : public KInfoListWidget                                                                                                         \
    {                                                                                                                                                          \
    public:                                                                                                                                                    \
        K##type##InfoWidget(QWidget *parent, const QVariantList &)                                                                                             \
            : KInfoListWidget(name, parent, GetInfo_##type)                                                                                                    \
        {                                                                                                                                                      \
        }                                                                                                                                                      \
    };

#ifdef INFO_IRQ_AVAILABLE
CREATE_FACTORY(IRQ, i18n("Interrupt"))
#endif
#ifdef INFO_IOPORTS_AVAILABLE
CREATE_FACTORY(IO_Ports, i18n("I/O-Port"))
#endif
#ifdef INFO_DMA_AVAILABLE
CREATE_FACTORY(DMA, i18n("DMA-Channel"))
#endif
#ifdef INFO_XSERVER_AVAILABLE
CREATE_FACTORY(XServer_and_Video, i18n("X-Server"))
#endif
#ifdef INFO_WAYLAND_AVAILABLE
CREATE_FACTORY(Wayland, i18n("Wayland"))
#endif

K_PLUGIN_FACTORY(KInfoModulesFactory,
#ifdef INFO_IRQ_AVAILABLE
                 registerPlugin<KIRQInfoWidget>(QStringLiteral("irq"));
#endif
#ifdef INFO_IOPORTS_AVAILABLE
                 registerPlugin<KIO_PortsInfoWidget>(QStringLiteral("ioports"));
#endif
#ifdef INFO_DMA_AVAILABLE
                 registerPlugin<KDMAInfoWidget>(QStringLiteral("dma"));
#endif
#ifdef INFO_XSERVER_AVAILABLE
                 registerPlugin<KXServer_and_VideoInfoWidget>(QStringLiteral("xserver"));
#endif
#ifdef INFO_WAYLAND_AVAILABLE
                 registerPlugin<KWaylandInfoWidget>(QStringLiteral("wayland"));
#endif
)

#include "main.moc"
