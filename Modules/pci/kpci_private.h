/* Retrive information about PCI subsystem through libpci library from
   pciutils package. This should be possible on Linux, BSD and AIX.
   
   Device classes, subclasses and programming interfaces are hardcoded
   here, since there are only few of them, and they are important and
   should their names be translated.
   
   pci.cpp private header, don't include in other files.

   Author: Konrad Rzepecki <hannibal@megapolis.pl>
*/
#ifndef KCONTROL_KPCI_PRIVATE_H
#define KCONTROL_KPCI_PRIVATE_H

#include "kpci.h"

#include <QString>

#include <KLocalizedString>
#include <kdemacros.h>

//pci lookup buffer size
#define NAME_BUFFER_SIZE 256

//common used strings
static const char strCtxt[]="state of PCI item";
static const char strEnabled[]=I18N_NOOP2("state of PCI item", "Enabled");
static const char strDisabled[]=I18N_NOOP2("state of PCI item", "Disabled");
static const char strYes[]=I18N_NOOP2("state of PCI item", "Yes");
static const char strNo[]=I18N_NOOP2("state of PCI item", "No");
static const char strUnknown[]=I18N_NOOP2("state of PCI item", "Unknown");

struct id2name {
	int id;
	QString name;
};

struct id3name {
	int id,id2;
	QString name;
};

struct id4name {
	int id,id2,id3;
	QString name;
};

