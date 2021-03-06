/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __USB_DB_H__
#define __USB_DB_H__

#include <QMap>
#include <QString>

#include <stdint.h>

class USBDB {
public:

	USBDB();

	QString vendor(uint16_t id) const;
	QString device(uint16_t vendor, uint16_t id) const;

	QString cls(uint8_t cls) const;
	QString subclass(uint8_t cls, uint8_t sub) const;
	QString protocol(uint8_t cls, uint8_t sub, uint8_t prot) const;

private:

	QMap<QString, QString> _classes;
	QMap<QString, QString> _ids;

};

#endif
