/*
 *  Copyright (C) 2008 Nicolas Ternisien <nicolas.ternisien@gmail.com>
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

#include "kcm_pci.h"

#include <QStringList>

#include <KPluginFactory>
#include <KPluginLoader>

#include <kaboutdata.h>
#include <kdialog.h>
#include <kdebug.h>

#include <QLayout>
#include <QPainter>

#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>

#include <klocale.h>
#include <kglobal.h>
#include <kseparator.h>

K_PLUGIN_FACTORY(KCMPciFactory,
		registerPlugin<KCMPci>();
)
K_EXPORT_PLUGIN(KCMPciFactory("kcm_pci"))

KCMPci::KCMPci(QWidget *parent, const QVariantList &) :
	KCModule(KCMPciFactory::componentData(), parent) {

	KAboutData *about = new KAboutData(I18N_NOOP("kcm_pci"), 0,
			ki18n("KDE PCI Information Control Module"),
			0, KLocalizedString(), KAboutData::License_GPL,
			ki18n(	"(c) 2008 Nicolas Ternisien"
					"(c) 1998 - 2002 Helge Deller"));

	about->addAuthor(ki18n("Nicolas Ternisien"), KLocalizedString(), "nicolas.ternisien@gmail.com");
	about->addAuthor(ki18n("Helge Deller"), KLocalizedString(), "deller@gmx.de");
	setAboutData(about);

	KGlobal::locale()->insertCatalog("kcm_infobase");
	
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	
	tree = new QTreeWidget(this);
	layout->addWidget(tree);
	tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
	tree->setAllColumnsShowFocus(true);
	tree->setRootIsDecorated(false);
	tree->setWhatsThis(i18n("This list displays PCI information.") );


}

KCMPci::~KCMPci() {

}

void KCMPci::load() {
	kDebug() << "Loading PCI information..." << endl;
	GetInfo_PCI(tree);

	//Resize the column width to the maximum needed
	tree->expandAll();
	tree->resizeColumnToContents( 0 );
	tree->collapseAll();
}

QString KCMPci::quickHelp() const {
	return i18n("This display shows information about your computer's PCI slots and the related connected devices.");
}

#include "kcm_pci.moc"