union pciInfo {
	unsigned char raw[256];
	struct {
		unsigned short vendor;
		unsigned short device;
		union {
			unsigned short command;
			struct {
				unsigned comIo:1;
				unsigned comMemory:1;
				unsigned comMaster:1;
				unsigned comSpecial:1;
				unsigned comInvalidate:1;
				unsigned comVgaPalette:1;
				unsigned comParity:1;
				unsigned comWait:1;
				unsigned comSerr:1;
				unsigned comFastBack:1;
				unsigned comInterrupt:1;
				unsigned comUnk:5;
			} command_bits KDE_PACKED;
		} command KDE_PACKED;
		union {
			unsigned short status;
			struct {
				unsigned statUnk:3;
				unsigned statInterrupt:1;
				unsigned statCapList:1;
				unsigned stat66MHz:1;
				unsigned statUdf:1;
				unsigned statFastBack:1;
				unsigned statParity:1;
				unsigned statDevsel:2;
				unsigned statSigTargetAbort:1;
				unsigned statRecTargetAbort:1;
				unsigned statRecMasterAbort:1;
				unsigned statSigSystemError:1;
				unsigned statDetectedParity:1;
			} status_bits KDE_PACKED;
		} status KDE_PACKED;
		unsigned char revision;
		union {
			unsigned char devProgIface;
			struct {
				unsigned progPriOperMode:1;
				unsigned progPriProgInd:1;
				unsigned progSecOperMode:1;
				unsigned progSecProgInd:1;
				unsigned progUnk:3;
				unsigned progIdeMaster:1;
			} devProgIface_bits KDE_PACKED;
		} devProgIface KDE_PACKED;
		unsigned char devSubClass;
		unsigned char devClass;
		unsigned char cacheLineSize;
		unsigned char latencyTimer;
		union {
			unsigned char headerTypeFull:1;
			struct {
				unsigned headerType:7;
				unsigned multifunctional:1;
			} headerType_bits KDE_PACKED;
		} headerType KDE_PACKED;
		union {
			unsigned char bist;
			struct {
				unsigned bistCode:4;
				unsigned bistUnk:2;
				unsigned bistStart:1;
				unsigned bistCapable:1;
			} bist_bits KDE_PACKED;
		} bist KDE_PACKED;
		union {
			struct { //header0
				union {
					unsigned baseAddress;
					struct {
						unsigned baseAddressMap:1;
						unsigned baseAddressType:2;
						unsigned baseAddressPref:1;
						unsigned baseAddressAddr:28;
					} baseAddress_bits KDE_PACKED;
				} KDE_PACKED mapping[6];
				unsigned cardbusCis;
				unsigned short subVendor;
				unsigned short subDevice;
				union {
					unsigned romAddress;
					struct {
						unsigned romEnabled:1;
						unsigned romUnk:10;
						unsigned romAddr:21;
					} romAddress_bits KDE_PACKED;
				} romAddress KDE_PACKED;
				unsigned char capabilityList;
				unsigned char reserved1[7];
				unsigned char interruptLine;
				unsigned char interruptPin;
				unsigned char minGnt;
				unsigned char maxLat;
			} header0 KDE_PACKED;
			struct { //header1
				unsigned baseAddress0_2;
				unsigned baseAddress1_2;
				unsigned char primaryBus;
				unsigned char secondaryBus;
				unsigned char subordinateBus;
				unsigned char secLatencyTimer;
				union {
					unsigned char ioBase;
					struct {
						unsigned ioBaseType:1;
						unsigned ioBaseUnk:3;
						unsigned ioBaseAddr:4;
					} ioBase_bits KDE_PACKED;
				} ioBase KDE_PACKED;
				unsigned char ioLimit;
				union {
					unsigned short secStatus;
					struct {
						unsigned secStatUnk:3;
						unsigned secStatInterrupt:1;
						unsigned secStatCapList:1;
						unsigned secStat66MHz:1;
						unsigned secStatUdf:1;
						unsigned secStatFastBack:1;
						unsigned secStatParity:1;
						unsigned secStatDevsel:2;
						unsigned secStatSigTargetAbort:1;
						unsigned secStatRecTargetAbort:1;
						unsigned secStatRecMasterAbort:1;
						unsigned secStatSigSystemError:1;
						unsigned secStatDetectedParity:1;
					} secStatus_bits KDE_PACKED;
				} secStatus KDE_PACKED;
				union {
					unsigned short memoryBase;
					struct {						
						unsigned memoryType:1;
						unsigned memoryUnk:3;
						unsigned memoryAddr:4;
					} memoryBase_bits KDE_PACKED;
				} memoryBase KDE_PACKED;
				unsigned short memoryLimit;
				union {
					unsigned short prefMemoryBase;
					struct {						
						unsigned prefMemoryType:1;
						unsigned prefMemoryUnk:3;
						unsigned prefMemoryAddr:4;
					} prefMemoryBase_bits KDE_PACKED;
				} prefMemoryBase KDE_PACKED;
				unsigned short prefMemoryLimit;
				unsigned prefBaseUpper32;
				unsigned prefLimitUpper32;
				unsigned short ioBaseUpper16;
				unsigned short ioLimitUpper16;
				unsigned char capabilityList_2;
				unsigned char reserved2[3];
				unsigned romAddress1;
				unsigned char interruptLine_2;
				unsigned char interruptPin_2;
				union {
					unsigned short bridgeControl;
					struct {
						unsigned bridgeControlParity:1;
						unsigned bridgeControlSerr:1;
						unsigned bridgeControlIsa:1;
						unsigned bridgeControlVga:1;
						unsigned bridgeControlUnk:1;
						unsigned bridgeControlMasterAbort:1;
						unsigned bridgeControlBusReset:1;
						unsigned bridgeControlFastBack:1;
						unsigned bridgeControlPriDisTim:1;
						unsigned bridgeControlSecDisTim:1;
						unsigned bridgeControlDisTimStat:1;
						unsigned bridgeControlDisTimeSerr:1;
						unsigned bridgeControlUnk2:4;
					} bridgeControl_bits KDE_PACKED;
				} bridgeControl KDE_PACKED;
			} header1 KDE_PACKED;
			struct{ //header2
				unsigned baseAddress0_3;
				union {
					unsigned short cbSecStatus;
					struct {
						unsigned cbSecStatUnk:3;
						unsigned cbSecStatInterrupt:1;
						unsigned cbSecStatCapList:1;
						unsigned cbSecStat66MHz:1;
						unsigned cbSecStatUdf:1;
						unsigned cbSecStatFastBack:1;
						unsigned cbSecStatParity:1;
						unsigned cbSecStatDevsel:2;
						unsigned cbSecStatSigTargetAbort:1;
						unsigned cbSecStatRecTargetAbort:1;
						unsigned cbSecStatRecMasterAbort:1;
						unsigned cbSecStatSigSystemError:1;
						unsigned cbSecStatDetectedParity:1;
					} cbSecStatus_bits KDE_PACKED;
				} cbSecStatus KDE_PACKED;
				unsigned char cbPrimaryBus;
				unsigned char cbCardBus;
				unsigned char cbSubordinateBus;
				unsigned char cbLatencyTimer;
				struct {
					unsigned cbMemoryBase;
					unsigned cbMemoryLimit;
				} KDE_PACKED cbMemory[2];
				struct {
					union {
						unsigned cbIoBase;
						struct {
							unsigned cbIoBaseType:1;
							unsigned coIoBaseUnk:1;
							unsigned cbIoBaseAddr:30;
						} cbIoBase_bits KDE_PACKED;
					} cbIoBase KDE_PACKED;
					unsigned cbIoLimit;
				} KDE_PACKED cbIo[2];
				unsigned char interruptLine_3;
				unsigned char interruptPin_3;
				union {
					unsigned short cbControl;
					struct {
						unsigned cbControlParity:1;
						unsigned cbControlSerr:1;
						unsigned cbControlIsa:1;
						unsigned cbControlVga:1;
						unsigned cbControlUnk:1;
						unsigned cbControlMasterAbort:1;
						unsigned cbControlBusReset:1;
						unsigned cbControl16Int:1;
						unsigned cbControlPref0:1;
						unsigned cbControlPref1:1;
						unsigned cbControlPostWrites:1;
						unsigned cbControlUnk2:5;
					} cbControl_bits KDE_PACKED;
				} cbControl KDE_PACKED;
				unsigned short cbSubVendor;
				unsigned short cbSubDevice;
				unsigned short cbLegacyModeBase;
			} header2 KDE_PACKED;
		} header KDE_PACKED;
	} cooked KDE_PACKED;
} KDE_PACKED;

