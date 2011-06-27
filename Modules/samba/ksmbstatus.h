/*
 * ksmbstatus.h
 *
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
#ifndef ksmbstatus_h_included
#define ksmbstatus_h_included

#include <QTimer>
#include <QTreeWidget>
#include <QLabel>
#include <QProcess>

#include <KConfig>

#define SCREEN_XY_OFFSET 20

class NetMon : public QWidget {
Q_OBJECT
public:
	explicit NetMon(QWidget *parent, KConfig * config=0);
	void saveSettings() {
	}
	void loadSettings() {
	}
private:
	KConfig *configFile;
	QProcess *showmountProc;
	QTreeWidget *list;
	QLabel *version;
	QTimer *timer;
	int rownumber;
	enum {header, connexions, locked_files, finished, nfs} readingpart;
	int lo[65536];
	int nrpid;
	void processNFSLine(char *bufline, int linelen);
	void processSambaLine(char *bufline, int linelen);

	QByteArray strShare, strUser, strGroup, strMachine, strSince, strPid;
	int iUser, iGroup, iMachine, iPid;

private Q_SLOTS:
	void killShowmount();
	void update();
	void readFromProcess();
	void smbstatusError();
};

#endif // main_included
