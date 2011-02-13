/* 	info_sgi.cpp
 
 !!!!! this file will be included by info.cpp !!!!!
 */

/*  all following functions should return true, when the Information 
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

#include <sys/systeminfo.h>

bool GetInfo_IRQ(QListView *) {
	return false;
}

bool GetInfo_DMA(QListView *) {
	return false;
}

bool GetInfo_PCI(QTreeWidget*) {
	return false;
}

bool GetInfo_IO_Ports(QListView *) {
	return false;
}

bool GetInfo_Sound(QListView *) {
	return false;
}

bool GetInfo_Devices(QListView *) {
	return false;
}

bool GetInfo_SCSI(QListView *) {
	return false;
}

bool GetInfo_Partitions(QListView *) {
	return false;
}

bool GetInfo_XServer_and_Video(QListView *lBox) {
	return GetInfo_XServer_Generic(lBox);
}

