/*
    SPDX-FileCopyrightText: 1998 Helge Deller deller @gmx.de

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "info.h"

#include <QHBoxLayout>
#include <QHeaderView>

#include <KLocalizedString>
#include <QDebug>
#include <QFontDatabase>

KInfoListWidget::KInfoListWidget(const QString &_title, QWidget *parent, bool _getlistbox(QTreeWidget *tree))
    : KCModule(parent)
    , title(_title)
{
    KAboutData *about = new KAboutData(QStringLiteral("kcminfo"),
                                       i18n("System Information Control Module"),
                                       QString(),
                                       QString(),
                                       KAboutLicense::GPL,
                                       i18n("(c) 2008 Nicolas Ternisien\n"
                                            "(c) 1998 - 2002 Helge Deller"));

    about->addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
    about->addAuthor(i18n("Helge Deller"), QString(), QStringLiteral("deller@kde.org"));
    setAboutData(about);

    // qDebug() << "Constructing a KInfoListWidget..." << endl;

    // setButtons(KCModule::Help);
    getlistbox = _getlistbox;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    widgetStack = new QStackedWidget(this);
    layout->addWidget(widgetStack);

    tree = new QTreeWidget(widgetStack);
    widgetStack->addWidget(tree);
    tree->setMinimumSize(200, 120);
    tree->setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont)); /* default font */
    tree->setSortingEnabled(true);
    tree->setRootIsDecorated(false);
    tree->header()->setSortIndicatorShown(true);
    tree->setWhatsThis(i18n("This list displays system information on the selected category."));

    noInfoText = new QLabel(widgetStack);
    widgetStack->addWidget(noInfoText);
    noInfoText->setAlignment(Qt::AlignCenter);
    noInfoText->setWordWrap(true);
    widgetStack->setCurrentWidget(noInfoText);
}

void KInfoListWidget::load()
{
    // qDebug() << "Loading KInfoListWidget..." << endl;

    // TODO Remove tree content before clear it
    tree->clear();

    errorString = i18nc("%1 is one of the modules of the kinfocenter, cpu info, os info, etc", "No information available about %1.", title)
        + QStringLiteral("\n\n") + DEFAULT_ERRORSTRING;

    /* No Sorting per default */
    tree->setSortingEnabled(false);

    bool ok = false;
    /* retrieve the information */
    if (getlistbox) {
        ok = (*getlistbox)(tree);
    }

    /* set default title */
    if (tree->headerItem()->columnCount() <= 1) {
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

QString KInfoListWidget::quickHelp() const
{
    return i18n(
        "All the information modules return information about a certain"
        " aspect of your computer hardware or your operating system.");
}
