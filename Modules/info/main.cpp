/*
 * SPDX-FileCopyrightText: 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *  SPDX-License-Identifier: GPL-2.0-or-later
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

#ifdef INFO_IOPORTS_AVAILABLE
CREATE_FACTORY(IO_Ports, i18n("I/O-Port"))
#endif
#ifdef INFO_DMA_AVAILABLE
CREATE_FACTORY(DMA, i18n("DMA-Channel"))
#endif
#ifdef INFO_XSERVER_AVAILABLE
CREATE_FACTORY(XServer_and_Video, i18n("X-Server"))
#endif

K_PLUGIN_FACTORY(KInfoModulesFactory,
#ifdef INFO_IOPORTS_AVAILABLE
                 registerPlugin<KIO_PortsInfoWidget>(QStringLiteral("ioports"));
#endif
#ifdef INFO_DMA_AVAILABLE
                 registerPlugin<KDMAInfoWidget>(QStringLiteral("dma"));
#endif
#ifdef INFO_XSERVER_AVAILABLE
                 registerPlugin<KXServer_and_VideoInfoWidget>(QStringLiteral("xserver"));
#endif
)

#include "main.moc"
