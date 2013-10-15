/*
 * main.cpp
 *
 * Copyright (c) 1999 Matthias Hoelzer-Kluepfel <hoelzer@kde.org>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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

#include <kcomponentdata.h>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocalizedString>
 
/* we have to include the info.cpp-file, to get the DEFINES about possible properties.
   example: we need the "define INFO_DMA_AVAILABLE" */
#include "info.h"

#include "os_current.h"

class KInfoModulesFactory : public KPluginFactory
{
    public:
        KInfoModulesFactory(const char *componentName);
        static KComponentData componentData();

    private:
        static KPluginFactory *s_instance;
};
KPluginFactory *KInfoModulesFactory::s_instance = 0;

#define CREATE_FACTORY(type, name) \
class K##type##InfoWidget : public KInfoListWidget \
{ \
    public: \
        K##type##InfoWidget(QWidget *parent, const QVariantList &) \
            : KInfoListWidget(name, parent, GetInfo_##type) \
        { \
        } \
}; \

#ifdef INFO_IRQ_AVAILABLE
CREATE_FACTORY(IRQ, i18n("Interrupt"))
#endif
#ifdef INFO_IOPORTS_AVAILABLE
CREATE_FACTORY(IO_Ports, i18n("I/O-Port"))
#endif
#ifdef INFO_SCSI_AVAILABLE
CREATE_FACTORY(SCSI, i18n("SCSI"))
#endif
#ifdef INFO_DMA_AVAILABLE
CREATE_FACTORY(DMA, i18n("DMA-Channel"))
#endif
#ifdef INFO_XSERVER_AVAILABLE
CREATE_FACTORY(XServer_and_Video, i18n("X-Server"))
#endif

KInfoModulesFactory::KInfoModulesFactory(const char *componentName)
    : KPluginFactory(componentName)
{
    s_instance = this;

#ifdef INFO_IRQ_AVAILABLE
    registerPlugin<KIRQInfoWidget>("irq");
#endif
#ifdef INFO_IOPORTS_AVAILABLE
    registerPlugin<KIO_PortsInfoWidget>("ioports");
#endif
#ifdef INFO_SCSI_AVAILABLE
    registerPlugin<KSCSIInfoWidget>("scsi");
#endif
#ifdef INFO_DMA_AVAILABLE
    registerPlugin<KDMAInfoWidget>("dma");
#endif
#ifdef INFO_XSERVER_AVAILABLE
    registerPlugin<KXServer_and_VideoInfoWidget>("xserver");
#endif
}

KComponentData KInfoModulesFactory::componentData()
{
    Q_ASSERT(s_instance);
    return s_instance->componentData();
}

K_EXPORT_PLUGIN(KInfoModulesFactory("kcminfo"))
