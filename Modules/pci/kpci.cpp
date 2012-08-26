/* Retrieve information about PCI subsystem through libpci library from
 pciutils package. This should be possible on Linux, BSD and AIX.
 
 This code is based on example.c, lspci.c and pci.ids from pciutils.
 
 Device classes, subclasses and programming interfaces are hardcoded
 here, since there are only few of them, and they are important and
 should their names be translated.
 
 Author: Konrad Rzepecki <hannibal@megapolis.pl>
 */
#include "kpci_private.h"

//extern "C" is needed to proper linking with libpci
extern "C" {
#include <pci/pci.h>
}
#include <unistd.h>
#include <sys/types.h> //getuid
#include <ctype.h> //isxdigit
#include <string.h> //memcpy
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFile>

static const QString& getNameById(const id2name *const table, int id) {
	for (int i=0;; i++) {
		if ((table[i].id==id)||(table[i].id==-1)) {
			return table[i].name;
		}//if
	}//while
}//getNameById

static const QString& getNameBy2Id(const id3name *const table, int id, int id2) {
	for (int i=0;; i++) {
		if (((table[i].id==id)&&(table[i].id2==id2))|| ((table[i].id==id)&&(table[i].id2==-1))|| (table[i].id==-1)) {
			return table[i].name;
		}//if
	}//while
}//getNameBy2Id

static const QString& getNameBy3Id(const id4name *const table, int id, int id2, int id3) {
	for (int i=0;; i++) {
		if (((table[i].id==id)&&(table[i].id2==id2)&&(table[i].id3==id3))|| ((table[i].id==id)&&(table[i].id2==id2)&&(table[i].id3==-1))|| ((table[i].id==id)&&(table[i].id2==-1))|| (table[i].id==-1)) {
			return table[i].name;
		}//if
	}//while
}//getNameBy3Id

static QTreeWidgetItem* create(QTreeWidgetItem* parent, const QString& title, const QString& value) {
	QStringList list;
	list << title << value;
	return new QTreeWidgetItem(parent, list);
}

static QTreeWidgetItem* createTitle(QTreeWidgetItem* parent, const QString& title) {
	QStringList list;
	list << title;
	return new QTreeWidgetItem(parent, list);
}

static QTreeWidgetItem* addDeviceClass(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QString value;
	QTreeWidgetItem *localAfter=NULL;
	after=create(parent, i18n("Device Class"), getNameById(devClass, info->cooked.devClass)+value.sprintf(" (0x%02X)", info->cooked.devClass));
	after=create(parent, i18n("Device Subclass"), getNameBy2Id(devSubclass, info->cooked.devClass, info->cooked.devSubClass)+value.sprintf(" (0x%02X)", info->cooked.devSubClass));
	after=create(parent, i18n("Device Programming Interface"), getNameBy3Id(devInterface, info->cooked.devClass, info->cooked.devSubClass, info->cooked.devProgIface.devProgIface)+value.sprintf(" (0x%02X)", info->cooked.devProgIface.devProgIface));
	if ((info->cooked.devClass==0x01)&&(info->cooked.devSubClass==0x01)) { //programming interface for IDE
		localAfter=create(after, i18n("Master IDE Device"), (info->cooked.devProgIface.devProgIface_bits.progIdeMaster ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Secondary programmable indicator"), (info->cooked.devProgIface.devProgIface_bits.progSecProgInd ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Secondary operating mode"), (info->cooked.devProgIface.devProgIface_bits.progSecOperMode ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Primary programmable indicator"), (info->cooked.devProgIface.devProgIface_bits.progPriProgInd ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Primary operating mode"), (info->cooked.devProgIface.devProgIface_bits.progPriOperMode ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	}//if
	return after;
}//addDeviceClass

static QTreeWidgetItem* addVendor(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, pci_access *PCIAccess) {
	char nameBuffer[NAME_BUFFER_SIZE];
	QString line, value, topname;
	short int subvendor=0, subdevice=0;
	bool isVendor=false, isDevice=false, isSub=false;

	memset((void*)nameBuffer, 0, NAME_BUFFER_SIZE);
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_CARDBUS) {
		subvendor=info->cooked.header.header2.cbSubVendor;
		subdevice=info->cooked.header.header2.cbSubDevice;
	}//if
	else {
		subvendor=info->cooked.header.header0.subVendor;
		subdevice=info->cooked.header.header0.subDevice;
	}//else

	//WARNING all pci_lookup_name calls should have 4 extra args for compatibility with older pcilib !
	if (pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_VENDOR, info->cooked.vendor, 0, 0, 0)!=NULL) {
		//		line.setAscii(nameBuffer); //not work, workaround below
		line = QString::fromAscii(pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_VENDOR, info->cooked.vendor, 0, 0, 0));
		if (line.contains("Unknown")==0) {
			isVendor=true;
			topname=line;
			after=create(parent, i18n("Vendor"), line+value.sprintf(" (0x%04X)", info->cooked.vendor));
			if (pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_DEVICE, info->cooked.vendor, info->cooked.device, 0, 0)!=NULL) {
				//				line.setAscii(nameBuffer); //not work, workaround below
				line = QString::fromAscii(pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_DEVICE, info->cooked.vendor, info->cooked.device, 0, 0));
				if (line.contains("Unknown")==0) {
					isDevice=true;
					topname+=QString(" ")+line;
					after=create(parent, i18n("Device"), line+value.sprintf(" (0x%04X)", info->cooked.device));
					if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE) {
						isSub=true;
					}//if
					else if (pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_DEVICE|PCI_LOOKUP_SUBSYSTEM, info->cooked.vendor, info->cooked.device, subvendor, subdevice)!=NULL) {
						//						line.setAscii(nameBuffer); //not work, workaround below
						line = QString::fromAscii(pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_DEVICE|PCI_LOOKUP_SUBSYSTEM, info->cooked.vendor, info->cooked.device, subvendor, subdevice));
						if (line.contains("Unknown")==0) {
							isSub=true;
							after=create(parent, i18n("Subsystem"), line+value.sprintf(" (0x%04X:0x%04X)", subvendor, subdevice));
						}//if
					}//eliif
				}//if
			}//iif
		}//if
	}//if
	if (!isVendor) {
		after=create(parent, i18n("Vendor"), i18nc(strCtxt, strUnknown)+value.sprintf(" (0x%04X)", info->cooked.vendor));
		if (!isDevice) {
			after=create(parent, i18n("Device"), i18nc(strCtxt, strUnknown)+value.sprintf(" (0x%04X)", info->cooked.device));
		}//if
		topname=i18nc(strCtxt, strUnknown);
	}//if
	if ((!isSub)&&(info->cooked.headerType.headerType_bits.headerType!=PCI_HEADER_TYPE_BRIDGE)) { //if entire subsytem was not found, search at least for subvendor
		if (pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_VENDOR, subvendor, 0, 0, 0)!=NULL) {
			//			line.setAscii(nameBuffer); //not work, workaround below
			line = QString::fromAscii(pci_lookup_name(PCIAccess, nameBuffer, NAME_BUFFER_SIZE, PCI_LOOKUP_VENDOR, subvendor, 0, 0, 0));
			if (line.contains("Unknown")==0) {
				after=create(parent, i18n("Subsystem"), line+i18n(" - device:")+value.sprintf(" 0x%04X (0x%04X:0x%04X)", subdevice, subvendor, subdevice));
			}//if
			else {
				after=create(parent, i18n("Subsystem"), i18nc(strCtxt, strUnknown)+value.sprintf(" (0x%04X:0x%04X)", subvendor, subdevice));
			}//else
		}//if
		else {
			after=create(parent, i18n("Subsystem"), i18nc(strCtxt, strUnknown)+value.sprintf(" (0x%04X:0x%04X)", subvendor, subdevice));
		}//else
	}//if
	parent->setText(1, topname);
	return after;
}//addVendor

