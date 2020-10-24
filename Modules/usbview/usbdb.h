/*
 *   SPDX-FileCopyrightText: 2001 Matthias Hoelzer-Kluepfel <mhk@caldera.de>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __USB_DB_H__
#define __USB_DB_H__

#include <QMap>
#include <QString>

class USBDB {
public:

	USBDB();

	QString vendor(int id);
	QString device(int vendor, int id);

	QString cls(int cls);
	QString subclass(int cls, int sub);
	QString protocol(int cls, int sub, int prot);

private:

	QMap<QString, QString> _classes;
	QMap<QString, QString> _ids;

};

#endif
