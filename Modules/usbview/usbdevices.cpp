/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usbdevices.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <QFile>
#include <QDir>
#include <QRegExp>

#include <KLocalizedString>
#include <kmessagebox.h>

#include "usbdb.h"

#include <math.h>

#if defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)
#include <sys/ioctl.h>
#include <sys/param.h>
#endif

QList<USBDevice*> USBDevice::_devices;
USBDB *USBDevice::_db;

USBDevice::USBDevice() :
	_bus(0), _level(0), _parent(0), _port(0), _count(0), _device(0), _channels(0), _power(0), _speed(0.0), _bwTotal(0), _bwUsed(0), _bwPercent(0), _bwIntr(0), _bwIso(0), _hasBW(false), _verMajor(0), _verMinor(0), _class(0), _sub(0), _prot(0), _maxPacketSize(0), _configs(0), _vendorID(0),
			_prodID(0), _revMajor(0), _revMinor(0) {
	_devices.append(this);

	if (!_db)
		_db = new USBDB;
}

USBDevice::~USBDevice() {

}

static QString catFile(const QString &fname) {
	char buffer[256];
	QString result;
	int fd =:: open(QFile::encodeName(fname), O_RDONLY);
	if (fd<0)
	return QString();

	if (fd >= 0)
	{
		ssize_t count;
		while ((count = ::read(fd, buffer, 256)) > 0)
        result.append(QString::fromLatin1(buffer).leftRef(count));

		::close(fd);
	}
	return result.trimmed();
}

void USBDevice::parseSysDir(int bus, int parent, int level, const QString& dname) {
	_level = level;
	_parent = parent;
    _manufacturer = catFile(dname + QStringLiteral("/manufacturer"));
    _product = catFile(dname + QStringLiteral("/product"));

	_bus = bus;
    _device = catFile(dname + QStringLiteral("/devnum")).toUInt();

	if (_device == 1)
		_product += QStringLiteral(" (%1)").arg(_bus);

    _vendorID = catFile(dname + QStringLiteral("/idVendor")).toUInt(nullptr, 16);
    _prodID = catFile(dname + QStringLiteral("/idProduct")).toUInt(nullptr, 16);

    _class = catFile(dname + QStringLiteral("/bDeviceClass")).toUInt(nullptr, 16);
    _sub = catFile(dname + QStringLiteral("/bDeviceSubClass")).toUInt(nullptr, 16);
    _maxPacketSize = catFile(dname + QStringLiteral("/bMaxPacketSize0")).toUInt();

    _speed = catFile(dname + QStringLiteral("/speed")).toDouble();
    _serial = catFile(dname + QStringLiteral("/serial"));
    _channels = catFile(dname + QStringLiteral("/maxchild")).toUInt();

    double version = catFile(dname + QStringLiteral("/version")).toDouble();
	_verMajor = int(version);
	_verMinor = int(10*(version - floor(version)));

	QDir dir(dname);
	dir.setNameFilters(QStringList() << QStringLiteral("%1-*").arg(bus));
	dir.setFilter(QDir::Dirs);
	const QStringList list = dir.entryList();

	for (QStringList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it) {
        if ((*it).contains(QLatin1Char(':')))
			continue;

		USBDevice* dev = new USBDevice();
        dev->parseSysDir(bus, ++level, _device, dname + QLatin1Char('/') + *it);
	}
}

