#ifndef OS_CURRENT_H_
#define OS_CURRENT_H_

class QTreeWidget;
class QTreeWidgetItem;
class QString;

/* function call-back-prototypes... */

bool GetInfo_CPU(QTreeWidget* tree);
bool GetInfo_IRQ(QTreeWidget* tree);
bool GetInfo_DMA(QTreeWidget* tree);
bool GetInfo_PCI(QTreeWidget* tree);
bool GetInfo_IO_Ports(QTreeWidget* tree);
bool GetInfo_Sound(QTreeWidget* tree);
bool GetInfo_Devices(QTreeWidget* tree);
bool GetInfo_SCSI(QTreeWidget* tree);
bool GetInfo_Partitions(QTreeWidget* tree);
bool GetInfo_XServer_and_Video(QTreeWidget* tree);


#ifdef __linux__
	
	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	/* i18n("Maybe the proc-filesystem is not enabled in Linux-Kernel.") */
	#define DEFAULT_ERRORSTRING QString() 


#elif defined(sgi) && sgi

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__FreeBSD__) || defined(__DragonFly__)


	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif __hpux

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	
	#define INFO_PARTITIONS_AVAILABLE
	
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING QString()

#elif __NetBSD__

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif __OpenBSD__

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__svr4__) && defined(sun)

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif __svr4__

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif _AIX

	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_CPU_AVAILABLE
	
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#elif defined(__APPLE__)

	#define INFO_CPU_AVAILABLE
	//#define INFO_IRQ_AVAILABLE
	//#define INFO_DMA_AVAILABLE
	//#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE

	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#else

	#define INFO_CPU_AVAILABLE
	#define INFO_IRQ_AVAILABLE
	#define INFO_DMA_AVAILABLE
	#define INFO_IOPORTS_AVAILABLE
	#define INFO_SOUND_AVAILABLE
	#define INFO_DEVICES_AVAILABLE
	#define INFO_SCSI_AVAILABLE
	#define INFO_PARTITIONS_AVAILABLE
	#define INFO_XSERVER_AVAILABLE
	
	#define DEFAULT_ERRORSTRING  i18n("This system may not be completely supported yet.")

#endif

#endif /*OS_CURRENT_H_*/
