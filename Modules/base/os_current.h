/*
    SPDX-FileCopyrightText: 2010 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef OS_CURRENT_H_
#define OS_CURRENT_H_

class QTreeWidget;
class QString;

/* function call-back-prototypes... */

bool GetInfo_IRQ(QTreeWidget *tree);
bool GetInfo_DMA(QTreeWidget *tree);
bool GetInfo_PCI(QTreeWidget *tree);
bool GetInfo_IO_Ports(QTreeWidget *tree);
bool GetInfo_XServer_and_Video(QTreeWidget *tree);
bool GetInfo_Wayland(QTreeWidget *tree);

#ifdef __linux__

#define INFO_IRQ_AVAILABLE
#define INFO_DMA_AVAILABLE
#define INFO_IOPORTS_AVAILABLE
#define INFO_XSERVER_AVAILABLE
#define INFO_WAYLAND_AVAILABLE

/* i18n("Maybe the proc-filesystem is not enabled in Linux-Kernel.") */
#define DEFAULT_ERRORSTRING QString()

#elif defined(__FreeBSD__) || defined(__DragonFly__)

#define INFO_IRQ_AVAILABLE
#define INFO_DMA_AVAILABLE
#define INFO_IOPORTS_AVAILABLE
#define INFO_XSERVER_AVAILABLE

#define DEFAULT_ERRORSTRING i18n("This system may not be completely supported yet.")

#else

#define INFO_IRQ_AVAILABLE
#define INFO_DMA_AVAILABLE
#define INFO_IOPORTS_AVAILABLE
#define INFO_XSERVER_AVAILABLE

#define DEFAULT_ERRORSTRING i18n("This system may not be completely supported yet.")

#endif

#endif /*OS_CURRENT_H_*/
