/*
 *  SPDX-FileCopyrightText: 2008 Nicolas Ternisien <nicolas.ternisien@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kcm_pci.h"

#include <KPluginFactory>

#include <KAboutData>
#include <QDebug>

#include <QHBoxLayout>
#include <QTreeWidget>

#include <KLocalizedString>
#include <KSeparator>

K_PLUGIN_CLASS_WITH_JSON(KCMPci, "kcm_pci.json")

KCMPci::KCMPci(QWidget *parent, const QVariantList &)
    : KCModule(parent)
{
    KAboutData *about = new KAboutData(i18n("kcm_pci"),
                                       i18n("PCI Devices"),
                                       QString(),
                                       QString(),
                                       KAboutLicense::GPL,
                                       i18n("(c) 2008 Nicolas Ternisien"
                                            "(c) 1998 - 2002 Helge Deller"));

    about->addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
    about->addAuthor(i18n("Helge Deller"), QString(), QStringLiteral("deller@gmx.de"));
    setAboutData(about);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    tree = new QTreeWidget(this);
    layout->addWidget(tree);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setAllColumnsShowFocus(true);
    tree->setRootIsDecorated(false);
    tree->setWhatsThis(i18n("This list displays PCI information."));
}

KCMPci::~KCMPci()
{
}

void KCMPci::load()
{
    // qDebug() << "Loading PCI information..." << endl;
    GetInfo_PCI(tree);

    // Resize the column width to the maximum needed
    tree->expandAll();
    tree->resizeColumnToContents(0);
    tree->collapseAll();
}

QString KCMPci::quickHelp() const
{
    return i18n("This display shows information about your computer's PCI slots and the related connected devices.");
}

#include "kcm_pci.moc"
