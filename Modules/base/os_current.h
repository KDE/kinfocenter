
/*
Copyright 2010  Nicolas Ternisien <nicolas.ternisien@gmail.com>

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

#ifndef OS_CURRENT_H_
#define OS_CURRENT_H_

class QTreeWidget;
class QString;

/* function call-back-prototypes... */

bool GetInfo_IRQ(QTreeWidget* tree);
bool GetInfo_DMA(QTreeWidget* tree);
bool GetInfo_PCI(QTreeWidget* tree);
bool GetInfo_IO_Ports(QTreeWidget* tree);
bool GetInfo_XServer_and_Video(QTreeWidget* tree);
bool GetInfo_Wayland(QTreeWidget* tree);


#ifdef __linux__

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
        #define INFO_WAYLAND_AVAILABLE
	
	/* i18n("Maybe the proc-filesystem is not enabled in Linux-Kernel.") */
	#define DEFAULT_ERRORSTRING QString() 


#elif defined(sgi) && sgi

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__FreeBSD__) || defined(__DragonFly__)

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__hpux)

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	
	
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING QString()

#elif defined(__NetBSD__)

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__OpenBSD__)

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__svr4__) && defined(sun)

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(_AIX)


	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__APPLE__)

	//#define INFO_IRQ_AVAILABLE
	//#define INFO_DMA_AVAILABLE
	//#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#else

	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#endif

#endif /*OS_CURRENT_H_*/
