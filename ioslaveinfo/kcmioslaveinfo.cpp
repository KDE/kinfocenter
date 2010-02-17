/*
 * kcmioslaveinfo.cpp
 *
 * Copyright 2001 Alexander Neundorf <neundorf@kde.org>
 * Copyright 2001 George Staikos  <staikos@kde.org>
 * Copyright 2008 Pino Toscano <pino@kde.org>
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

#include <QLabel>
#include <QLayout>
#include <QComboBox>

#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kprotocolinfo.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <dom/dom_node.h>
#include <dom/html_element.h>

#include "kcmioslaveinfo.h"

K_PLUGIN_FACTORY(SlaveFactory, registerPlugin<KCMIOSlaveInfo>();)
K_EXPORT_PLUGIN( SlaveFactory("kcmioslaveinfo"))

KCMIOSlaveInfo::KCMIOSlaveInfo(QWidget *parent, const QVariantList &) :
	KCModule(SlaveFactory::componentData(), parent) {
	QVBoxLayout *layout=new QVBoxLayout(this);
	layout->setMargin(0);

	setQuickHelp(i18n("Overview of the installed ioslaves and supported protocols."));
	setButtons(KCModule::Help);
	
	QHBoxLayout* selectionLayout = new QHBoxLayout();
	
	ioSlaves = new QComboBox(this);
	QLabel* ioSlavesLabel = new QLabel(i18n("Select the protocol documentation to display:"));
	ioSlavesLabel->setBuddy(ioSlaves);
	
	connect(ioSlaves, SIGNAL(currentIndexChanged(const QString&) ), SLOT( showInfo(const QString&) ));

	selectionLayout->addWidget(ioSlavesLabel);
	selectionLayout->addWidget(ioSlaves);
	
	layout->addLayout(selectionLayout);
	

	m_info = new KHTMLPart(this, this);
	m_info->setJScriptEnabled(false);
	m_info->setJavaEnabled(false);
	m_info->setMetaRefreshEnabled(false);
	m_info->setPluginsEnabled(false);

	connect(m_info, SIGNAL(completed()), this, SLOT(loadingCompleted()));
	connect(m_info->browserExtension(), SIGNAL(openUrlRequestDelayed(const KUrl&, const KParts::OpenUrlArguments&, const KParts::BrowserArguments&)),
	        this, SLOT(openUrl(const KUrl&, const KParts::OpenUrlArguments&, const KParts::BrowserArguments&)));
	
	layout->addWidget(m_info->view());

	QStringList protocols=KProtocolInfo::protocols();
	protocols.sort();
	foreach(const QString &proto, protocols) {
		//m_ioslavesLb->addItem(new QListWidgetItem ( SmallIcon( KProtocolInfo::icon( proto )), proto, m_ioslavesLb));
		ioSlaves->addItem(SmallIcon( KProtocolInfo::icon( proto )), proto);
	};
	//m_ioslavesLb->sort();
	//m_ioslavesLb->setSelected(0, true);

	KAboutData *about = new KAboutData(I18N_NOOP("kcmioslaveinfo"), 0,
			ki18n("KDE Panel System Information Control Module"),
			0, KLocalizedString(), KAboutData::License_GPL,
			ki18n("(c) 2001 - 2002 Alexander Neundorf"));

	about->addAuthor(ki18n("Alexander Neundorf"), KLocalizedString(), "neundorf@kde.org");
	about->addAuthor(ki18n("George Staikos"), KLocalizedString(), "staikos@kde.org");
	setAboutData(about);

}

/**
 * Big Hack to only select content of the help documentation
 * The HTML content is cut by removing the all DIV blocks but the class="article" one
 */
void KCMIOSlaveInfo::selectHelpBody() {
	DOM::Document document = m_info->document();
	m_info->view()->setUpdatesEnabled(true);
	if (document.isNull()) {
		return;
	}
	const DOM::NodeList bodylist = document.getElementsByTagName("body");
	if (bodylist.length() != 1) {
		return;
	}
	DOM::Node body = bodylist.item(0);
	const DOM::NodeList bodyChildren = body.childNodes();
	for (unsigned long i = 0; i < bodyChildren.length(); ++i) {
		const DOM::Node child = bodyChildren.item(i);
		if ((child.nodeType() != DOM::Node::ELEMENT_NODE)
		    || (child.nodeName().lower() != DOM::DOMString("div"))) {
			continue;
		}
		const DOM::HTMLElement el = child;
		if (el.className() == DOM::DOMString("article")) {
			continue;
		}
		--i;
		body.removeChild(child);
	}
}

void KCMIOSlaveInfo::showInfo(const QString& protocol) {
	//get X-DocPath entry "foo/index.html" from .protocol file
	QString docPath=KProtocolInfo::docPath (protocol);
	QString file = docPath;
	//check for "foo/index.docbook" to get rid of the error dialog from KShortUriFilter
	//the error message is displayed in the dialog window here
	file.replace ( QString(".html"), QString(".docbook") );
	file = KGlobal::locale()->langLookup(file);
	if (!file.isEmpty()) {
		m_info->view()->setUpdatesEnabled(false);
		m_info->openUrl(KUrl(QString( "help:/%1" ).arg(docPath)));
		return;
	}
	m_info->begin();
	m_info->write(i18n("<html><body><p style='text-align:center'>No documentation available for the '%1:/' protocol.</p></body></html>", protocol));
	m_info->end();
}

void KCMIOSlaveInfo::loadingCompleted() {
	selectHelpBody();
}

void KCMIOSlaveInfo::openUrl(const KUrl& url, const KParts::OpenUrlArguments&, const KParts::BrowserArguments&) {
	if (url.protocol() == QLatin1String("mailto")) {
		KToolInvocation::invokeMailer(url);
	} else {
		KToolInvocation::invokeBrowser(url.url());
	}
}

#include "kcmioslaveinfo.moc"