void USBDevice::parseLine(const QString& line) {
	if (line.startsWith(QLatin1String("T:")))
		sscanf(line.toLocal8Bit().data(), "T:  Bus=%2d Lev=%2d Prnt=%2d Port=%d Cnt=%2d Dev#=%3d Spd=%3f MxCh=%2d", &_bus, &_level, &_parent, &_port, &_count, &_device, &_speed, &_channels);
	else if (line.startsWith(QLatin1String("S:  Manufacturer")))
		_manufacturer = line.mid(17);
	else if (line.startsWith(QLatin1String("S:  Product"))) {
		_product = line.mid(12);
		/* add bus number to root devices */
		if (_device==1)
			_product += QStringLiteral(" (%1)").arg(_bus);
	} else if (line.startsWith(QLatin1String("S:  SerialNumber")))
		_serial = line.mid(17);
	else if (line.startsWith(QLatin1String("B:"))) {
		sscanf(line.toLocal8Bit().data(), "B:  Alloc=%3d/%3d us (%2d%%), #Int=%3d, #Iso=%3d", &_bwUsed, &_bwTotal, &_bwPercent, &_bwIntr, &_bwIso);
		_hasBW = true;
	} else if (line.startsWith(QLatin1String("D:"))) {
		char buffer[11];
		sscanf(line.toLocal8Bit().data(), "D:  Ver=%x.%x Cls=%x(%10s) Sub=%x Prot=%x MxPS=%u #Cfgs=%u", &_verMajor, &_verMinor, &_class, buffer, &_sub, &_prot, &_maxPacketSize, &_configs);
        _className = QString::fromLatin1(buffer);
	} else if (line.startsWith(QLatin1String("P:")))
		sscanf(line.toLocal8Bit().data(), "P:  Vendor=%x ProdID=%x Rev=%x.%x", &_vendorID, &_prodID, &_revMajor, &_revMinor);
}

USBDevice* USBDevice::find(int bus, int device) {
	foreach(USBDevice* usbDevice, _devices) {
		if (usbDevice->bus() == bus && usbDevice->device() == device)
			return usbDevice;
	}

	return nullptr;
}

QString USBDevice::product() {
	if (!_product.isEmpty())
		return _product;
	QString pname = _db->device(_vendorID, _prodID);
	if (!pname.isEmpty())
		return pname;
	return i18n("Unknown");
}

QString USBDevice::dump() {
	QString r;

    r = QStringLiteral("<qml><h2><center>") + product() + QStringLiteral("</center></h2><br/><hl/>");

	if (!_manufacturer.isEmpty())
        r += i18n("<b>Manufacturer:</b> ") + _manufacturer + QStringLiteral("<br/>");
	if (!_serial.isEmpty())
        r += i18n("<b>Serial #:</b> ") + _serial + QStringLiteral("<br/>");

	r += QLatin1String("<br/><table>");

	QString c = QStringLiteral("<td>%1</td>").arg(_class);
	QString cname = _db->cls(_class);
	if (!cname.isEmpty())
        c += QStringLiteral("<td>(") + i18n(cname.toLatin1()) +QStringLiteral(")</td>");
	r += i18n("<tr><td><i>Class</i></td>%1</tr>", c);
	QString sc = QStringLiteral("<td>%1</td>").arg(_sub);
	QString scname = _db->subclass(_class, _sub);
	if (!scname.isEmpty())
        sc += QStringLiteral("<td>(") + i18n(scname.toLatin1()) +QStringLiteral(")</td>");
	r += i18n("<tr><td><i>Subclass</i></td>%1</tr>", sc);
	QString pr = QStringLiteral("<td>%1</td>").arg(_prot);
	QString prname = _db->protocol(_class, _sub, _prot);
	if (!prname.isEmpty())
        pr += QStringLiteral("<td>(") + prname +QStringLiteral(")</td>");
	r += i18n("<tr><td><i>Protocol</i></td>%1</tr>", pr);
#if !(defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD))
	r += ki18n("<tr><td><i>USB Version</i></td><td>%1.%2</td></tr>")
	.subs(_verMajor,0,16).subs(_verMinor,2,16,QChar::fromLatin1('0'))
	.toString();
#endif
	r += QLatin1String("<tr><td></td></tr>");

	QString v = QString::number(_vendorID, 16);
	QString name = _db->vendor(_vendorID);
	if (!name.isEmpty())
        v += QStringLiteral("<td>(") + name +QStringLiteral(")</td>");
	r += i18n("<tr><td><i>Vendor ID</i></td><td>0x%1</td></tr>", v);
	QString p = QString::number(_prodID, 16);
	QString pname = _db->device(_vendorID, _prodID);
	if (!pname.isEmpty())
        p += QStringLiteral("<td>(") + pname +QStringLiteral(")</td>");
	r += i18n("<tr><td><i>Product ID</i></td><td>0x%1</td></tr>", p);
	r += ki18n("<tr><td><i>Revision</i></td><td>%1.%2</td></tr>")
	.subs(_revMajor,0,16).subs(_revMinor,2,16,QChar::fromLatin1('0'))
	.toString();
	r += QLatin1String("<tr><td></td></tr>");

	r += i18n("<tr><td><i>Speed</i></td><td>%1 Mbit/s</td></tr>", _speed);
	r += i18n("<tr><td><i>Channels</i></td><td>%1</td></tr>", _channels);
#if (defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD)) && !defined(DISABLE_USBDEVICES_FREEBSD)
	if ( _power )
	r += i18n("<tr><td><i>Power Consumption</i></td><td>%1 mA</td></tr>", _power);
	else
	r += i18n("<tr><td><i>Power Consumption</i></td><td>self powered</td></tr>");
	r += i18n("<tr><td><i>Attached Devicenodes</i></td><td>%1</td></tr>", _devnodes.at(0));
	if ( _devnodes.count() > 1 ) {
		QStringList::const_iterator it = _devnodes.constBegin();
		++it;
		for (; it != _devnodes.constEnd(); ++it )
        r += QStringLiteral("<tr><td></td><td>") + *it + QStringLiteral("</td></tr>");
	}
#else
	r += i18n("<tr><td><i>Max. Packet Size</i></td><td>%1</td></tr>", _maxPacketSize);
#endif
	r += QLatin1String("<tr><td></td></tr>");

	if (_hasBW) {
		r += i18n("<tr><td><i>Bandwidth</i></td><td>%1 of %2 (%3%)</td></tr>", _bwUsed, _bwTotal, _bwPercent);
		r += i18n("<tr><td><i>Intr. requests</i></td><td>%1</td></tr>", _bwIntr);
		r += i18n("<tr><td><i>Isochr. requests</i></td><td>%1</td></tr>", _bwIso);
		r += QLatin1String("<tr><td></td></tr>");
	}

	r += QLatin1String("</table>");

	return r;
}