union agpInfo{
	unsigned char raw[10];
	struct {
		struct {
			unsigned revMinor:4;
			unsigned revMaior:4;
			unsigned char unk;
		} revision KDE_PACKED;
		union {
			unsigned status;
			struct {
				unsigned statusEnhRate:4;
				unsigned statusUnk:28;
			} status_bits0 KDE_PACKED;
			struct {
				unsigned statusRate:3;
				unsigned statusMode:1;
				unsigned statusFastWrite:1;
				unsigned statusOver4gb:1;
				unsigned statusHtrans:1;
				unsigned statusGart64b:1;
				unsigned statusItaCoh:1;
				unsigned statusSba:1;
				unsigned statusCalCycle:3;
				unsigned statusOptReqSize:3;
				unsigned statusUnk1:1;
				unsigned statusIsochSupp:1;
				unsigned statusUnk2:6;
				unsigned statusReq:8;
			} status_bits1 KDE_PACKED;
		} status KDE_PACKED;
		union {
			unsigned config;
			struct {
				unsigned configEnhRate:4;
				unsigned configUnk:28;
			} config_bits0 KDE_PACKED;
			struct {
				unsigned configRate:3;
				unsigned configUnk1:1;
				unsigned configFastWrite:1;
				unsigned configOver4gb:1;
				unsigned configUnk2:1;
				unsigned configGart64b:1;
				unsigned configAgp:1;
				unsigned configSba:1;
				unsigned configCalCycle:3;
				unsigned configOptReqSize:3;
				unsigned configUnk3:8;
				unsigned configReq:8;
			} config_bits1 KDE_PACKED;
		} config KDE_PACKED;
	} cooked KDE_PACKED;
} KDE_PACKED;

