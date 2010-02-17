/*
 Main Widget for showing system-dependent information.
 (But all functions in THIS FILE should be system independent !)

 (C) 1998-2003 by Helge Deller <deller@kde.org>

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
	KAboutData *about = new KAboutData("kcminfo", 0,
			ki18n("KDE Panel System Information Control Module"),
			0, KLocalizedString(), KAboutData::License_GPL,
			ki18n(	"(c) 2008 Nicolas Ternisien\n"
					"(c) 1998 - 2002 Helge Deller"));

	about->addAuthor(ki18n("Nicolas Ternisien"), KLocalizedString(), "nicolas.ternisien@gmail.com");
	about->addAuthor(ki18n("Helge Deller"), KLocalizedString(), "deller@kde.org");
	setAboutData(about);

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


