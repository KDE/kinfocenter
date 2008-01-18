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

NetMon::NetMon( QWidget * parent, KConfig *config )
   : QWidget(parent)
   ,configFile(config)
   ,showmountProc(0)
   ,strShare("")
   ,strUser("")
   ,strGroup("")
   ,strMachine("")
   ,strSince("")
   ,strPid("")
   ,iUser(0)
   ,iGroup(0)
   ,iMachine(0)
   ,iPid(0)
{
    QBoxLayout *topLayout = new QVBoxLayout(this);
    topLayout->setMargin(KDialog::marginHint());
    topLayout->setSpacing(KDialog::spacingHint());

    list=new Q3ListView(this,"Hello");
    topLayout->addWidget(list);
    version=new QLabel(this);
    topLayout->addWidget(version);

    list->setAllColumnsShowFocus(true);
    list->setMinimumSize(425,200);
    list->setShowSortIndicator(true);

    list->addColumn(i18n("Type"));
    list->addColumn(i18n("Service"));
    list->addColumn(i18n("Accessed From"));
    list->addColumn(i18n("UID"));
    list->addColumn(i18n("GID"));
    list->addColumn(i18n("PID"));
    list->addColumn(i18n("Open Files"));

    timer = new QTimer(this);
    timer->start(15000);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    update();
}

void NetMon::processNFSLine(char *bufline, int)
{
   QByteArray line(bufline);
   if (line.contains(":/"))
      new Q3ListViewItem(list,"NFS",After(":",line),Before(":/",line));
}

void NetMon::processSambaLine(char *bufline, int)
{
   QByteArray line(bufline);
   rownumber++;
   if (rownumber == 2)
      version->setText(bufline); // second line = samba version
   if ((readingpart==header) && line.contains("Service"))
   {
      iUser=line.indexOf("uid");
      iGroup=line.indexOf("gid");
      iPid=line.indexOf("pid");
      iMachine=line.indexOf("machine");
   }
   else if ((readingpart==header) && (line.contains("---")))
   {
      readingpart=connexions;
   }
   else if ((readingpart==connexions) && (int(line.length())>=iMachine))
   {
      strShare=line.mid(0,iUser);
      strUser=line.mid(iUser,iGroup-iUser);
      strGroup=line.mid(iGroup,iPid-iGroup);
      strPid=line.mid(iPid,iMachine-iPid);

      line=line.mid(iMachine,line.length());
      strMachine=line;
      new Q3ListViewItem(list,"SMB",strShare,strMachine, strUser,strGroup,strPid/*,strSince*/);
   }
   else if (readingpart==connexions)
      readingpart=locked_files;
   else if ((readingpart==locked_files) && (line.indexOf("No ")==0))
      readingpart=finished;
   else if (readingpart==locked_files)
   {
      if ((strncmp(bufline,"Pi", 2) !=0) // "Pid DenyMode ..."
          && (strncmp(bufline,"--", 2) !=0)) // "------------"
      {
         char *tok=strtok(bufline," ");
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
void NetMon::slotReceivedData(K3Process *, char *buffer, int )
{
   //kDebug()<<"received stuff";
   char s[250],*start,*end;
   size_t len;
   start = buffer;
   while ((end = strchr(start,'\n'))) // look for '\n'
   {
      len = end-start;
      if (len>=sizeof(s))
	      len=sizeof(s)-1;
      strncpy(s,start,len);
      s[len] = '\0';
      //kDebug() << "recived: "<<s;
      if (readingpart==nfs)
         processNFSLine(s,len);
      else
         processSambaLine(s,len); // process each line
      start=end+1;
   }
   if (readingpart==nfs)
   {
      list->viewport()->update();
      list->update();
   }
   // here we could save the remaining part of line, if ever buffer
   // doesn't end with a '\n' ... but will this happen ?
}

void NetMon::update()
{
   K3Process * process = new K3Process();

   memset(&lo, 0, sizeof(lo));
   list->clear();
   /* Re-read the Contents ... */

   QString path(::getenv("PATH"));
   path += "/bin:/sbin:/usr/bin:/usr/sbin";

   rownumber=0;
   readingpart=header;
   nrpid=0;
   process->setEnvironment("PATH", path);
   connect(process,
           SIGNAL(receivedStdout(K3Process *, char *, int)),
           SLOT(slotReceivedData(K3Process *, char *, int)));
   *process << "smbstatus";
   if (!process->start(K3Process::Block,K3Process::Stdout))
      version->setText(i18n("Error: Unable to run smbstatus"));
   else if (rownumber==0) // empty result
      version->setText(i18n("Error: Unable to open configuration file \"smb.conf\""));
   else
   {
      // ok -> count the number of locked files for each pid
      for (Q3ListViewItem *row=list->firstChild();row!=0;row=row->itemBelow())
      {
//         cerr<<"NetMon::update: this should be the pid: "<<row->text(5)<<endl;
         int pid=row->text(5).toInt();
         row->setText(6,QString("%1").arg((lo)[pid]));
      }
   }
   delete process;
   process=0;

   readingpart=nfs;
   delete showmountProc;
   showmountProc=new K3Process();
   showmountProc->setEnvironment("PATH", path);
   *showmountProc<<"showmount"<<"-a"<<"localhost";
   connect(showmountProc,SIGNAL(receivedStdout(K3Process *, char *, int)),SLOT(slotReceivedData(K3Process *, char *, int)));
   //without this timer showmount hangs up to 5 minutes
   //if the portmapper daemon isn't running
   QTimer::singleShot(5000,this,SLOT(killShowmount()));
   //kDebug()<<"starting kill timer with 5 seconds";
   connect(showmountProc,SIGNAL(processExited(K3Process*)),this,SLOT(killShowmount()));
   if (!showmountProc->start(K3Process::NotifyOnExit,K3Process::Stdout)) // run showmount
   {
      delete showmountProc;
      showmountProc=0;
   }

   version->adjustSize();
   list->show();
}

void NetMon::killShowmount()
{
   //kDebug()<<"killShowmount()";
    delete showmountProc;
    showmountProc=0;
}

