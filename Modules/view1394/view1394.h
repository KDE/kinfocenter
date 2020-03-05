/*
 * view1394.h
 *
 *  Copyright (C) 2003 Alexander Neundorf <neundorf@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef VIEW1394_H_
#define VIEW1394_H_

#include <kcmodule.h>

#include <QMap>
#include <QSocketNotifier>

#include <QTimer>

#include "ui_view1394widget.h"

#include <libraw1394/raw1394.h>


class OuiDb {
public:
	OuiDb();
	QString vendor(octlet_t guid);
private:
	void loadFromOuiTxt(const QString &filename);
	void loadFromCustomOuiDb(const QString &filename);
	QMap<QString, QString> m_vendorIds;
};

class View1394Widget : public QWidget, public Ui::View1394Widget {
public:
	View1394Widget(QWidget *parent) :
		QWidget(parent) {
		setupUi( this);
	}
};

class View1394 : public KCModule {
Q_OBJECT
public:
	View1394(QWidget *parent, const QVariantList &args);
	virtual ~View1394();

public Q_SLOTS:
	// Public slots
	void rescanBus();
	void generateBusReset();

private:
	View1394Widget *m_view;
	QList<raw1394handle_t> m_handles;
	QList<QSocketNotifier*> m_notifiers;
	bool readConfigRom(raw1394handle_t handle, nodeid_t nodeid, quadlet_t& firstQuad, quadlet_t& cap, octlet_t& guid);
	bool m_insideRescanBus;
	QTimer m_rescanTimer;
	OuiDb *m_ouiDb;
	OuiDb *ouiDb();
private Q_SLOTS:
	void callRaw1394EventLoop(int fd);
};
#endif