union pmInfo {
	unsigned char raw[6];
	struct {
		union {
			unsigned short caps;
			struct {
				unsigned capsVer:3;
				unsigned capsClock:1;
				unsigned capsUnk:1;
				unsigned capsDsi:1;
				unsigned capsAuxCur:3;
				unsigned capsD1Supp:1;
				unsigned capsD2Supp:1;
				unsigned capsPmeD0:1;
				unsigned capsPmeD1:1;
				unsigned capsPmeD2:1;
				unsigned capsPmeD3hot:1;
				unsigned capsPmeD3cold:1;
			} caps_bits KDE_PACKED;
		} caps KDE_PACKED;
		union {
			unsigned short status;
			struct {
				unsigned statPower:2;
				unsigned statUnk:6;
				unsigned statPme:1;
				unsigned statDataSelect:4;
				unsigned statDataScale:2;
				unsigned statPmeStat:1;
			} status_bits KDE_PACKED;
		} status KDE_PACKED;
		union {
			unsigned char statusBridge;
			struct {
				unsigned statBridgeUnk:6;
				unsigned statBridgeBx:1;
				unsigned statBridgeClock:1;
			} statusBridge_bits KDE_PACKED;
		} statusBridge KDE_PACKED;
		unsigned char data;
	} cooked KDE_PACKED;
} KDE_PACKED;

union vpdInfo {
	unsigned char raw[6];
	struct {
		unsigned vpdAddress:15;
		unsigned vpdTransfer:1;
		unsigned vpdData;
	} cooked  KDE_PACKED;
} KDE_PACKED;

union msiInfo {
	unsigned char raw[22];
	struct {
		union {
			unsigned short msiControl;
			struct {
				unsigned msiEnable:1;
				unsigned msiMmCapable:3;
				unsigned msiMmEnable:3;
				unsigned msi64bit:1;
				unsigned msiPerVector:1;
				unsigned msiUnk0:7;
			} msiControl_bits KDE_PACKED;
		} msiControl KDE_PACKED;
		unsigned msiAddress;
		union {
			struct {
				unsigned msiUpperAddress;
				unsigned short msiData64;
				unsigned short msiUnk64;
				unsigned msiMask64;
				unsigned msiPending64;
			} address KDE_PACKED;
			struct {
				unsigned short msiData;
				unsigned short msiUnk;			
				unsigned msiMask;
				unsigned msiPending;
			} data KDE_PACKED;
		} payload KDE_PACKED;
	} cooked KDE_PACKED;
} KDE_PACKED;

//In following arrays -1 mean default value

//keep -1 in last position
//device classes list
static const id2name devClass[]={{	0x00,	i18n("Unclassified device")},
								{	0x01,	i18n("Mass storage controller")},
								{	0x02,	i18n("Network controller")},
								{	0x03,	i18n("Display controller")},
								{	0x04,	i18n("Multimedia controller")},
								{	0x05,	i18n("Memory controller")},
								{	0x06,	i18n("Bridge")},
								{	0x07,	i18n("Communication controller")},
								{	0x08,	i18n("Generic system peripheral")},
								{	0x09,	i18n("Input device controller")},
								{	0x0A,	i18n("Docking station")},
								{	0x0B,	i18n("Processor")},
								{	0x0C,	i18n("Serial bus controller")},
								{	0x0D,	i18n("Wireless controller")},
								{	0x0E,	i18n("Intelligent controller")},
								{	0x0F,	i18n("Satellite communications controller")},
								{	0x10,	i18n("Encryption controller")},
								{	0x11,	i18n("Signal processing controller")},
								{	-1,		i18n("Unknown device class")}
};

