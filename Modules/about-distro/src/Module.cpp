/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "Module.h"
#include "ui_Module.h"

#include <QClipboard>
#include <QIcon>
#include <QLocale>

#include <KAboutData>
#include <KCoreAddons>
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KOSRelease>
#include <KSharedConfig>

#include "CPUEntry.h"
#include "GraphicsPlatformEntry.h"
#include "GPUEntry.h"
#include "KernelEntry.h"
#include "MemoryEntry.h"
#include "PlasmaEntry.h"
#include "SectionLabel.h"
#include "Version.h"

Module::Module(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args),
    ui(new Ui::Module)
{
    KAboutData *aboutData = new KAboutData(QStringLiteral("kcm-about-distro"),
                                           i18nc("@title", "About System"),
                                           QString::fromLatin1(global_s_versionStringFull),
                                           QString(),
                                           KAboutLicense::LicenseKey::GPL_V3,
                                           i18nc("@info:credit", "Copyright 2012-2020 Harald Sitter"));

    aboutData->addAuthor(i18nc("@info:credit", "Harald Sitter"),
                         i18nc("@info:credit", "Author"),
                         QStringLiteral("sitter@kde.org"));

    setAboutData(aboutData);

    ui->setupUi(this);

    QFont font = ui->nameVersionLabel->font();
    font.setPixelSize(24);
    font.setBold(true);
    ui->nameVersionLabel->setFont(font);

    QFont fontVariant = ui->variantLabel->font();
    fontVariant.setPixelSize(18);
    fontVariant.setBold(true);
    ui->variantLabel->setFont(fontVariant);

    ui->urlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);

    // We have no help so remove the button from the buttons.
    setButtons(buttons() ^ KCModule::Help ^ KCModule::Default ^ KCModule::Apply);

    // Setup Copy to Clipboard button
    connect(ui->pushButtonCopyInfo, &QPushButton::clicked, this, &Module::copyToClipboard);
    connect(ui->pushButtonCopyInfoInEnglish, &QPushButton::clicked, this, &Module::copyToClipboardInEnglish);
    if (QLocale::system().language() == QLocale::English || QLocale::system().language() == QLocale::C) {
        ui->pushButtonCopyInfoInEnglish->hide();
    }
    ui->pushButtonCopyInfo->setShortcut(QKeySequence::Copy);

    // https://bugs.kde.org/show_bug.cgi?id=366158
    // When a KCM loads fast enough do a blocking load via the constructor.
    // Otherwise there is a noticeable rendering gap where dummy/no data is
    // shown. Makes it look bad.
    load();
}

Module::~Module()
{
    delete ui;
    qDeleteAll(m_entries);
}

void Module::load()
{
    // load is called lazily, but also from the ctor -> prevent double init.
    if (!m_entries.empty()) {
        return;
    }

    loadOSData();
    loadEntries();
}

void Module::save()
{
}

void Module::defaults()
{
}

void Module::loadOSData()
{
    // NOTE: do not include globals, otherwise kdeglobals could provide values
    //       even though we only explicitly want them from our own config.
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral("kcm-about-distrorc"),
                                                          KConfig::NoGlobals);
    KConfigGroup cg = KConfigGroup(config, "General");

    KOSRelease os;

    QString logoPath = cg.readEntry("LogoPath", os.logo());
    if (logoPath.isEmpty()) {
        logoPath = QStringLiteral("start-here-kde");
    }
    const QPixmap logo = QIcon::fromTheme(logoPath).pixmap(128, 128);
    ui->logoLabel->setPixmap(logo);

    // We allow overriding of the OS name for branding purposes.
    // For example OS Ubuntu may be rebranded as Kubuntu. Also Kubuntu Active
    // as a product brand is different from Kubuntu.
    const QString distroName = cg.readEntry("Name", os.name());
    const QString osrVersion = cg.readEntry("UseOSReleaseVersion", false)
            ? os.version()
            : os.versionId();
    const QString versionId = cg.readEntry("Version", osrVersion);
    // This creates a trailing space if versionId is empty, so trimming String
    // to remove possibly trailing spaces
    const QString distroNameVersion = QStringLiteral("%1 %2").arg(distroName, versionId).trimmed();
    ui->nameVersionLabel->setText(distroNameVersion);

    // Insert a dummy entry for debug info dumps.
    m_entries.push_back(new Entry(ki18n("Operating System:"), distroNameVersion));

    const QString variant = cg.readEntry("Variant", os.variant());
    if (variant.isEmpty()) {
        ui->variantLabel->hide();
    } else {
        ui->variantLabel->setText(variant);
    }

    const QString url = cg.readEntry("Website", os.homeUrl());
    if (url.isEmpty()) {
        ui->urlLabel->hide();
    } else {
        ui->urlLabel->setText(QStringLiteral("<a href='%1'>%1</a>").arg(url));
    }
}

void Module::loadEntries()
{
    auto addSectionHeader = [this](const QString &text)
    {
        int row = ui->infoGrid->rowCount();
        // Random sizes stolen from original UI file values :S
        ui->infoGrid->addItem(new QSpacerItem(17, 21, QSizePolicy::Minimum, QSizePolicy::Fixed), row, 1, 1, 1);
        ++row;
        ui->infoGrid->addWidget(new SectionLabel(text), row, 1, Qt::AlignLeft);
        ++row;
    };

    auto addEntriesToGrid = [this](std::vector<const Entry *> entries)
    {
        int row = ui->infoGrid->rowCount();
        for (auto entry : entries) {
            if (!entry->isValid()) {
                delete entry; // since we do not keep it around
                continue;
            }
            ui->infoGrid->addWidget(new QLabel(entry->localizedLabel()), row, 0, Qt::AlignRight);
            ui->infoGrid->addWidget(new QLabel(entry->localizedValue()), row, 1, Qt::AlignLeft);
            m_entries.push_back(entry);
            ++row;
        }
    };

    // software
    addSectionHeader(i18nc("@title:group", "Software"));
    addEntriesToGrid({
                         new PlasmaEntry(),
                         new Entry(ki18n("KDE Frameworks Version:"), KCoreAddons::versionString()),
                         new Entry(ki18n("Qt Version:"), QString::fromLatin1(qVersion())),
                         new KernelEntry(),
                         new GraphicsPlatformEntry()
                     });

    // hardware
    addSectionHeader(i18nc("@title:group", "Hardware"));
    addEntriesToGrid({
                         new CPUEntry(),
                         new MemoryEntry(),
                         new GPUEntry()
                     });
}

void Module::copyToClipboard()
{
    QString text;
    for (auto entry : m_entries) {
        text += entry->diagnosticLine(Entry::Language::System);
    }
    QGuiApplication::clipboard()->setText(text.trimmed());
}

void Module::copyToClipboardInEnglish()
{
    QString text;
    for (auto entry : m_entries) {
        text += entry->diagnosticLine(Entry::Language::English);
    }
    QGuiApplication::clipboard()->setText(text.trimmed());
}
