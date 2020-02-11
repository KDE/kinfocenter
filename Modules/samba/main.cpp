/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <KCModule>
#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>
#include <KTitleWidget>

#include "ksambasharemodel.h"
#include "smbmountmodel.h"

class SambaContainer : public KCModule {
Q_OBJECT
public:
    SambaContainer(QWidget *parent = nullptr, const QVariantList &list = QVariantList());
    ~SambaContainer() override = default;

private:
    QTableView *addTableView(const QString &localizedLabel, QAbstractListModel *model);
};

K_PLUGIN_FACTORY(SambaFactory,
		registerPlugin<SambaContainer>();
)

SambaContainer::SambaContainer(QWidget *parent, const QVariantList &)
    : KCModule(parent)
{
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
    about->addAuthor(i18n("Harald Sitter"), QString(), QStringLiteral("sitter@kde.org"));
	setAboutData(about);

    QVBoxLayout *layout = new QVBoxLayout(this);
    Q_ASSERT(this->layout());
    setLayout(layout);

    addTableView(i18nc("@title/group", "Exported Shares"), new KSambaShareModel(this));

    auto importsView = addTableView(i18nc("@title/group", "Mounted Shares"), new SmbMountModel(this));
    importsView->horizontalHeader()->setSectionResizeMode(static_cast<int>(SmbMountModel::ColumnRole::Accessible),
                                                          QHeaderView::ResizeToContents);

    setButtons(Help);
}

QTableView *SambaContainer::addTableView(const QString &localizedLabel, QAbstractListModel *model)
{
    auto title = new KTitleWidget(this);
    title->setText(localizedLabel);
    title->setLevel(2);
    layout()->addWidget(title);

    auto view = new QTableView(this);
    layout()->addWidget(view);
    view->setModel(model);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->horizontalHeader()->reset();
    view->horizontalHeader()->setVisible(true);
    view->verticalHeader()->setVisible(false);

    return view;
}

#include "main.moc"
