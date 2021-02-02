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
#include <QRegularExpression>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>

USBDB::USBDB() {
	QString db = QStringLiteral("/usr/share/hwdata/usb.ids"); /* on Fedora and Arch*/
	if (!QFile::exists(db)) {
		db = QStringLiteral("/usr/share/misc/usb.ids"); /* on Gentoo */
	}
	if (!QFile::exists(db)) {
                //cannot use locate(AppDataLocation) as the app is kinfocenter
		db = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kcmusb"), QStandardPaths::LocateDirectory);
                if (!db.isEmpty())
                    db+=QLatin1String("/usb.ids");
        }
	if (db.isEmpty())
		return;

	QFile f(db);

	if (f.open(QIODevice::ReadOnly)) {
		QTextStream ts(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		ts.setCodec("UTF-8");
#endif

		QString line, name;
		int id=0, subid=0, protid=0;

		const QRegularExpression vendor(QStringLiteral("^[0-9a-fA-F]+ "));
		const QRegularExpression product(QStringLiteral("^\\s+[0-9a-fA-F]+ "));
		const QRegularExpression cls(QStringLiteral("^C [0-9a-fA-F]{2}"));
		const QRegularExpression subclass(QStringLiteral("^\\s+[0-9a-fA-F]{2}  "));
		const QRegularExpression prot(QStringLiteral("^\\s+[0-9a-fA-F]{2}  "));

		QRegularExpressionMatch rmatch;
		while (!ts.atEnd()) {
			line = ts.readLine();
            if (!line.isEmpty() && (line.at(0) == QLatin1String("#") || line.trimmed().isEmpty()))
				continue;

			// skip AT lines
			if (line.left(2) == QLatin1String("AT"))
				continue;

			if (line.contains(cls, &rmatch) && rmatch.capturedLength(0) == 4) {
				id = line.midRef(2,2).toInt(0, 16);
				name = line.mid(4).trimmed();
				_classes.insert(QStringLiteral("%1").arg(id), name);
			} else if (line.contains(prot, &rmatch) && rmatch.capturedLength(0) > 5) {
				line = line.trimmed();
				protid = line.leftRef(2).toInt(0, 16);
				name = line.mid(4).trimmed();
				_classes.insert(QStringLiteral("%1-%2-%3").arg(id).arg(subid).arg(protid), name);
			} else if (line.contains(subclass, &rmatch) && rmatch.capturedLength(0) > 4) {
				line = line.trimmed();
				subid = line.leftRef(2).toInt(0, 16);
				name = line.mid(4).trimmed();
				_classes.insert(QStringLiteral("%1-%2").arg(id).arg(subid), name);
			} else if (line.contains(vendor, &rmatch) && rmatch.capturedLength(0) == 5) {
				id = line.leftRef(4).toInt(0, 16);
				name = line.mid(6);
				_ids.insert(QStringLiteral("%1").arg(id), name);
			} else if (line.contains(product, &rmatch) && rmatch.capturedLength(0) > 5) {
				line = line.trimmed();
				subid = line.leftRef(4).toInt(0, 16);
				name = line.mid(6);
				_ids.insert(QStringLiteral("%1-%2").arg(id).arg(subid), name);
			}

		}

		f.close();
	}
}

QString USBDB::vendor(int id) {
	QString s = _ids[QStringLiteral("%1").arg(id)];
	if (id != 0) {
		return s;
	}
	return QString();
}

QString USBDB::device(int vendor, int id) {
	QString s = _ids[QStringLiteral("%1-%2").arg(vendor).arg(id)];
	if ((id != 0) && (vendor != 0))
		return s;
	return QString();
}

QString USBDB::cls(int cls) {
	return _classes[QStringLiteral("%1").arg(cls)];
}

QString USBDB::subclass(int cls, int sub) {
	return _classes[QStringLiteral("%1-%2").arg(cls).arg(sub)];
}

QString USBDB::protocol(int cls, int sub, int prot) {
	return _classes[QStringLiteral("%1-%2-%3").arg(cls).arg(sub).arg(prot)];
}