//keep -1 in last position in "id"
//and in last position in "id2" with certain "id"
//subdevice classes list
static const id3name devSubclass[]={	{	0x00,	0x00,	i18n("Non-VGA unclassified device")},
										{	0x00,	0x01,	i18n("VGA unclassified device")},
										{	0x00,	-1,		i18n("Unknown unclassified device")},

										{	0x01,	0x00,	i18n("SCSI storage controller")},
										{	0x01,	0x01,	i18n("IDE controller")},
										{	0x01,	0x02,	i18n("Floppy disk controller")},
										{	0x01,	0x03,	i18n("IPI bus controller")},
										{	0x01,	0x04,	i18n("RAID bus controller")},
										{	0x01,	0x05,	i18n("ATA controller")},
										{	0x01,	0x06,	i18n("Serial ATA direct port access")},
										{	0x01,	0x80,	i18n("Mass storage controller")},
										{	0x01,	-1,		i18n("Unknown storage controller")},

										{	0x02,	0x00,	i18n("Ethernet controller")},
										{	0x02,	0x01,	i18n("Token ring network controller")},
										{	0x02,	0x02,	i18n("FDDI network controller")},
										{	0x02,	0x03,	i18n("ATM network controller")},
										{	0x02,	0x04,	i18n("ISDN controller")},
										{	0x02,	0x05,	i18n("WorldFip controller")},
										{	0x02,	0x06,	i18n("PICMG 2.14 multi computing")},
										{	0x02,	0x80,	i18n("Network controller")},
										{	0x02,	-1,		i18n("Unknown network controller")},

										{	0x03,	0x00,	i18n("VGA compatible controller")},
										{	0x03,	0x01,	i18n("XGA compatible controller")},
										{	0x03,	0x02,	i18n("3D controller")},
										{	0x03,	0x80,	i18n("Display controller")},
										{	0x03,	-1,		i18n("Unknown display controller")},

										{	0x04,	0x00,	i18n("Multimedia video controller")},
										{	0x04,	0x01,	i18n("Multimedia audio controller")},
										{	0x04,	0x02,	i18n("Computer telephony device")},
										{	0x04,	0x80,	i18n("Multimedia controller")},
										{	0x04,	-1,		i18n("Unknown multimedia controller")},

										{	0x05,	0x00,	i18n("RAM memory")},
										{	0x05,	0x01,	i18n("FLASH memory")},
										{	0x05,	0x80,	i18n("Memory controller")},
										{	0x05,	-1,		i18n("Unknown memory controller")},

										{	0x06,	0x00,	i18n("Host bridge")},
										{	0x06,	0x01,	i18n("ISA bridge")},
										{	0x06,	0x02,	i18n("EISA bridge")},
										{	0x06,	0x03,	i18n("MicroChannel bridge")},
										{	0x06,	0x04,	i18n("PCI bridge")},
										{	0x06,	0x05,	i18n("PCMCIA bridge")},
										{	0x06,	0x06,	i18n("NuBus bridge")},
										{	0x06,	0x07,	i18n("CardBus bridge")},
										{	0x06,	0x08,	i18n("RACEway bridge")},
										{	0x06,	0x09,	i18n("Semi-transparent PCI-to-PCI bridge")},
										{	0x06,	0x0A,	i18n("InfiniBand to PCI host bridge")},
										{	0x06,	0x80,	i18n("Bridge")},
										{	0x06,	-1,		i18n("Unknown bridge")},

										{	0x07,	0x00,	i18n("Serial controller")},
										{	0x07,	0x01,	i18n("Parallel controller")},
										{	0x07,	0x02,	i18n("Multiport serial controller")},
										{	0x07,	0x03,	i18n("Modem")},
										{	0x07,	0x04,	i18n("GPIB (IEEE 488.1/2) controller")},
										{	0x07,	0x05,	i18n("Smart card")},
										{	0x07,	0x80,	i18n("Communication controller")},
										{	0x07,	-1,		i18n("Unknown communication controller")},

										{	0x08,	0x00,	i18n("PIC")},
										{	0x08,	0x01,	i18n("DMA controller")},
										{	0x08,	0x02,	i18n("Timer")},
										{	0x08,	0x03,	i18n("RTC")},
										{	0x08,	0x04,	i18n("PCI Hot-plug controller")},
										{	0x08,	0x80,	i18n("System peripheral")},
										{	0x08,	-1,		i18n("Unknown system peripheral")},

										{	0x09,	0x00,	i18n("Keyboard controller")},
										{	0x09,	0x01,	i18n("Digitizer Pen")},
										{	0x09,	0x02,	i18n("Mouse controller")},
										{	0x09,	0x03,	i18n("Scanner controller")},
										{	0x09,	0x04,	i18n("Gameport controller")},
										{	0x09,	0x80,	i18n("Input device controller")},
										{	0x09,	-1,		i18n("Unknown input device controller")},

										{	0x0A,	0x00,	i18n("Generic docking station")},
										{	0x0A,	0x80,	i18n("Docking station")},
										{	0x0A,	-1,		i18n("Unknown docking station")},

										{	0x0B,	0x00,	i18n("386")},
										{	0x0B,	0x01,	i18n("486")},
										{	0x0B,	0x02,	i18n("Pentium")},
										{	0x0B,	0x10,	i18n("Alpha")},
										{	0x0B,	0x20,	i18n("Power PC")},
										{	0x0B,	0x30,	i18n("MIPS")},
										{	0x0B,	0x40,	i18n("Co-processor")},
										{	0x0B,	-1,		i18n("Unknown processor")},

										{	0x0C,	0x00,	i18n("FireWire (IEEE 1394)")},
										{	0x0C,	0x01,	i18n("ACCESS bus")},
										{	0x0C,	0x02,	i18n("SSA")},
										{	0x0C,	0x03,	i18n("USB controller")},
										{	0x0C,	0x04,	i18n("Fibre channel")},
										{	0x0C,	0x05,	i18n("SMBus")},
										{	0x0C,	0x06,	i18n("InfiniBand")},
										{	0x0C,	0x07,	i18n("IPMI interface")},
										{	0x0C,	0x08,	i18n("SERCOS interface")},
										{	0x0C,	0x09,	i18n("CANbus")},
										{	0x0C,	-1,		i18n("Unknown serial bus controller")},

										{	0x0D,	0x00,	i18n("IRDA controller")},
										{	0x0D,	0x01,	i18n("Consumer IR controller")},
										{	0x0D,	0x10,	i18n("RF controller")},
										{	0x0D,	0x11,	i18n("Bluetooth")},
										{	0x0D,	0x12,	i18n("Broadband")},
										{	0x0D,	0x20,	i18n("Ethernet (802.11a - 5 GHz)")},
										{	0x0D,	0x21,	i18n("Ethernet (802.11b - 2.4 GHz)")},
										{	0x0D,	0x80,	i18n("Wireless controller")},
										{	0x0D,	-1,		i18n("Unknown wireless controller")},

										{	0x0E,	0x00,	i18n("I2O")},
										{	0x0E,	-1,		i18n("Unknown intelligent controller")},

										{	0x0F,	0x01,	i18n("Satellite TV controller")},
										{	0x0F,	0x02,	i18n("Satellite audio communication controller")},
										{	0x0F,	0x03,	i18n("Satellite voice communication controller")},
										{	0x0F,	0x04,	i18n("Satellite data communication controller")},
										{	0x0F,	-1,		i18n("Unknown satellite communications controller")},

										{	0x10,	0x00,	i18n("Network and computing encryption device")},
										{	0x10,	0x10,	i18n("Entertainment encryption device")},
										{	0x10,	0x80,	i18n("Encryption controller")},
										{	0x10,	-1,		i18n("Unknown encryption controller")},

										{	0x11,	0x00,	i18n("DPIO module")},
										{	0x11,	0x01,	i18n("Performance counters")},
										{	0x11,	0x10,	i18n("Communication synchronizer")},
										{	0x11,	0x20,	i18n("Management card")},
										{	0x11,	0x80,	i18n("Signal processing controller")},
										{	0x11,	-1,		i18n("Unknown signal processing controller")},

										{	-1,		-1,		i18n("Unknown subdevice class")}
};

