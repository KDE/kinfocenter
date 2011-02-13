/* 	
 info_svr4.cpp

 UNIX System V Release 4 specific Information about the Hardware.
 Appropriate for SCO OpenServer and UnixWare.
 Written 20-Feb-99 by Ronald Joe Record (rr@sco.com)
 Initially based on info_sgi.cpp
 */

#define INFO_DEV_SNDSTAT "/dev/sndstat"

#include <sys/systeminfo.h>

/*  all following functions should return true, when the Information 
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

bool GetInfo_ReadfromFile(QListView *lBox, char *Name, char splitchar) {
	QString str;
	char buf[512];

	QFile *file = new QFile(Name);
	QListViewItem* olditem = 0;

	if (!file->open(QIODevice::ReadOnly)) {
		delete file;
		return false;
	}

	while (file->readLine(buf, sizeof(buf)-1) > 0) {
		if (strlen(buf)) {
			char *p=buf;
			if (splitchar!=0) /* remove leading spaces between ':' and the following text */
				while (*p) {
					if (*p==splitchar) {
						*p++ = ' ';
						while (*p==' ')
							++p;
						*(--p) = splitchar;
						++p;
					} else
						++p;
				}

			QString s1 = QString::fromLocal8Bit(buf);
			QString s2 = s1.mid(s1.find(splitchar)+1);

			s1.truncate(s1.find(splitchar));
			if (!(s1.isEmpty() || s2.isEmpty()))
				olditem = new QListViewItem(lBox, olditem, s1, s2);
		}
	}
	file->close();

	delete file;
	return true;
}

bool GetInfo_IRQ(QListView *) {
	return false;
}

bool GetInfo_DMA(QListView *) {
	return false;
}

bool GetInfo_PCI(QTreeWidget* tree) {
	char buf[256];

	sysinfo(SI_BUSTYPES, buf, sizeof(buf));
	QStringList list;
	list << QString::fromLocal8Bit(buf);
	new QTreeWidgetItem(tree, list);
	return true;
}

bool GetInfo_IO_Ports(QListView *) {
	return false;
}

bool GetInfo_Sound(QListView *lBox) {
	if (GetInfo_ReadfromFile(lBox, INFO_DEV_SNDSTAT, 0))
		return true;
	else
		return false;
}

bool GetInfo_SCSI(QListView *) {
	return false;
}

bool GetInfo_Partitions(QListView *) {
	return false;
}

bool GetInfo_XServer_and_Video(QListView *lBox) {
	return GetInfo_XServer_Generic(lBox);
}