#if !(defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD))
bool USBDevice::parse(const QString &fname) {
	_devices.clear();

	QString result;

	// read in the complete file
	//
	// Note: we can't use a QTextStream, as the files in /proc
	// are pseudo files with zero length
	char buffer[256];
	int fd =:: open(QFile::encodeName(fname), O_RDONLY);
	if (fd<0)
	return false;

	if (fd >= 0)
	{
		ssize_t count;
		while ((count = ::read(fd, buffer, 256)) > 0)
        result.append(QString::fromLatin1(buffer).leftRef(count));

		::close(fd);
	}

	// read in the device infos
    USBDevice *device = nullptr;
	int start=0, end;
    result.remove(QRegExp(QStringLiteral("^\n")));
    while ((end = result.indexOf(QLatin1Char('\n'), start)) > 0)
	{
		QString line = result.mid(start, end-start);

		if (line.startsWith(QLatin1String("T:")))
		device = new USBDevice();

		if (device)
		device->parseLine(line);

		start = end+1;
	}
	return true;
}

bool USBDevice::parseSys(const QString &dname) {
	QDir d(dname);
	d.setNameFilters(QStringList() << QStringLiteral("usb*"));
	const QStringList list = d.entryList();

    for (QStringList::const_iterator it = list.constBegin(), total = list.constEnd(); it != total; ++it) {
		USBDevice* device = new USBDevice();

		int bus = 0;
		QRegExp bus_reg(QStringLiteral("[a-z]*([0-9]+)"));
		if (bus_reg.indexIn(*it) != -1)
			bus = bus_reg.cap(1).toInt();

        device->parseSysDir(bus, 0, 0, d.absolutePath() + QLatin1Char('/') + *it);
	}

	return d.count();
}

#else

// Unused by *BSD
bool USBDevice::parseSys(const QString &fname)
{
	Q_UNUSED(fname)

	return true;
}

# if defined(DISABLE_USBDEVICES_FREEBSD)

