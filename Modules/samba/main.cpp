/*
 * main.cpp for the samba kcontrol module
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

#include <QTabWidget>

#include <kaboutdata.h>
#include <kcmodule.h>
#include <KLocalizedString>
#include <QVBoxLayout>
#include "kcmsambaimports.h"
#include "kcmsambalog.h"
#include "kcmsambastatistics.h"
#include "ksmbstatus.h"
#include <KPluginFactory>

class SambaContainer : public KCModule {
Q_OBJECT
public:
    SambaContainer(QWidget *parent=nullptr, const QVariantList &list = QVariantList());
	virtual ~SambaContainer();
	void load() override;
	void save() override;

private:
	KConfig config;
	QTabWidget tabs;
	NetMon status;
	ImportsView imports;
	LogView logView;
	StatisticsView statisticsView;
};

K_PLUGIN_FACTORY(SambaFactory,
		registerPlugin<SambaContainer>();
)

SambaContainer::SambaContainer(QWidget *parent, const QVariantList&) :
	KCModule(parent), config(QStringLiteral("kcmsambarc")), tabs(this), status(&tabs, &config), imports(&tabs, &config), logView(&tabs, &config), statisticsView(&tabs, &config) {
	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(&tabs);
	tabs.addTab(&status, i18n("&Exports"));
	tabs.addTab(&imports, i18n("&Imports"));
	tabs.addTab(&logView, i18n("&Log"));
	tabs.addTab(&statisticsView, i18n("&Statistics"));
	connect(&logView, &LogView::contentsChanged, &statisticsView, &StatisticsView::setListInfo);
	setButtons(Help);

	setQuickHelp(i18n("<p>The Samba and NFS Status Monitor is a front end to the programs"
		" <em>smbstatus</em> and <em>showmount</em>. Smbstatus reports on current"
		" Samba connections, and is part of the suite of Samba tools, which"
		" implements the SMB (Session Message Block) protocol, also called the"
		" NetBIOS or LanManager protocol. This protocol can be used to provide"
		" printer sharing or drive sharing services on a network including"
		" machines running the various flavors of Microsoft Windows.</p>"));

	KAboutData *about = new KAboutData(i18n("kcmsamba"),
			i18n("System Information Control Module"),
			QString(), QString(), KAboutLicense::GPL,
			i18n("(c) 2002 KDE Information Control Module Samba Team"));
	about->addAuthor(i18n("Michael Glauche"), QString(), QStringLiteral("glauche@isa.rwth-aachen.de"));
	about->addAuthor(i18n("Matthias Hoelzer"), QString(), QStringLiteral("hoelzer@kde.org"));
	about->addAuthor(i18n("David Faure"), QString(), QStringLiteral("faure@kde.org"));
	about->addAuthor(i18n("Harald Koschinski"), QString(), QStringLiteral("Harald.Koschinski@arcormail.de"));
	about->addAuthor(i18n("Wilco Greven"), QString(), QStringLiteral("greven@kde.org"));
	about->addAuthor(i18n("Alexander Neundorf"), QString(), QStringLiteral("neundorf@kde.org"));
	setAboutData(about);
}

SambaContainer::~SambaContainer() {
	save();
}

void SambaContainer::load() {
	status.loadSettings();
	imports.loadSettings();
	logView.loadSettings();
	statisticsView.loadSettings();
}

void SambaContainer::save() {
	status.saveSettings();
	imports.saveSettings();
	logView.saveSettings();
	statisticsView.saveSettings();
	config.sync();
}

#include "main.moc"