//keep -1 in last position in "id",
//in last position in "id2" with certain "id",
//and in last position in "id3" with certain "id2"
//device programming interface list
static const id4name devInterface[]={	{	0x01,	0x05,	0x20,	i18n("single DMA")},
										{	0x01,	0x05,	0x30,	i18n("chained DMA")},

										{	0x03,	0x00,	0x00,	i18n("VGA compatible")},
										{	0x03,	0x00,	0x01,	i18n("8514 compatible")},

										{	0x06,	0x04,	0x00,	i18n("Normal decode")},
										{	0x06,	0x04,	0x01,	i18n("Subtractive decode")},

										{	0x06,	0x08,	0x00,	i18n("Transparent mode")},
										{	0x06,	0x08,	0x01,	i18n("Endpoint mode")},

										{	0x06,	0x09,	0x40,	i18n("Primary bus towards host CPU")},
										{	0x06,	0x09,	0x40,	i18n("Secondary bus towards host CPU")},

										{	0x07,	0x00,	0x00,	i18n("8250")},
										{	0x07,	0x00,	0x01,	i18n("16450")},
										{	0x07,	0x00,	0x02,	i18n("16550")},
										{	0x07,	0x00,	0x03,	i18n("16650")},
										{	0x07,	0x00,	0x04,	i18n("16750")},
										{	0x07,	0x00,	0x05,	i18n("16850")},
										{	0x07,	0x00,	0x06,	i18n("16950")},

										{	0x07,	0x01,	0x00,	i18n("SPP")},
										{	0x07,	0x01,	0x01,	i18n("BiDir")},
										{	0x07,	0x01,	0x02,	i18n("ECP")},
										{	0x07,	0x01,	0x03,	i18n("IEEE1284")},
										{	0x07,	0x01,	0xFE,	i18n("IEEE1284 Target")},

										{	0x07,	0x03,	0x00,	i18n("Generic")},
										{	0x07,	0x03,	0x01,	i18n("Hayes/16450")},
										{	0x07,	0x03,	0x02,	i18n("Hayes/16550")},
										{	0x07,	0x03,	0x03,	i18n("Hayes/16650")},
										{	0x07,	0x03,	0x04,	i18n("Hayes/16750")},

										{	0x08,	0x00,	0x00,	i18n("8259")},
										{	0x08,	0x00,	0x01,	i18n("ISA PIC")},
										{	0x08,	0x00,	0x02,	i18n("EISA PIC")},
										{	0x08,	0x00,	0x03,	i18n("IO-APIC")},
										{	0x08,	0x00,	0x04,	i18n("IO(X)-APIC")},

										{	0x08,	0x01,	0x00,	i18n("8237")},
										{	0x08,	0x01,	0x01,	i18n("ISA DMA")},
										{	0x08,	0x01,	0x01,	i18n("EISA DMA")},

										{	0x08,	0x02,	0x00,	i18n("8254")},
										{	0x08,	0x02,	0x01,	i18n("ISA timer")},
										{	0x08,	0x02,	0x01,	i18n("EISA timers")},

										{	0x08,	0x03,	0x00,	i18n("Generic")},
										{	0x08,	0x03,	0x01,	i18n("ISA RTC")},

										{	0x09,	0x04,	0x00,	i18n("Generic")},
										{	0x09,	0x04,	0x01,	i18n("Extended")},

										{	0x0C,	0x00,	0x00,	i18n("Generic")},
										{	0x0C,	0x00,	0x01,	i18n("OHCI")},

										{	0x0C,	0x03,	0x00,	i18n("UHCI")},
										{	0x0C,	0x03,	0x10,	i18n("OHCI")},
										{	0x0C,	0x03,	0x20,	i18n("EHCI")},
										{	0x0C,	0x03,	0x80,	i18n("Unspecified")},
										{	0x0C,	0x03,	0xFE,	i18n("USB Device")},

										{	0x0C,	0x07,	0x00,	i18n("SMIC")},
										{	0x0C,	0x07,	0x01,	i18n("Keyboard controller style")},
										{	0x0C,	0x07,	0x02,	i18n("Block transfer")},
									
										{	-1,		-1,		-1,		i18n(strUnknown)}
};


