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
#include <QLabel>
#include <k3process.h>
#include <q3listview.h>
#include <kconfig.h>

#define SCREEN_XY_OFFSET 20

class K3Process;

class NetMon : public QWidget
{
Q_OBJECT
public:
   NetMon(QWidget *parent, KConfig * config=0 );
   void saveSettings() {}
   void loadSettings() {}
private:
   KConfig *configFile;
   K3Process *showmountProc;
   Q3ListView *list;
   QLabel *version;
   QTimer *timer;
   Q3ListViewItem *killrow;
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
   void slotReceivedData(K3Process *proc, char *buffer, int buflen);
};

#endif // main_included
