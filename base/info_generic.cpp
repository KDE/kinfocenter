/*

 1998 by Helge Deller (deller@gmx.de)
 free source under GPL
 
 !!!!! this file will be included by info.cpp !!!!!
 */

// Default for unsupportet systems


/*  all following functions should return true, when the Information 
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

bool GetInfo_CPU(QTreeWidget*) {
	return false;
}

bool GetInfo_IRQ(QTreeWidget*) {
	return false;
}

bool GetInfo_DMA(QTreeWidget*) {
	return false;
}

bool GetInfo_PCI(QTreeWidget*) {
	return false;
}

bool GetInfo_IO_Ports(QTreeWidget*) {
	return false;
}

bool GetInfo_Sound(QTreeWidget*) {
	return false;
}

bool GetInfo_Devices(QTreeWidget*) {
	return false;
}

bool GetInfo_SCSI(QTreeWidget*) {
	return false;
}

bool GetInfo_Partitions(QTreeWidget*) {
	return false;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
