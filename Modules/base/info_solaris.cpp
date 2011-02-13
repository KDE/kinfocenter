/*
 *  info_solaris.cpp
 *
 *  Torsten Kasch <tk@Genetik.Uni-Bielefeld.DE>
 */

#include "config-infocenter.h"

#include <QTreeWidgetItemIterator>

#include <stdio.h>
#include <stdlib.h>
#include <kstat.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <time.h>

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

bool GetInfo_SCSI(QTreeWidget*) {
	return false;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
