/*
 * kcmsambaimports.h
 *
 * Copyright (c) 2000 Alexander Neundorf <alexander.neundorf@rz.tu-ilmenau.de>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef kcmsambaimports_h_included
#define kcmsambaimports_h_included

#include <QTimer>
#include <QTreeWidget>

#include <KConfig>

class ImportsView : public QWidget {
Q_OBJECT
public:
	explicit ImportsView(QWidget *parent, KConfig *config=0);
	virtual ~ImportsView() {
	}
	void saveSettings() {
	}
	void loadSettings() {
	}
private:
	KConfig *configFile;
	QTreeWidget list;
	QTimer timer;
private Q_SLOTS:
	void updateList();
};

#endif // main_included