/*
 * FIXME: The USB subsystem has changed a lot in FreeBSD 8.0
 *        Support for it must be written.
 */

bool USBDevice::parse(const QString &fname)
{
	Q_UNUSED(fname)

	return true;
}

# else

/*
 * FreeBSD support by Markus Brueffer <markus@brueffer.de>
 *
 * Basic idea and some code fragments were taken from FreeBSD's usbdevs(8),
 * originally developed for NetBSD, so this code should work with no or
 * only little modification on NetBSD.
 */

void USBDevice::collectData( int fd, int level, usb_device_info &di, int parent)
{
	// determine data for this device
	_level = level;
	_parent = parent;

	_bus = di.udi_bus;
	_device = di.udi_addr;
	_product = QLatin1String(di.udi_product);
	if ( _device == 1 )
	_product += ' ' + QString::number( _bus );
	_manufacturer = QLatin1String(di.udi_vendor);
	_prodID = di.udi_productNo;
	_vendorID = di.udi_vendorNo;
	_class = di.udi_class;
	_sub = di.udi_subclass;
	_prot = di.udi_protocol;
	_power = di.udi_power;
	_channels = di.udi_nports;

	// determine the speed
#if defined(__DragonFly__) || (defined(Q_OS_FREEBSD) && __FreeBSD_version > 490102) || defined(Q_OS_NETBSD)
	switch (di.udi_speed) {
		case USB_SPEED_LOW: _speed = 1.5; break;
		case USB_SPEED_FULL: _speed = 12.0; break;
		case USB_SPEED_HIGH: _speed = 480.0; break;
	}
#else
	_speed = di.udi_lowspeed ? 1.5 : 12.0;
#endif

	// Get all attached devicenodes
	for ( int i = 0; i < USB_MAX_DEVNAMES; ++i )
	if ( di.udi_devnames[i][0] )
	_devnodes << di.udi_devnames[i];

	// For compatibility, split the revision number
	sscanf( di.udi_release, "%x.%x", &_revMajor, &_revMinor );

	// Cycle through the attached devices if there are any
	for ( int p = 0; p < di.udi_nports; ++p ) {
		// Get data for device
		struct usb_device_info di2;

		di2.udi_addr = di.udi_ports[p];

		if ( di2.udi_addr >= USB_MAX_DEVICES )
		continue;

		if ( ioctl(fd, USB_DEVICEINFO, &di2) == -1 )
		continue;

		// Only add the device if we didn't detect it, yet
		if (!find( di2.udi_bus, di2.udi_addr ) )
		{
			USBDevice *device = new USBDevice();
			device->collectData( fd, level + 1, di2, di.udi_addr );
		}
	}
}

bool USBDevice::parse(const QString &fname)
{
	Q_UNUSED(fname)

	static bool showErrorMessage = true;
	bool error = false;
	_devices.clear();

	QFile controller("/dev/usb0");
	int i = 1;
	while ( controller.exists() )
	{
		// If the devicenode exists, continue with further inspection
		if ( controller.open(QIODevice::ReadOnly) )
		{
			for ( int addr = 1; addr < USB_MAX_DEVICES; ++addr )
			{
				struct usb_device_info di;

				di.udi_addr = addr;
				if ( ioctl(controller.handle(), USB_DEVICEINFO, &di) != -1 )
				{
					if (!find( di.udi_bus, di.udi_addr ) )
					{
						USBDevice *device = new USBDevice();
						device->collectData( controller.handle(), 0, di, 0);
					}
				}
			}
			controller.close();
#ifndef Q_OS_NETBSD
		} else {
			error = true;
#endif
		}
		controller.setFileName( QString::fromLocal8Bit("/dev/usb%1").arg(i++) );
	}

	if ( showErrorMessage && error ) {
		showErrorMessage = false;
		KMessageBox::error( 0, i18n("Could not open one or more USB controller. Make sure, you have read access to all USB controllers that should be listed here."));
	}

	return true;
}

# endif // defined(DISABLE_USBDEVICES_FREEBSD)
#endif // !(defined(Q_OS_FREEBSD) || defined(Q_OS_NETBSD))