//keep -1 in last position
//capabilities list
static const id2name capNames[]={{	0x01,	i18n("Power management")},
								{	0x02,	i18n("AGP")},
								{	0x03,	i18n("Vital product data")},
								{	0x04,	i18n("Slot identification")},
								{	0x05,	i18n("Message signaled interrupts")},
								{	0x06,	i18n("CompactPCI hot swap")},
								{	0x07,	i18n("PCI-X")},
								{	0x08,	i18n("HyperTransport")},
								{	0x09,	i18n("Vendor specific")},
								{	0x0A,	i18n("Debug port")},
								{	0x0B,	i18n("CompactPCI central resource control")},
								{	0x0C,	i18n("PCI hot-plug")},
								{	0x0E,	i18n("AGP x8")},
								{	0x0F,	i18n("Secure device")},
								{	0x10,	i18n("PCI express")},
								{	0x11,	i18n("MSI-X")},
								{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//device selection timing
static const id2name devSel[]={	{	0x00,	i18n("Fast")},
								{	0x01,	i18n("Medium")},
								{	0x02,	i18n("Slow")},
								{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//mapping type
static const id2name mappingType[]={{	0x00,	i18n("32 bit")},
									{	0x01,	i18n("Below 1M")},
									{	0x02,	i18n("64 bit")},
									{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//header type
static const id2name headerType[]={	{	0x00,	i18n("Standard")},
									{	0x01,	i18n("Bridge")},
									{	0x02,	i18n("CardBus")},
									{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//AGP rate
static const id2name agpRate[]={{	0x01,	i18n("1X")},
								{	0x02,	i18n("2X")},
								{	0x03,	i18n("1X & 2X")},
								{	0x04,	i18n("4X")},
								{	0x05,	i18n("1X & 4X")},
								{	0x06,	i18n("2X & 4X")},
								{	0x07,	i18n("1X & 2X & 4X")},
								{	0x09,	i18n("4X")},
								{	0x0A,	i18n("8X")},
								{	0x0B,	i18n("4X & 8X")},
								{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//AGP calibrating cycle
static const id2name calCycle[]={	{	0x00,	i18n("4 ms")},
									{	0x01,	i18n("16 ms")},
									{	0x02,	i18n("64 ms")},
									{	0x03,	i18n("256 ms")},
									{	0x07,	i18n("Not needed")},
									{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//PM auxiliary current
static const id2name auxCur[]={	{	0x00,	i18n("0 (self powered)")},
								{	0x01,	i18n("55 mA")},
								{	0x02,	i18n("100 mA")},
								{	0x03,	i18n("160 mA")},
								{	0x04,	i18n("220 mA")},
								{	0x05,	i18n("270 mA")},
								{	0x06,	i18n("320 mA")},
								{	0x07,	i18n("375 mA")},
								{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//PM power state
static const id2name powerState[]={	{	0x00,	i18n("D0")},
									{	0x01,	i18n("D1")},
									{	0x02,	i18n("D2")},
									{	0x03,	i18n("D3 hot")},
									{	-1,		i18n(strUnknown)}
};

//keep -1 in last position
//MSI multiple message
static const id2name multipleMessage[]={{	0x00,	i18n("1 vector")},
										{	0x01,	i18n("2 vectors")},
										{	0x02,	i18n("4 vectors")},
										{	0x03,	i18n("8 vectors")},
										{	0x04,	i18n("16 vectors")},
										{	0x05,	i18n("32 vectors")},
										{	-1,		i18n(strUnknown)}
};

static const QString& getNameById(const id2name *const table,int id);
static const QString& getNameBy2Id(const id3name *const table,int id,int id2);
static const QString& getNameBy3Id(const id4name *const table,int id,int id2, int id3);

#endif //KCONTROL_KPCI_PRIVATE_H
