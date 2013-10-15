
/*
Copyright 1998  Helge Deller deller@gmx.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "info.h"		

#include <QLayout>
#include <QProcess>
#include <QLabel>
#include <QHBoxLayout>
#include <QTextStream>
#include <QHeaderView>

#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kdialog.h>
#include <klocale.h>
#include <kdebug.h>

KInfoListWidget::KInfoListWidget(const KComponentData &inst, const QString &_title, QWidget *parent, bool _getlistbox(QTreeWidget *tree) ) :
	KCModule(inst, parent), title(_title) {
	KAboutData *about = new KAboutData("kcminfo", QString(),
			i18n("System Information Control Module"),
			QString(), QString(), KAboutData::License_GPL,
			i18n(	"(c) 2008 Nicolas Ternisien\n"
					"(c) 1998 - 2002 Helge Deller"));

	about->addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
	about->addAuthor(i18n("Helge Deller"), QString(), QStringLiteral("deller@kde.org"));
	setAboutData(about);

	KGlobal::locale()->insertCatalog("kcm_infobase");
	
	kDebug() << "Constructing a KInfoListWidget..." << endl;

	//setButtons(KCModule::Help);
	getlistbox = _getlistbox;
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setSpacing(KDialog::spacingHint());
	layout->setMargin(0);
	widgetStack = new QStackedWidget(this);
	layout->addWidget(widgetStack);

	tree = new QTreeWidget(widgetStack);
	widgetStack->addWidget(tree);
	tree->setMinimumSize(200, 120);
	tree->setFont(KGlobalSettings::generalFont()); /* default font */
	tree->setSortingEnabled(true);
	tree->setRootIsDecorated(false);
	tree->header()->setSortIndicatorShown(true);
	tree->setWhatsThis(i18n("This list displays system information on the selected category.") );
	
	noInfoText = new QLabel(widgetStack);
	widgetStack->addWidget(noInfoText);
	noInfoText->setAlignment(Qt::AlignCenter);
	noInfoText->setWordWrap( true);
	widgetStack->setCurrentWidget(noInfoText);
	
}

void KInfoListWidget::load() {
	kDebug() << "Loading KInfoListWidget..." << endl;

	//TODO Remove tree content before clear it
	tree->clear();

	errorString = i18nc("%1 is one of the modules of the kinfocenter, cpu info, os info, etc", "No information available about %1.", title) + QLatin1String("\n\n") + DEFAULT_ERRORSTRING;

	/* No Sorting per default */
	tree->setSortingEnabled(false); 

	bool ok = false;
	/* retrieve the information */
	if (getlistbox) {
		ok = (*getlistbox)(tree); 
	}

	/* set default title */
	if (tree->headerItem()->columnCount()<=1) {
		QStringList headers;
		headers << title;
		tree->setHeaderLabels(headers);
	}

	if (ok) {
		widgetStack->setCurrentWidget(tree);
	} else {
		noInfoText->setText(errorString);
		widgetStack->setCurrentWidget(noInfoText);
	}
	
	tree->resizeColumnToContents(0);

	emit changed(false);
}

QString KInfoListWidget::quickHelp() const {
	return i18n("All the information modules return information about a certain"
		" aspect of your computer hardware or your operating system.");
}


