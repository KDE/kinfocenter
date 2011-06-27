/*
 * ksmbstatus.cpp
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <QLayout>

#include <klocale.h>
#include <kdialog.h>

#include "ksmbstatus.h"
#include "ksmbstatus.moc"

#define Before(ttf,in) in.left(in.indexOf(ttf))
#define After(ttf,in)  (in.contains(ttf)?QString(in.mid(in.indexOf(ttf)+QString(ttf).length())):QString(""))

NetMon::NetMon(QWidget * parent, KConfig *config) :
	QWidget(parent), configFile(config), showmountProc(0), strShare(""), strUser(""), strGroup(""), strMachine(""), strSince(""), strPid(""), iUser(0), iGroup(0), iMachine(0), iPid(0) {
	QBoxLayout *topLayout = new QVBoxLayout(this);
	topLayout->setMargin(KDialog::marginHint());
	topLayout->setSpacing(KDialog::spacingHint());

	list = new QTreeWidget(this);
	topLayout->addWidget(list);
	version=new QLabel(this);
	topLayout->addWidget(version);

	list->setAllColumnsShowFocus(true);
	list->setMinimumSize(425, 200);

	QStringList headers;
        headers << i18n("Type") << i18n("Service") << i18n("Accessed From")
		<< i18n("UID") << i18n("GID") << i18n("PID") << i18n("Open Files");
	list->setHeaderLabels(headers);

	timer = new QTimer(this);
	timer->start(15000);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	update();
}

void NetMon::processNFSLine(char *bufline, int) {
	QByteArray line(bufline);
	if (line.contains(":/")) {
		QTreeWidgetItem *item = new QTreeWidgetItem(list);
		item->setText(0, "NFS");
		item->setText(0, After(":",line));
		item->setText(0, Before(":/",line));
	}
}

void NetMon::processSambaLine(char *bufline, int) {
	QByteArray line(bufline);
	rownumber++;
	if (rownumber == 2)
		version->setText(bufline); // second line = samba version
	if ((readingpart==header) && line.contains("Service")) {
		iUser=line.indexOf("uid");
		iGroup=line.indexOf("gid");
		iPid=line.indexOf("pid");
		iMachine=line.indexOf("machine");
	} else if ((readingpart==header) && (line.contains("---"))) {
		readingpart=connexions;
	} else if ((readingpart==connexions) && (int(line.length())>=iMachine)) {
		strShare=line.mid(0, iUser);
		strUser=line.mid(iUser, iGroup-iUser);
		strGroup=line.mid(iGroup, iPid-iGroup);
		strPid=line.mid(iPid, iMachine-iPid);

		line=line.mid(iMachine, line.length());
		strMachine=line;
		QTreeWidgetItem * item = new QTreeWidgetItem(list);
		item->setText(0, "SMB");
		item->setText(1, strShare);
		item->setText(2, strMachine);
		item->setText(3, strUser);
		item->setText(4, strGroup);
		item->setText(5, strPid/*,strSince*/);
	} else if (readingpart==connexions)
		readingpart=locked_files;
	else if ((readingpart==locked_files) && (line.indexOf("No ")==0))
		readingpart=finished;
	else if (readingpart==locked_files) {
		if ((strncmp(bufline, "Pi", 2) !=0) // "Pid DenyMode ..."
				&& (strncmp(bufline, "--", 2) !=0)) // "------------"
		{
			char *tok=strtok(bufline, " ");
			if (tok) {
				int pid=atoi(tok);
				(lo)[pid]++;
			}
		}
	}
}

// called when we get some data from smbstatus
// can be called for any size of buffer (one line, several lines,
// half of one ...)
void NetMon::readFromProcess() {
	QProcess *process = qobject_cast<QProcess *>(sender());
	if (!process || !process->canReadLine())
		return;

	qint64 buflen = 8046; // 8k enough?
	char buffer[buflen];
	buflen = process->readLine(buffer, buflen);
	//kDebug()<<"received stuff";
	char s[250], *start, *end;
	size_t len;
	start = buffer;
	while ((end = strchr(start, '\n'))) // look for '\n'
	{
		len = end-start;
		if (len>=sizeof(s))
			len=sizeof(s)-1;
		strncpy(s, start, len);
		s[len] = '\0';
		//kDebug() << "recived: "<<s;
		if (readingpart==nfs)
			processNFSLine(s, len);
		else
			processSambaLine(s, len); // process each line
		start=end+1;
	}
        /* FIXME: is this needed? was here with the Q3Support classes, but seems a little inane
	if (readingpart==nfs) {
		list->viewport()->update();
		list->update();
	}
        */
	// here we could save the remaining part of line, if ever buffer
	// doesn't end with a '\n' ... but will this happen ?
}

void NetMon::smbstatusError()
{
	version->setText(i18n("Error: Unable to run smbstatus"));
}

void NetMon::update() {
	QProcess *process = new QProcess();

	memset(&lo, 0, sizeof(lo));
	list->clear();
	/* Re-read the Contents ... */

	QString path(::getenv("PATH"));
	path += "/bin:/sbin:/usr/bin:/usr/sbin";

	rownumber=0;
	readingpart=header;
	nrpid=0;
	process->setEnvironment(QStringList() << ("PATH=" + path));
	connect(process, SIGNAL(readyRead()), SLOT(readFromProcess()));
	connect(process, SIGNAL(error(QProcess::ProcessError)), SLOT(smbstatusError()));
	process->start("smbstatus");
	process->waitForFinished();
	if (rownumber==0) // empty result
		version->setText(i18n("Error: Unable to open configuration file \"smb.conf\""));
	else
	{
		// ok -> count the number of locked files for each pid
		for (int i = 0; i < list->topLevelItemCount(); ++i)
		{
			QTreeWidgetItem *row = list->topLevelItem(i);
			//         cerr<<"NetMon::update: this should be the pid: "<<row->text(5)<<endl;
			int pid=row->text(5).toInt();
			row->setText(6,QString("%1").arg((lo)[pid]));
		}
	}

	delete process;
	process=0;

	readingpart=nfs;
	delete showmountProc;
	showmountProc=new QProcess();
	connect(showmountProc, SIGNAL(readyRead()), SLOT(readFromProcess()));
	showmountProc->setEnvironment(QStringList() << ("PATH=" + path));
	//without this timer showmount hangs up to 5 minutes
	//if the portmapper daemon isn't running
	QTimer::singleShot(5000,this,SLOT(killShowmount()));
	//kDebug()<<"starting kill timer with 5 seconds";
	connect(showmountProc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(killShowmount()));
	connect(showmountProc,SIGNAL(error(QProcess::ProcessError)),this,SLOT(killShowmount()));
	showmountProc->start("showmount", QStringList() << "-a" << "localhost");

	version->adjustSize();
	list->show();
}

void NetMon::killShowmount() {
	//kDebug()<<"killShowmount()";
	showmountProc->deleteLater();
	showmountProc=0;
}

