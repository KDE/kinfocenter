/*
 * kcmsambastatistics.h
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
#ifndef kcmsambastatistics_h_included
#define kcmsambastatistics_h_included

#include <QList>
#include <QWidget>

class QTreeWidget;
class QLabel;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QPushButton;

class KConfig;

class SmallLogItem {
public:
	SmallLogItem() :
		name(""), count(0) {
	}
	SmallLogItem(const QString &n) :
		name(n), count(1) {
	}
	QString name;
	int count;
};

class LogItem {
public:
	LogItem() :
		name(""), accessed(), count(0) {
	}
	LogItem(const QString &n, const QString &a) :
		name(n), accessed(), count(1) {
		accessed.append(new SmallLogItem(a));
	}
	~LogItem() {
		qDeleteAll(accessed);
		accessed.clear();
	}
	QString name;
	QList<SmallLogItem *> accessed;
	int count;
	SmallLogItem* itemInList(const QString &name);
	void addItem(const QString &host);
};

class SambaLog {
public:
	~SambaLog() {
		qDeleteAll(items);
		items.clear();
	}
	QList<LogItem *> items;
	void addItem(const QString &share, const QString &host);
	void printItems();
private:
	LogItem* itemInList(const QString &name);
};

class StatisticsView : public QWidget {
Q_OBJECT
public:
	explicit StatisticsView(QWidget *parent=0, KConfig *config=0);
	virtual ~StatisticsView() {
	}
	void saveSettings() {
	}
	void loadSettings() {
	}
public Q_SLOTS:
	void setListInfo(QTreeWidget *list, int nrOfFiles, int nrOfConnections);
private:
	KConfig *configFile;
	QTreeWidget* dataList;
	QTreeWidget* viewStatistics;
	QLabel* connectionsL, *filesL;
	QComboBox* eventCb;
	QLabel* eventL;
	QLineEdit* serviceLe;
	QLabel* serviceL;
	QLineEdit* hostLe;
	QLabel* hostL;
	QPushButton* calcButton, *clearButton;
	QCheckBox* expandedInfoCb, *expandedUserCb;
	int connectionsCount, filesCount, calcCount;
private Q_SLOTS:
	void clearStatistics();
	void calculate();
};
#endif // main_included
