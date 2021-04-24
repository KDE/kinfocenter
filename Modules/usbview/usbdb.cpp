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
#include <QRegularExpressionMatch>
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

		QString line;
		int id = -1, subid = -1;
		const QRegularExpression vendor(QStringLiteral("^([0-9a-fA-F]{4})\\s+(.+)$"));
		const QRegularExpression product(QStringLiteral("^\t([0-9a-fA-F]{4})\\s+(.+)$"));
		const QRegularExpression cls(QStringLiteral("^C ([0-9a-fA-F]{2})\\s+(.+)$"));
		const QRegularExpression subclass(QStringLiteral("^\t([0-9a-fA-F]{2})\\s+(.+)$"));
		const QRegularExpression prot(QStringLiteral("^\t\t([0-9a-fA-F]{2})\\s+(.+)$"));
		while (ts.readLineInto(&line)) {
			if (!line.isEmpty() && line.startsWith(QLatin1String("#")))
				continue;

			// skip unhandled categories lines
			if (line.isEmpty() ||
			    line.startsWith(QLatin1String("AT ")) ||
			    line.startsWith(QLatin1String("HID ")) ||
			    line.startsWith(QLatin1String("R ")) ||
			    line.startsWith(QLatin1String("BIAS ")) ||
			    line.startsWith(QLatin1String("PHY ")) ||
			    line.startsWith(QLatin1String("HUT ")) ||
			    line.startsWith(QLatin1String("L ")) ||
			    line.startsWith(QLatin1String("HCC ")) ||
			    line.startsWith(QLatin1String("VT "))) {
				id = -1;
				subid = -1;
				continue;
			}

			QRegularExpressionMatch match;
			if (line.startsWith('C') && (match = cls.match(line)).hasMatch()) {
				id = match.capturedRef(1).toInt(0, 16);
				const QString name = match.capturedRef(2).trimmed().toString();
				_classes.insert(QStringLiteral("%1").arg(id), name);
			} else if (id >= 0 && subid >= 0 && (match = prot.match(line)).hasMatch()) {
				const int protid = match.capturedRef(1).toInt(0, 16);
				const QString name = match.capturedRef(2).trimmed().toString();
				_classes.insert(QStringLiteral("%1-%2-%3").arg(id).arg(subid).arg(protid), name);
			} else if (id >= 0 && (match = subclass.match(line)).hasMatch()) {
				subid = match.capturedRef(1).toInt(0, 16);
				const QString name = match.capturedRef(2).trimmed().toString();
				_classes.insert(QStringLiteral("%1-%2").arg(id).arg(subid), name);
			} else if ((match = vendor.match(line)).hasMatch()) {
				id = match.capturedRef(1).toInt(0, 16);
				const QString name = match.captured(2);
				_ids.insert(QStringLiteral("%1").arg(id), name);
			} else if (id >= 0 && (match = product.match(line)).hasMatch()) {
				subid = match.capturedRef(1).toInt(0, 16);
				const QString name = match.capturedRef(2).trimmed().toString();
				_ids.insert(QStringLiteral("%1-%2").arg(id).arg(subid), name);
			} else {
				id = -1;
				subid = -1;
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

