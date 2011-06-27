/*
 * kcmsambaimports.cpp
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
#include "kcmsambaimports.h"
#include "kcmsambaimports.moc"

#include <QLayout>
#include <QWhatsThis>

#include <klocale.h>
#include <kdialog.h>

#include <stdio.h>

ImportsView::ImportsView(QWidget * parent, KConfig *config) :
	QWidget(parent), configFile(config), list(this) {
	QBoxLayout *topLayout = new QVBoxLayout(this);
	topLayout->setMargin(KDialog::marginHint());
	topLayout->setSpacing(KDialog::spacingHint());
	topLayout->addWidget(&list);

	list.setAllColumnsShowFocus(true);
	list.setMinimumSize(425, 200);
	list.setHeaderLabels(QStringList() << i18n("Type") << i18n("Resource") << i18n("Mounted Under"));

	this->setWhatsThis(i18n("This list shows the Samba and NFS shared"
		" resources mounted on your system from other hosts. The \"Type\""
		" column tells you whether the mounted resource is a Samba or an NFS"
		" type of resource. The \"Resource\" column shows the descriptive name"
		" of the shared resource. Finally, the third column, which is labeled"
		" \"Mounted under\" shows the location on your system where the shared"
		" resource is mounted.") );

	timer.start(10000);
	QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(updateList()));
	updateList();
}

void ImportsView::updateList() {
	list.clear();
	char *e;
	char buf[250];
	QByteArray s(""), strSource, strMount, strType;
	FILE *f=popen("mount", "r");
	if (f==0)
		return;
	do {
		e=fgets(buf, 250, f);
		if (e!=0) {
			s=buf;
			if ((s.contains(" nfs ")) || (s.contains(" smbfs "))) {
				strSource=s.left(s.indexOf(" on /"));
				strMount=s.mid(s.indexOf(" on /")+4, s.length());
				if ((s.contains(" nfs ")) || (s.contains("/remote on ")))
					strType="NFS";
				else if (s.contains(" smbfs "))
					strType="SMB";
				int pos(strMount.indexOf(" type "));
				if (pos==-1)
					pos=strMount.indexOf(" read/");
				strMount=strMount.left(pos);
				QTreeWidgetItem *item = new QTreeWidgetItem(&list);
				item->setText(0, strType);
				item->setText(1, strSource);
				item->setText(2, strMount);
			};
		};
	} while (!feof(f));
	pclose(f);
}