static QTreeWidgetItem* addInterrupt(QTreeWidgetItem *parent, QTreeWidgetItem *after, int irq, int pin) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if ((irq!=0)||(pin!=0)) {
		after=createTitle(parent, i18n("Interrupt"));
		localAfter=create(after, i18n("IRQ"), value.sprintf("%i", irq));
		localAfter=create(after, i18n("Pin"), value.sprintf("%c", (pin==0 ? '?' : 'A'-1+pin)));
	}//if
	return after;
}//addInterrupt

static QTreeWidgetItem* addControl(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	after=create(parent, i18n("Control"), value.sprintf("0x%04X", info->cooked.command.command));
	localAfter=create(after, i18n("Response in I/O space"), (info->cooked.command.command_bits.comIo ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Response in memory space"), (info->cooked.command.command_bits.comMemory ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Bus mastering"), (info->cooked.command.command_bits.comMaster ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Response to special cycles"), (info->cooked.command.command_bits.comSpecial ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Memory write and invalidate"), (info->cooked.command.command_bits.comInvalidate ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Palette snooping"), (info->cooked.command.command_bits.comVgaPalette ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Parity checking"), (info->cooked.command.command_bits.comParity ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Address/data stepping"), (info->cooked.command.command_bits.comWait ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("System error"), (info->cooked.command.command_bits.comSerr ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Back-to-back writes"), (info->cooked.command.command_bits.comFastBack ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Interrupt"), (info->cooked.command.command_bits.comInterrupt ? i18nc(strCtxt, strDisabled) : i18nc(strCtxt, strEnabled))); //reverse order is intentional
	return after;
}//addControl

static QTreeWidgetItem* addStatus(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	after=create(parent, i18n("Status"), value.sprintf("0x%04X", info->cooked.status.status));
	localAfter=create(after, i18n("Interrupt status"), (info->cooked.status.status_bits.statCapList ? i18nc(strCtxt, strEnabled) : i18nc(strCtxt, strDisabled)));
	localAfter=create(after, i18n("Capability list"), (info->cooked.status.status_bits.statCapList ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("66 MHz PCI 2.1 bus"), (info->cooked.status.status_bits.stat66MHz ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("User-definable features"), (info->cooked.status.status_bits.statUdf ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Accept fast back-to-back"), (info->cooked.status.status_bits.statFastBack ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Data parity error"), (info->cooked.status.status_bits.statParity ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Device selection timing"), getNameById(devSel, info->cooked.status.status_bits.statDevsel));
	localAfter=create(after, i18n("Signaled target abort"), (info->cooked.status.status_bits.statSigTargetAbort ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Received target abort"), (info->cooked.status.status_bits.statRecTargetAbort ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Received master abort"), (info->cooked.status.status_bits.statRecMasterAbort ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Signaled system error"), (info->cooked.status.status_bits.statSigSystemError ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	localAfter=create(after, i18n("Parity error"), (info->cooked.status.status_bits.statDetectedParity ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	return after;
}//addStatus

static QTreeWidgetItem* addLatency(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	after=create(parent, i18n("Latency"), value.sprintf("%u", info->cooked.latencyTimer));
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_NORMAL) {
		if (info->cooked.header.header0.minGnt==0) {
			localAfter=create(after, i18n("MIN_GNT"), i18n("No major requirements (0x00)"));
		}//if
		else {
			localAfter=create(after, i18n("MIN_GNT"), value.sprintf("%u ns (0x%02X)", info->cooked.header.header0.minGnt*250, info->cooked.header.header0.minGnt));
		}//else
		if (info->cooked.header.header0.maxLat==0) {
			localAfter=create(after, i18n("MAX_LAT"), i18n("No major requirements (0x00)"));
		}//if
		else {
			localAfter=create(after, i18n("MAX_LAT"), value.sprintf("%u ns (0x%02X)", info->cooked.header.header0.maxLat*250, info->cooked.header.header0.maxLat));
		}//else
	}//if
	return after;
}//addLatency

static QTreeWidgetItem* addHeaderType(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	after=create(parent, i18n("Header"),value.sprintf("0x%02X",info->cooked.headerType.headerTypeFull));
	localAfter=create(after, i18n("Type"),getNameById(headerType,info->cooked.headerType.headerType_bits.headerType)+value.sprintf(" (0x%02X)",info->cooked.headerType.headerType_bits.headerType));
	localAfter=create(after, i18n("Multifunctional"),(info->cooked.headerType.headerType_bits.multifunctional?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
	return after;
}//addHeaderType

static QTreeWidgetItem* addBist(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	after=create(parent, i18n("Build-in self test"), value.sprintf("0x%02X", info->cooked.bist.bist));
	localAfter=create(after, i18n("BIST Capable"), (info->cooked.bist.bist_bits.bistCapable ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
	if (info->cooked.bist.bist_bits.bistCapable==1) {
		localAfter=create(after, i18n("BIST Start"), (info->cooked.bist.bist_bits.bistStart ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Completion code"), value.sprintf("0x%01X", info->cooked.bist.bist_bits.bistCode));
	}//if
	return after;
}//addBist

static QTreeWidgetItem* addSize(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciaddr_t size) {
	if (size<0x400) {
		after=create(parent, i18n("Size"), QString("%1 B").arg(static_cast<unsigned long>(size)));
	}//if
	else if (size<0x100000) {
		after=create(parent, i18n("Size"), QString("%1 kiB").arg(static_cast<unsigned long>(size/0x400)));
	}//elif
	else if (size<0x40000000) {
		after=create(parent, i18n("Size"), QString("%1 MiB").arg(static_cast<unsigned long>(size/0x100000)));
	}//elif

#ifdef HAVE_PCIADDR_T64

	else if (size<0x10000000000LL) {
		after=create(parent, i18n("Size"),QString("%1 GiB").arg(static_cast<unsigned long>(size/0x40000000)));
	}//elif
	else if (size<0x4000000000000LL) {
		after=create(parent, i18n("Size"),QString("%1 PiB").arg(static_cast<unsigned long>(size/0x10000000000LL)));
	}//elif
	else if (size<0x1000000000000000LL) {
		after=create(parent, i18n("Size"),QString("%1 EiB").arg(static_cast<unsigned long>(size/0x4000000000000LL)));
	}//elif

#else //HAVE_PCIADDR_T64
	else {
		after=create(parent, i18n("Size"), QString("%1 GiB").arg(static_cast<unsigned long>(size/0x40000000)));
	}//else

#endif //HAVE_PCIADDR_T64
	return after;
}//addSize

static QTreeWidgetItem* addMapping(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, pci_dev* PCIDevice) {
	QTreeWidgetItem *localAfter=NULL;
	QTreeWidgetItem *topLocalAfter=NULL;
	QString value;
	bool is64b=false;
	after=createTitle(parent, i18n("Address mappings"));
	for (int i=0; i<6; i++) {
		if (((info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE)&&(i>1))||((info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_CARDBUS)&&(i>0))) {
			break;
		}//if
		if (is64b) { //skip one range
			continue;
		}//if
		topLocalAfter=createTitle(after, i18n("Mapping %1", i));
		localAfter=create(topLocalAfter, i18n("Space"), (info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap ? i18n("I/O") : i18n("Memory")));
		if (info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap==0) { //memory only
			localAfter=create(topLocalAfter, i18n("Type"), getNameById(mappingType, info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressType));
			localAfter=create(topLocalAfter, i18n("Prefetchable"), (info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressPref ? i18nc(strCtxt, strYes) : i18nc(strCtxt, strNo)));
		}//if
		if (is64b) { //there is no long long support in Qt so we need compose value
			topLocalAfter->setText(1, value.sprintf("0x%08X%08X", info->cooked.header.header0.mapping[i+1].baseAddress, info->cooked.header.header0.mapping[i].baseAddress));
			if (info->cooked.header.header0.mapping[i+1].baseAddress==0) { //no top 4 bytes
				if ((info->cooked.header.header0.mapping[i].baseAddress&(~(info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap ? 0x3 : 0xF)))==0) { //no address at all
					localAfter=create(topLocalAfter, i18n("Address"), i18nc("unassigned address", "Unassigned"));
					localAfter=create(topLocalAfter, i18n("Size"), i18nc("unassigned size", "Unassigned"));
				}//if
				else {
					localAfter=create(topLocalAfter, i18n("Address"), value.sprintf("0x%X", (info->cooked.header.header0.mapping[i].baseAddress&(~(info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap ? 0x3 : 0xF)))));
					localAfter=addSize(topLocalAfter, localAfter, PCIDevice->size[i]);
				}//else
			}//if
			else {
				localAfter=create(topLocalAfter, i18n("Address"), value.sprintf("0x%X%08X", info->cooked.header.header0.mapping[i+1].baseAddress, (~(info->cooked.header.header0.mapping[i].baseAddress&(info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap ? 0x3 : 0xF)))));
				localAfter=addSize(topLocalAfter, localAfter, PCIDevice->size[i]);
			}//else
		}//if
		else {
			topLocalAfter->setText(1, value.sprintf("0x%08X", info->cooked.header.header0.mapping[i].baseAddress));
			if ((info->cooked.header.header0.mapping[i].baseAddress&(~(info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap ? 0x3 : 0xF)))==0) { //no address at all
				localAfter=create(topLocalAfter, i18n("Address"), i18nc("unassigned address", "Unassigned"));
				localAfter=create(topLocalAfter, i18n("Size"), i18nc("unassigned size", "Unassigned"));
			}//if
			else {
				localAfter=create(topLocalAfter, i18n("Address"), value.sprintf("0x%X", (info->cooked.header.header0.mapping[i].baseAddress&(~(info->cooked.header.header0.mapping[i].baseAddress_bits.baseAddressMap ? 0x3 : 0xF)))));
				localAfter=addSize(topLocalAfter, localAfter, PCIDevice->size[i]);
			}//else
		}//else
	}//for
	return after;
}//addMapping

static QTreeWidgetItem* addBus(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE) {
		after=createTitle(parent, i18n("Bus"));
		localAfter=create(after, i18n("Primary bus number"), value.sprintf("0x%02X", info->cooked.header.header1.primaryBus));
		localAfter=create(after, i18n("Secondary bus number"), value.sprintf("0x%02X", info->cooked.header.header1.secondaryBus));
		localAfter=create(after, i18n("Subordinate bus number"), value.sprintf("0x%02X", info->cooked.header.header1.subordinateBus));
		localAfter=create(after, i18n("Secondary latency timer"), value.sprintf("0x%02X", info->cooked.header.header1.secLatencyTimer));
	}//if
	else if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_CARDBUS) { //should be checked
		after=createTitle(parent, i18n("Bus"));
		localAfter=create(after, i18n("Primary bus number"), value.sprintf("0x%02X", info->cooked.header.header2.cbPrimaryBus));
		localAfter=create(after, i18n("CardBus number"), value.sprintf("0x%02X", info->cooked.header.header2.cbCardBus));
		localAfter=create(after, i18n("Subordinate bus number"), value.sprintf("0x%02X", info->cooked.header.header2.cbSubordinateBus));
		localAfter=create(after, i18n("CardBus latency timer"), value.sprintf("0x%02X", info->cooked.header.header2.cbLatencyTimer));
	}//elif
	return after;
}//addBus

static QTreeWidgetItem* addSecStatus(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE) {
		after=create(parent, i18n("Secondary status"), value.sprintf("0x%04X", info->cooked.header.header1.secStatus.secStatus));
		localAfter=create(after, i18n("Interrupt status"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatCapList?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Capability list"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatCapList?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("66 MHz PCI 2.1 bus"),(info->cooked.header.header1.secStatus.secStatus_bits.secStat66MHz?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("User-definable features"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatUdf?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Accept fast back-to-back"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatFastBack?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Data parity error"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatParity?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Device selection timing"),getNameById(devSel,info->cooked.header.header1.secStatus.secStatus_bits.secStatDevsel));
		localAfter=create(after, i18n("Signaled target abort"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatSigTargetAbort?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Received target abort"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatRecTargetAbort?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Received master abort"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatRecMasterAbort?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Signaled system error"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatSigSystemError?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Parity error"),(info->cooked.header.header1.secStatus.secStatus_bits.secStatDetectedParity?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
	}//if		
	else if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_CARDBUS) { //should be checked
		after=create(parent,i18n("Secondary status"),value.sprintf("0x%04X",info->cooked.header.header2.cbSecStatus.cbSecStatus));
		localAfter=create(after, i18n("Interrupt status"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatCapList?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Capability list"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatCapList?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("66 MHz PCI 2.1 bus"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStat66MHz?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("User-definable features"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatUdf?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Accept fast back-to-back"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatFastBack?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Data parity error"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatParity?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Device selection timing"),getNameById(devSel,info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatDevsel));
		localAfter=create(after, i18n("Signaled target abort"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatSigTargetAbort?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Received target abort"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatRecTargetAbort?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Received master abort"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatRecMasterAbort?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Signaled system error"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatSigSystemError?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Parity error"),(info->cooked.header.header2.cbSecStatus.cbSecStatus_bits.cbSecStatDetectedParity?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
	}//elif		
	return after;
}//addSecStatus

static QTreeWidgetItem* addBridgeBehind(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE) {
		after=createTitle(parent, i18n("I/O behind bridge"));
		localAfter=create(after, i18n("32-bit"),(info->cooked.header.header1.ioBase.ioBase_bits.ioBaseType?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		if (info->cooked.header.header1.ioBase.ioBase_bits.ioBaseType==0) {
			localAfter=create(after, i18n("Base"),value.sprintf("0x%04X",info->cooked.header.header1.ioBase.ioBase&0xFFF0));
			localAfter=create(after, i18n("Limit"),value.sprintf("0x%04X",info->cooked.header.header1.ioLimit|0x0F));
		}//if
		else {
			localAfter=create(after, i18n("Base"),value.sprintf("0x%04X%04X",info->cooked.header.header1.ioBaseUpper16,info->cooked.header.header1.ioBase.ioBase&0xFFF0));
			localAfter=create(after, i18n("Limit"),value.sprintf("0x%04X%04X",info->cooked.header.header1.ioLimitUpper16,info->cooked.header.header1.ioLimit|0x0F));
		}//else
		after=createTitle(parent, i18n("Memory behind bridge"));
		localAfter=create(after, i18n("Base"),value.sprintf("0x%08X",(info->cooked.header.header1.memoryBase.memoryBase<<16)&0xFFFFFFF0));
		localAfter=create(after, i18n("Limit"),value.sprintf("0x%08X",(info->cooked.header.header1.memoryLimit<<16)|0x0FFFFF));
		after=createTitle(parent, i18n("Prefetchable memory behind bridge"));
		localAfter=create(after, i18n("64-bit"),(info->cooked.header.header1.ioBase.ioBase_bits.ioBaseType?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		if (info->cooked.header.header1.ioBase.ioBase_bits.ioBaseType==0) {
			localAfter=create(after, i18n("Base"),value.sprintf("0x%08X",(info->cooked.header.header1.prefMemoryBase.prefMemoryBase<<16)&0xFFFFFFF0));
			localAfter=create(after, i18n("Limit"),value.sprintf("0x%08X",(info->cooked.header.header1.prefMemoryLimit<<16)|0x0FFFFF));
		}//if
		else {
			localAfter=create(after, i18n("Base"),value.sprintf("0x%08X%08X",info->cooked.header.header1.prefBaseUpper32,(info->cooked.header.header1.prefMemoryBase.prefMemoryBase<<16)&0xFFFFFFF0));
			localAfter=create(after, i18n("Limit"),value.sprintf("0x%0x8X%08X",info->cooked.header.header1.prefLimitUpper32,(info->cooked.header.header1.prefMemoryLimit<<16)|0x0FFFFF));
		}//else
	}//if		
	return after;
}//addBridgeBechind

static QTreeWidgetItem* addBridgeControl(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE) {
		after=create(parent, i18n("Bridge control"),value.sprintf("0x%04X",info->cooked.header.header1.bridgeControl.bridgeControl));
		localAfter=create(after, i18n("Secondary parity checking"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlParity?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Secondary system error"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlSerr?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("ISA ports forwarding"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlIsa?i18nc(strCtxt, strDisabled):i18nc(strCtxt, strEnabled))); //reverse order is intentional
		localAfter=create(after, i18n("VGA forwarding"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlVga?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Master abort"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlMasterAbort?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Secondary bus reset"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlBusReset?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Secondary back-to-back writes"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlFastBack?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Primary discard timer counts"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlPriDisTim?i18n("2e10 PCI clocks"):i18n("2e15 PCI clocks")));
		localAfter=create(after, i18n("Secondary discard timer counts"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlSecDisTim?i18n("2e10 PCI clocks"):i18n("2e15 PCI clocks")));
		localAfter=create(after, i18n("Discard timer error"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlDisTimStat?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Discard timer system error"),(info->cooked.header.header1.bridgeControl.bridgeControl_bits.bridgeControlDisTimeSerr?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
	}//if		
	return after;
}//addBridgeControl

static QTreeWidgetItem* addRom(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, pci_dev* PCIDevice) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if ((info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_NORMAL)||(info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE)) {
		after=createTitle(parent, i18n("Expansion ROM"));
		localAfter=create(after, i18n("Status"),(info->cooked.header.header0.romAddress.romAddress_bits.romEnabled?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		if (PCIDevice->rom_base_addr==0) { //no address at all
			localAfter=create(after, i18n("Address"),i18nc("unassigned address", "Unassigned"));
			localAfter=create(after, i18n("Size"),i18nc("unassigned size", "Unassigned"));
		}//if
		else {
			localAfter=create(after, i18n("Address"),value.sprintf("0x%X",static_cast<unsigned>(PCIDevice->rom_base_addr)));
			localAfter=addSize(after, localAfter, PCIDevice->rom_size);
		}//else
	}//if
	return after;
}//addRom

static QTreeWidgetItem* addCardbusResource(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QTreeWidgetItem *topLocalAfter=NULL;
	QString value;
	int pref=0;
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_CARDBUS) {
		after=createTitle(parent, i18n("Memory windows"));
		for (int i=0; i<2; i++) {
			pref=(i ? info->cooked.header.header2.cbControl.cbControl_bits.cbControlPref1 : info->cooked.header.header2.cbControl.cbControl_bits.cbControlPref0);
			topLocalAfter=createTitle(after, i18n("Window %1", i));
			localAfter=create(topLocalAfter, i18n("Prefetchable"),(pref?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
			localAfter=create(topLocalAfter, i18n("Base"),value.sprintf("0x%08X",info->cooked.header.header2.cbMemory[i].cbMemoryBase));
			localAfter=create(topLocalAfter, i18n("Limit"),value.sprintf("0x%08X",info->cooked.header.header2.cbMemory[i].cbMemoryLimit));
		}//for
		after=createTitle(parent, i18n("I/O windows"));
		for (int i=0; i<2; i++) {
			topLocalAfter=createTitle(after, i18n("Window %1", i));
			localAfter=create(topLocalAfter, i18n("Type"),(info->cooked.header.header2.cbIo[i].cbIoBase.cbIoBase_bits.cbIoBaseType?i18n("32-bit"):i18n("16-bit")));
			if (info->cooked.header.header2.cbIo[i].cbIoBase.cbIoBase_bits.cbIoBaseType==1) {
				localAfter=create(topLocalAfter, i18n("Base"),value.sprintf("0x%08X",info->cooked.header.header2.cbIo[i].cbIoBase.cbIoBase&0xFFFFFFFC));
				localAfter=create(topLocalAfter, i18n("Limit"),value.sprintf("0x%08X",info->cooked.header.header2.cbIo[i].cbIoLimit|0x03));
			}//if
			else {
				localAfter=create(topLocalAfter, i18n("Base"),value.sprintf("0x%04X",info->cooked.header.header2.cbIo[i].cbIoBase.cbIoBase&0xFFFC));
				localAfter=create(topLocalAfter, i18n("Limit"),value.sprintf("0x%04X",(info->cooked.header.header2.cbIo[i].cbIoLimit&0xFFFF)|0x03));
			}//else
		}//for
		after=create(parent, i18n("16-bit legacy interface ports"),value.sprintf("0x%04X",info->cooked.header.header2.cbLegacyModeBase));
	}//if
	return after;
}//addCardbusResource

static QTreeWidgetItem* addCardbusControl(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	if (info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_CARDBUS) {
		after=create(parent, i18n("CardBus control"),value.sprintf("0x%04X",info->cooked.header.header2.cbControl.cbControl));
		localAfter=create(after, i18n("Secondary parity checking"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlParity?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Secondary system error"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlSerr?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("ISA ports forwarding"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlIsa?i18nc(strCtxt, strDisabled):i18nc(strCtxt, strEnabled))); //reverse order is intentional
		localAfter=create(after, i18n("VGA forwarding"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlVga?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Master abort"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlMasterAbort?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Interrupts for 16-bit cards"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControl16Int?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Window 0 prefetchable memory"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlPref0?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Window 1 prefetchable memory"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlPref1?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Post writes"),(info->cooked.header.header2.cbControl.cbControl_bits.cbControlPostWrites?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
	}//if		
	return after;
}//addCardbusControl

static QTreeWidgetItem* addRaw(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QString value, temp;
	after=createTitle(parent, i18n("Raw PCI config space"));
	for (int i=0; i<(getuid()==0 ? 16 : 4); i++) {
		for (int j=0; j<16; j++) {
			if (j!=0) {
				value+=temp.sprintf(" %02X", info->raw[i*16+j]);
			}//if
			else {
				value.sprintf("%02X", info->raw[i*16+j]);
			}//if
		}//for
		localAfter=create(after, temp.sprintf("0x%02X:",i*16),value);
	}//for
	return after;
}//addRaw

static QTreeWidgetItem* addCapsPm(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, int offset) {
	QTreeWidgetItem *localAfter=NULL;
	QTreeWidgetItem *subLocalAfter=NULL;
	QString value;
	pmInfo infoPm;
	if ((offset+2+sizeof(pmInfo))<256) {
		memcpy(reinterpret_cast<void*>(&infoPm.raw[0]), reinterpret_cast<void*>(&info->raw[offset+2]), sizeof(pmInfo));
		after=create(parent, i18n("Capabilities"),value.sprintf("0x%04X",infoPm.cooked.caps.caps));
		localAfter=create(after, i18n("Version"),QString::number(infoPm.cooked.caps.caps_bits.capsVer));
		localAfter=create(after, i18n("Clock required for PME generation"),(infoPm.cooked.caps.caps_bits.capsClock?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Device-specific initialization required"),(infoPm.cooked.caps.caps_bits.capsDsi?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		localAfter=create(after, i18n("Maximum auxiliary current required in D3 cold"),getNameById(auxCur,infoPm.cooked.caps.caps_bits.capsAuxCur));
		localAfter=create(after, i18n("D1 support"),(infoPm.cooked.caps.caps_bits.capsD1Supp?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("D2 support"),(infoPm.cooked.caps.caps_bits.capsD2Supp?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=createTitle(after, i18n("Power management events"));
		subLocalAfter=create(localAfter, i18n("D0"),(infoPm.cooked.caps.caps_bits.capsPmeD0?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		subLocalAfter=create(localAfter, i18n("D1"),(infoPm.cooked.caps.caps_bits.capsPmeD1?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		subLocalAfter=create(localAfter, i18n("D2"),(infoPm.cooked.caps.caps_bits.capsPmeD2?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		subLocalAfter=create(localAfter, i18n("D3 hot"),(infoPm.cooked.caps.caps_bits.capsPmeD3hot?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		subLocalAfter=create(localAfter, i18n("D3 cold"),(infoPm.cooked.caps.caps_bits.capsPmeD3cold?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=NULL;
		after=create(parent, i18n("Status"),value.sprintf("0x%04X",infoPm.cooked.status.status));
		localAfter=create(after, i18n("Power state"),getNameById(powerState,infoPm.cooked.status.status_bits.statPower));
		localAfter=create(after, i18n("Power management"),(infoPm.cooked.status.status_bits.statPme?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Data select"),QString::number(infoPm.cooked.status.status_bits.statDataSelect));
		localAfter=create(after, i18n("Data scale"),QString::number(infoPm.cooked.status.status_bits.statDataScale));
		localAfter=create(after, i18n("Power management status"),(infoPm.cooked.status.status_bits.statPmeStat?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		if ((info->cooked.devClass==0x06)&&(info->cooked.devSubClass==0x04)) { //PCI bridge
			subLocalAfter=NULL;
			localAfter=create(after, i18n("Bridge status"),value.sprintf("0x%02X",infoPm.cooked.statusBridge.statusBridge));
			subLocalAfter=create(localAfter, i18n("Secondary bus state in D3 hot"),(infoPm.cooked.statusBridge.statusBridge_bits.statBridgeBx?i18n("B2"):i18n("B3")));
			subLocalAfter=create(localAfter, i18n("Secondary bus power & clock control"),(infoPm.cooked.statusBridge.statusBridge_bits.statBridgeClock?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		}//if
	}//if
	after=create(parent, i18n("Data"),value.sprintf("0x%02X",infoPm.cooked.data));
	return after;
}//addCapsPm

static QTreeWidgetItem* addCapsAgp(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, int offset) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	agpInfo infoAgp;
	int i, cycleSize;
	if ((offset+2+sizeof(agpInfo))<256) {
		memcpy(reinterpret_cast<void*>(&infoAgp.raw[0]), reinterpret_cast<void*>(&info->raw[offset+2]), sizeof(agpInfo));
		//		after=create(parent, i18n("Revision"),value.sprintf("%i.%i",infoAgp.revMaior,infoAgp.revMinor));
		after=create(parent, i18n("Revision"),QString("%1.%2").arg(infoAgp.cooked.revision.revMaior).arg(infoAgp.cooked.revision.revMinor));
		after=create(parent, i18n("Status"),value.sprintf("0x%08X",infoAgp.cooked.status.status));
		localAfter=create(after, i18n("Rate"),getNameById(agpRate,infoAgp.cooked.status.status_bits0.statusEnhRate));
		localAfter=create(after, i18n("AGP 3.0 mode"),(infoAgp.cooked.status.status_bits1.statusMode?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Fast Writes"),(infoAgp.cooked.status.status_bits1.statusFastWrite?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Address over 4 GiB"),(infoAgp.cooked.status.status_bits1.statusOver4gb?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		if (infoAgp.cooked.status.status_bits1.statusMode==1) {
			localAfter=create(after, i18n("Translation of host processor access"),(infoAgp.cooked.status.status_bits1.statusHtrans?i18nc(strCtxt, strDisabled):i18nc(strCtxt, strEnabled))); //reverse order is intentional
			localAfter=create(after, i18n("64-bit GART"),(infoAgp.cooked.status.status_bits1.statusGart64b?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
			localAfter=create(after, i18n("Cache Coherency"),(infoAgp.cooked.status.status_bits1.statusItaCoh?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		}//if
		localAfter=create(after, i18n("Side-band addressing"),(infoAgp.cooked.status.status_bits1.statusSba?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		if (infoAgp.cooked.status.status_bits1.statusMode==1) {
			localAfter=create(after, i18n("Calibrating cycle"),getNameById(calCycle,infoAgp.cooked.status.status_bits1.statusCalCycle));
			for (i=0, cycleSize=1; i<(infoAgp.cooked.status.status_bits1.statusOptReqSize+4); i++) {
				cycleSize*=2;
			}//for
			localAfter=create(after, i18n("Optimum asynchronous request size"),value.sprintf("%i (0x%02X)",cycleSize,infoAgp.cooked.status.status_bits1.statusOptReqSize));
			localAfter=create(after, i18n("Isochronous transactions"),(infoAgp.cooked.status.status_bits1.statusIsochSupp?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		}//if
		localAfter=create(after, i18n("Maximum number of AGP command"),value.sprintf("%i (0x%02X)",infoAgp.cooked.status.status_bits1.statusReq+1,infoAgp.cooked.status.status_bits1.statusReq));
		localAfter=NULL;
		after=create(parent, i18n("Configuration"),value.sprintf("0x%08X",infoAgp.cooked.config));
		localAfter=create(after, i18n("Rate"),getNameById(agpRate,infoAgp.cooked.config.config_bits0.configEnhRate));
		localAfter=create(after, i18n("Fast Writes"),(infoAgp.cooked.config.config_bits1.configFastWrite?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		if (infoAgp.cooked.status.status_bits1.statusMode==1) {
			localAfter=create(after, i18n("Address over 4 GiB"),(infoAgp.cooked.config.config_bits1.configOver4gb?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
			localAfter=create(after, i18n("64-bit GART"),(infoAgp.cooked.config.config_bits1.configGart64b?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		}//if
		localAfter=create(after, i18n("AGP"),(infoAgp.cooked.config.config_bits1.configAgp?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Side-band addressing"),(infoAgp.cooked.config.config_bits1.configSba?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		if (infoAgp.cooked.status.status_bits1.statusMode==1) {
			localAfter=create(after, i18n("Calibrating cycle"),getNameById(calCycle,infoAgp.cooked.config.config_bits1.configCalCycle));
			for (i=0, cycleSize=1; i<(infoAgp.cooked.config.config_bits1.configOptReqSize+4); i++) {
				cycleSize*=2;
			}//for
			localAfter=create(after, i18n("Optimum asynchronous request size"),value.sprintf("%i (0x%02X)",cycleSize,infoAgp.cooked.config.config_bits1.configOptReqSize));
		}//if
		localAfter=create(after, i18n("Maximum number of AGP command"),value.sprintf("%i (0x%02X)",infoAgp.cooked.config.config_bits1.configReq+1,infoAgp.cooked.config.config_bits1.configReq));
	}//if
	return after;
}//addCapsAgp

static QTreeWidgetItem* addCapsVpd(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, int offset) {
	QString value;
	vpdInfo infoVpd;
	if ((offset+2+sizeof(vpdInfo))<256) {
		memcpy(reinterpret_cast<void*>(&infoVpd.raw[0]), reinterpret_cast<void*>(&info->raw[offset+2]), sizeof(vpdInfo));
		after=create(parent, i18n("Data address"),value.sprintf("0x%04X",infoVpd.cooked.vpdAddress));
		after=create(parent, i18n("Transfer completed"),(infoVpd.cooked.vpdTransfer?i18nc(strCtxt, strYes):i18nc(strCtxt, strNo)));
		after=create(parent, i18n("Data"),value.sprintf("0x%08X",infoVpd.cooked.vpdData));
	}//if
	return after;
}//addCapsVpd

static QTreeWidgetItem* addCapsMsi(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, int offset) {
	QTreeWidgetItem *localAfter=NULL;
	QString value;
	msiInfo infoMsi;
	int size=10;
	if ((offset+4)<256) { //copy control only (for now)
		memcpy(reinterpret_cast<void*>(&infoMsi.raw[0]), reinterpret_cast<void*>(&info->raw[offset+2]), 2);
		after=create(parent, i18n("Message control"),value.sprintf("0x%04X",infoMsi.cooked.msiControl.msiControl));
		localAfter=create(after, i18n("Message signaled interrupts"),(infoMsi.cooked.msiControl.msiControl_bits.msiEnable?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Multiple message capable"),getNameById(multipleMessage,infoMsi.cooked.msiControl.msiControl_bits.msiMmCapable));
		localAfter=create(after, i18n("Multiple message enable"),getNameById(multipleMessage,infoMsi.cooked.msiControl.msiControl_bits.msiMmEnable));
		localAfter=create(after, i18n("64-bit address"),(infoMsi.cooked.msiControl.msiControl_bits.msi64bit?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		localAfter=create(after, i18n("Per vector masking"),(infoMsi.cooked.msiControl.msiControl_bits.msiPerVector?i18nc(strCtxt, strEnabled):i18nc(strCtxt, strDisabled)));
		size+=(infoMsi.cooked.msiControl.msiControl_bits.msi64bit ? 4 : 0)+(infoMsi.cooked.msiControl.msiControl_bits.msiPerVector ? 8 : 0);
		if ((offset+size)<256) { //copy all MSI data
			memcpy(reinterpret_cast<void*>(&infoMsi.raw[0]), reinterpret_cast<void*>(&info->raw[offset+size]), 2);
			if (infoMsi.cooked.msiControl.msiControl_bits.msi64bit==1) {
				after=create(parent, i18n("Address"),value.sprintf("0x%08X%08X",infoMsi.cooked.payload.address.msiUpperAddress,infoMsi.cooked.msiAddress));
				after=create(parent, i18n("Data"),value.sprintf("0x%04X",infoMsi.cooked.payload.address.msiData64));
				if (infoMsi.cooked.msiControl.msiControl_bits.msiPerVector==1) {
					after=create(parent, i18n("Mask"),value.sprintf("0x%08X",infoMsi.cooked.payload.address.msiMask64));
					after=create(parent, i18n("Pending"),value.sprintf("0x%08X",infoMsi.cooked.payload.address.msiPending64));
				}//if
			}//if
			else {
				after=create(parent, i18n("Address"),value.sprintf("0x%08X",infoMsi.cooked.msiAddress));
				after=create(parent, i18n("Data"),value.sprintf("0x%04X",infoMsi.cooked.payload.data.msiData));
				if (infoMsi.cooked.msiControl.msiControl_bits.msiPerVector==1) {
					after=create(parent, i18n("Mask"),value.sprintf("0x%08X",infoMsi.cooked.payload.data.msiMask));
					after=create(parent, i18n("Pending"),value.sprintf("0x%08X",infoMsi.cooked.payload.data.msiPending));
				}//if
			}//else
		}//if
	}//if
	return after;
}//addCapsMsi

static QTreeWidgetItem* addCapsVendor(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info, int offset) {
	QString value, temp;
	after=create(parent, i18n("Length"),value.sprintf("0x%02X",info->raw[offset+2]));
	if ((offset+3)<256) { //check partial size
		if (info->raw[offset+2]<=2) {
			after=create(parent, i18n("Data"),i18nc("no data", "None"));
		}//if
		else {
			if ((offset+info->raw[offset+2])<256) { //check full size
				for (int i=3; i<(info->raw[offset+2]); i++) {
					if (i!=3) {
						value+=temp.sprintf(" 0x%02X", info->raw[offset+i]);
					}//if
					else {
						value.sprintf("0x%02X", info->raw[offset+i]);
					}//if
				}//for
				after=create(parent, i18n("Data"),value);
			}//if
		}//else
	}//if
	return after;
}//addCapsVendor

static QTreeWidgetItem* addCaps(QTreeWidgetItem *parent, QTreeWidgetItem *after, pciInfo *info) {
	QTreeWidgetItem *localAfter=NULL;
	QTreeWidgetItem *topLocalAfter=NULL;
	QString value;
	unsigned char offset;
	if ((info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_NORMAL)||(info->cooked.headerType.headerType_bits.headerType==PCI_HEADER_TYPE_BRIDGE)) {
		if ((info->cooked.header.header0.capabilityList!=0)&&(info->cooked.status.status_bits.statCapList!=0)) {
			after=create(parent, i18n("Capabilities"),value.sprintf("0x%02X",info->cooked.header.header0.capabilityList));
			if (getuid()==0) {
				for (offset=info->cooked.header.header0.capabilityList; offset!=0; offset=info->raw[offset+1]) {
					topLocalAfter=create(after, getNameById(capNames,info->raw[offset]),value.sprintf("0x%02X",info->raw[offset]));
					localAfter=create(topLocalAfter, i18n("Next"),(info->raw[offset+1]==0?i18n("0x00 (None)"):value.sprintf("0x%02X",info->raw[offset+1])));
					switch (info->raw[offset]) {
					case 0x01: //Power Managemet
						addCapsPm(topLocalAfter, localAfter, info, offset);
						break;
					case 0x02: //AGP
						addCapsAgp(topLocalAfter, localAfter, info, offset);
						break;
					case 0x03: //VPD
						addCapsVpd(topLocalAfter, localAfter, info, offset);
						break;
					case 0x05: //MSI
						addCapsMsi(topLocalAfter, localAfter, info, offset);
						break;
					case 0x09: //vendor specific
						addCapsVendor(topLocalAfter, localAfter, info, offset);
						break;
					default:
						break;
					}//switch
				}//for
			}//if
			else {
				topLocalAfter=createTitle(after, i18n("Root only"));
			}//else
		}//if
		else {
			after=create(parent, i18n("Capabilities"),i18n("0x00 (None)"));
		}//else
	}//if
	return after;
}//addCaps

bool GetInfo_PCIUtils(QTreeWidget* tree) {

	QStringList headers;
	headers << i18n("Information") << i18n("Value");
	tree->setHeaderLabels(headers);
	tree->setRootIsDecorated(true);
	
	pci_access *PCIAccess=NULL;
	pci_dev *PCIDevice=NULL;

	//init libpci
	PCIAccess=pci_alloc();
	if (PCIAccess==NULL) {
		return false;
	}//if

	pci_init(PCIAccess);
	pci_scan_bus(PCIAccess);

	QTreeWidgetItem *DeviceName=NULL, *after=NULL;
	QString value;
	pciInfo info;

	//proces all devices
	for (PCIDevice=PCIAccess->devices; PCIDevice; PCIDevice=PCIDevice->next) {
		//get PCI data
		pci_fill_info(PCIDevice, PCI_FILL_IDENT|PCI_FILL_IRQ|PCI_FILL_BASES|PCI_FILL_ROM_BASE|PCI_FILL_SIZES);
		if (getuid()==0) {
			pci_read_block(PCIDevice, 0, info.raw, 256);
		}//if
		else {
			pci_read_block(PCIDevice, 0, info.raw, 64);
		}//else

		QStringList deviceList;
		deviceList << value.sprintf("%02X:%02X.%d", PCIDevice->bus, PCIDevice->dev, PCIDevice->func);
		//create device tree
		DeviceName=new QTreeWidgetItem(tree, deviceList);
		//adding class, subclass and programming interface info
		after=addDeviceClass(DeviceName, NULL, &info);

		//adding revision
		after=create(DeviceName, i18n("Revision"), value.sprintf("0x%02X", info.cooked.revision));
		//adding vendor, device, and subvendor/sudevice info
		after=addVendor(DeviceName, after, &info, PCIAccess);
		//adding control
		after=addControl(DeviceName, after, &info);
		//adding status
		after=addStatus(DeviceName, after, &info);

		//adding cache line size
		after=create(DeviceName, i18n("Cache line size"), value.sprintf("0x%02X", info.cooked.cacheLineSize));
		//adding latency
		after=addLatency(DeviceName, after, &info);
		//adding header type
		after=addHeaderType(DeviceName, after, &info);
		//adding BIST
		after=addBist(DeviceName, after, &info);
		//adding address mapping
		after=addMapping(DeviceName, after, &info, PCIDevice);
		//adding bus info
		after=addBus(DeviceName, after, &info);
		//adding secondary status
		after=addSecStatus(DeviceName, after, &info);
		//adding resourece behind bridge 
		after=addBridgeBehind(DeviceName, after, &info);
		//adding bridge control
		after=addBridgeControl(DeviceName, after, &info);
		//adding cardbus resource
		after=addCardbusResource(DeviceName, after, &info);
		//adding cardbus control
		after=addCardbusControl(DeviceName, after, &info);
		//adding ROM
		after=addRom(DeviceName, after, &info, PCIDevice);
		//adding capabilities
		after=addCaps(DeviceName, after, &info);

		//adding interrupt info (IRQ, pin)
		after=addInterrupt(DeviceName, after, PCIDevice->irq, info.cooked.header.header0.interruptPin); //PCI data have wrong IRQ ?!?
		//add raw PCI config data
		after=addRaw(DeviceName, after, &info);
	}//for

	pci_cleanup(PCIAccess);
	return true;
} //GetInfo_PCIUtils

