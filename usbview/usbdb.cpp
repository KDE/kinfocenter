/***************************************************************************
 *   Copyright (C) 2001 by Matthias Hoelzer-Kluepfel <mhk@caldera.de>      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usbdb.h"

#include <iostream>

#include <QFile>
#include <QRegExp>
//Added by qt3to4:
#include <QTextStream>

#include <kstandarddirs.h>

USBDB::USBDB() {
	QString db = "/usr/share/hwdata/usb.ids"; /* on Fedora */
	if (!QFile::exists(db))
		db = KStandardDirs::locate("data", "kcmusb/usb.ids");
	if (db.isEmpty())
		return;

	QFile f(db);

	if (f.open(QIODevice::ReadOnly)) {
		QTextStream ts(&f);
		ts.setCodec("UTF-8");

		QString line, name;
		int id=0, subid=0, protid=0;
		QRegExp vendor("[0-9a-fA-F]+ ");
		QRegExp product("\\s+[0-9a-fA-F]+ ");
		QRegExp cls("C [0-9a-fA-F][0-9a-fA-F]");
		QRegExp subclass("\\s+[0-9a-fA-F][0-9a-fA-F]  ");
		QRegExp prot("\\s+[0-9a-fA-F][0-9a-fA-F]  ");
		while (!ts.atEnd()) {
			line = ts.readLine();
			if (line.left(1) == "#" || line.trimmed().isEmpty())
				continue;

			// skip AT lines
			if (line.left(2) == "AT")
				continue;

			if (cls.indexIn(line) == 0 && cls.matchedLength() == 4) {
				id = line.mid(2,2).toInt(0, 16);
				name = line.mid(4).trimmed();
				_classes.insert(QString("%1").arg(id), name);
			} else if (prot.indexIn(line) == 0 && prot.matchedLength() > 5) {
				line = line.trimmed();
				protid = line.left(2).toInt(0, 16);
				name = line.mid(4).trimmed();
				_classes.insert(QString("%1-%2-%3").arg(id).arg(subid).arg(protid), name);
			} else if (subclass.indexIn(line) == 0 && subclass.matchedLength() > 4) {
				line = line.trimmed();
				subid = line.left(2).toInt(0, 16);
				name = line.mid(4).trimmed();
				_classes.insert(QString("%1-%2").arg(id).arg(subid), name);
			} else if (vendor.indexIn(line) == 0 && vendor.matchedLength() == 5) {
				id = line.left(4).toInt(0, 16);
				name = line.mid(6);
				_ids.insert(QString("%1").arg(id), name);
			} else if (product.indexIn(line) == 0 && product.matchedLength() > 5) {
				line = line.trimmed();
				subid = line.left(4).toInt(0, 16);
				name = line.mid(6);
				_ids.insert(QString("%1-%2").arg(id).arg(subid), name);
			}

		}

		f.close();
	}
}

QString USBDB::vendor(int id) {
	QString s = _ids[QString("%1").arg(id)];
	if (id != 0) {
		return s;
	}
	return QString();
}

QString USBDB::device(int vendor, int id) {
	QString s = _ids[QString("%1-%2").arg(vendor).arg(id)];
	if ((id != 0) && (vendor != 0))
		return s;
	return QString();
}

QString USBDB::cls(int cls) {
	return _classes[QString("%1").arg(cls)];
}

QString USBDB::subclass(int cls, int sub) {
	return _classes[QString("%1-%2").arg(cls).arg(sub)];
}

QString USBDB::protocol(int cls, int sub, int prot) {
	return _classes[QString("%1-%2-%3").arg(cls).arg(sub).arg(prot)];
}

