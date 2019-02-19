/*
  Copyright (C) 2012-2014 Harald Sitter <apachelogger@ubuntu.com>

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

#include "Module.h"
#include "ui_Module.h"

#include <QClipboard>
#include <QIcon>
#include <QLocale>
#include <QStandardPaths>

#include <KAboutData>
#include <KCoreAddons>
#include <KConfig>
#include <KConfigGroup>
#include <KDesktopFile>
#include <KFormat>
#include <KLocalizedString>
#include <KSharedConfig>

#include <solid/device.h>
#include <solid/processor.h>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#elif defined(Q_OS_FREEBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#include <sys/utsname.h>

#include "OSRelease.h"
#include "Version.h"

static qlonglong calculateTotalRam()
{
    qlonglong ret = -1;
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0)
        // manpage "sizes are given as multiples of mem_unit bytes"
        ret = info.totalram * info.mem_unit;
#elif defined(Q_OS_FREEBSD)
    /* Stuff for sysctl */
    size_t len;

    unsigned long memory;
    len = sizeof(memory);
    sysctlbyname("hw.physmem", &memory, &len, NULL, 0);

    ret = memory;
#endif
    return ret;
}

Module::Module(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args),
    ui(new Ui::Module)
{
    KAboutData *aboutData = new KAboutData(QStringLiteral("kcm-about-distro"),
                                           i18nc("@title", "About Distribution"),
                                           QString::fromLatin1(global_s_versionStringFull),
                                           QString(),
                                           KAboutLicense::LicenseKey::GPL_V3,
                                           i18nc("@info:credit", "Copyright 2012-2014 Harald Sitter"));

    aboutData->addAuthor(i18nc("@info:credit", "Harald Sitter"),
                         i18nc("@info:credit", "Author"),
                         QStringLiteral("apachelogger@kubuntu.org"));

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
    if (QLocale::system().language() == QLocale::English) {
        ui->pushButtonCopyInfoInEnglish->hide();
    }
    ui->pushButtonCopyInfo->setShortcut(QKeySequence::Copy);

    // https://bugs.kde.org/show_bug.cgi?id=366158
    // When a KCM loads fast enough do a blocking load via the constructor.
    // Otherwise there is a notciable rendering gap where dummy/no data is
    // shown. Makes it look bad.
    load();
}

Module::~Module()
{
    delete ui;
}

void Module::load()
{
    labelsForClipboard.clear();
    englishTextForClipboard = QStringLiteral("");
    loadSoftware();
    loadHardware();
}

void Module::save()
{
}

void Module::defaults()
{
}

void Module::loadSoftware()
{
    // NOTE: do not include globals, otherwise kdeglobals could provide values
    //       even though we only explicitly want them from our own config.
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral("kcm-about-distrorc"),
                                                          KConfig::NoGlobals);
    KConfigGroup cg = KConfigGroup(config, "General");

    OSRelease os;

    QString logoPath = cg.readEntry("LogoPath", os.logo);
    if (logoPath.isEmpty()) {
        logoPath = QStringLiteral("start-here-kde");
    }
    const QPixmap logo = QIcon::fromTheme(logoPath).pixmap(128, 128);
    ui->logoLabel->setPixmap(logo);

    // We allow overriding of the OS name for branding purposes.
    // For example OS Ubuntu may be rebranded as Kubuntu. Also Kubuntu Active
    // as a product brand is different from Kubuntu.
    const QString distroName = cg.readEntry("Name", os.name);
    const QString versionId = cg.readEntry("Version", os.versionId);
    const QString distroNameVersion = QStringLiteral("%1 %2").arg(distroName, versionId);
    ui->nameVersionLabel->setText(distroNameVersion);

    const auto dummyDistroDescriptionLabel = new QLabel(i18nc("@title:row", "Operating System:"), this);
    dummyDistroDescriptionLabel->hide();
    labelsForClipboard << qMakePair(dummyDistroDescriptionLabel, ui->nameVersionLabel);
    englishTextForClipboard += QStringLiteral("Operating System: %1\n").arg(distroNameVersion);

    const QString variant = cg.readEntry("Variant", os.variant);
    if (variant.isEmpty()) {
        ui->variantLabel->hide();
    } else {
        ui->variantLabel->setText(variant);
    }

    const QString url = cg.readEntry("Website", os.homeUrl);
    if (url.isEmpty()) {
        ui->urlLabel->hide();
    } else {
        ui->urlLabel->setText(QStringLiteral("<a href='%1'>%1</a>").arg(url));
    }

    // Since Plasma version detection isn't based on a library query it can fail
    // in weird cases; instead of admitting defeat we simply hide everything :P
    const QString plasma = plasmaVersion();
    if (plasma.isEmpty()) {
        ui->plasma->hide();
        ui->plasmaLabel->hide();
    } else {
        ui->plasmaLabel->setText(plasma);
        labelsForClipboard << qMakePair(ui->plasma, ui->plasmaLabel);
        englishTextForClipboard += QStringLiteral("KDE Plasma Version: %1\n").arg(plasma);
    }

    const QString frameworksVersion = KCoreAddons::versionString();
    ui->frameworksLabel->setText(frameworksVersion);
    labelsForClipboard << qMakePair(ui->frameworksLabelKey, ui->frameworksLabel);
    englishTextForClipboard += QStringLiteral("KDE Frameworks Version: %1\n").arg(frameworksVersion);

    const QString qversion = QString::fromLatin1(qVersion());
    ui->qtLabel->setText(qversion);
    labelsForClipboard << qMakePair(ui->qt, ui->qtLabel);
    englishTextForClipboard += QStringLiteral("Qt Version: %1\n").arg(qversion);
}

void Module::loadHardware()
{
    struct utsname utsName;
    if(uname(&utsName) != 0) {
        ui->kernel->hide();
        ui->kernelLabel->hide();
    } else {
        QString kernelVersion = QString::fromLatin1(utsName.release);
        ui->kernelLabel->setText(kernelVersion);
        labelsForClipboard << qMakePair(ui->kernel, ui->kernelLabel);
        englishTextForClipboard += QStringLiteral("Kernel Version: %1\n").arg(kernelVersion);
    }

    const int bits = QT_POINTER_SIZE == 8 ? 64 : 32;
    const QString bitsStr = QString::number(bits);
    ui->bitsLabel->setText(i18nc("@label %1 is the CPU bit width (e.g. 32 or 64)",
                                 "%1-bit", bitsStr));
    labelsForClipboard << qMakePair(ui->bitsKey, ui->bitsLabel);
    englishTextForClipboard += QStringLiteral("OS Type: %1-bit\n").arg(bitsStr);

    const QList<Solid::Device> list = Solid::Device::listFromType(Solid::DeviceInterface::Processor);
    ui->processor->setText(i18np("Processor:", "Processors:", list.count()));
    // Format processor string
    // Group by processor name
    QMap<QString, int> processorMap;
    Q_FOREACH(const Solid::Device &device, list) {
        const QString name = device.product();
        auto it = processorMap.find(name);
        if (it == processorMap.end()) {
            processorMap.insert(name, 1);
        } else {
            ++it.value();
        }
    }
    // Create a formatted list of grouped processors
    QStringList names;
    names.reserve(processorMap.count());
    for (auto it = processorMap.constBegin(); it != processorMap.constEnd(); ++it) {
        const int count = it.value();
        QString name = it.key();
        name.replace(QStringLiteral("(TM)"), QChar(8482));
        name.replace(QStringLiteral("(R)"), QChar(174));
        name = name.simplified();
        names.append(QStringLiteral("%1 × %2").arg(count).arg(name));
    }

    const QString processorLabel = names.join(QStringLiteral(", "));
    ui->processorLabel->setText(processorLabel);
    if (ui->processorLabel->text().isEmpty()) {
        ui->processor->setHidden(true);
        ui->processorLabel->setHidden(true);
    } else {
        labelsForClipboard << qMakePair(ui->processor, ui->processorLabel);
        englishTextForClipboard += QStringLiteral("Processors: %1\n").arg(processorLabel);
    }

    const qlonglong totalRam = calculateTotalRam();
    const QString memoryLabel = totalRam > 0
                             ? i18nc("@label %1 is the formatted amount of system memory (e.g. 7,7 GiB)",
                                     "%1 of RAM", KFormat().formatByteSize(totalRam))
                             : i18nc("Unknown amount of RAM", "Unknown");
    ui->memoryLabel->setText(memoryLabel);
    labelsForClipboard << qMakePair(ui->memory, ui->memoryLabel);
    englishTextForClipboard += QStringLiteral("Memory: %1\n").arg(KFormat().formatByteSize(totalRam));
}

void Module::copyToClipboard()
{
    QString text;
    // note that this loop does not necessarily represent the same order as in the GUI
    for (auto labelPair : qAsConst(labelsForClipboard)) {
        const auto valueLabel = labelPair.second;
        if (!valueLabel->isHidden()) {
            const auto descriptionLabelText = labelPair.first->text();
            const auto valueLabelText = valueLabel->text();
            text += i18nc("%1 is a label already including a colon, %2 is the corresponding value", "%1 %2", descriptionLabelText, valueLabelText) + QStringLiteral("\n");
        }
    }

    QGuiApplication::clipboard()->setText(text);
}

void Module::copyToClipboardInEnglish()
{
    QGuiApplication::clipboard()->setText(englishTextForClipboard);
}

QString Module::plasmaVersion() const
{
    const QStringList &filePaths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                             QStringLiteral("xsessions/plasma.desktop"));

    if (filePaths.length() < 1) {
        return QString();
    }

    // Despite the fact that there can be multiple desktop files we simply take
    // the first one as users usually don't have xsessions/ in their $HOME
    // data location, so the first match should (usually) be the only one and
    // reflect the plasma session run.
    KDesktopFile desktopFile(filePaths.first());
    return desktopFile.desktopGroup().readEntry("X-KDE-PluginInfo-Version", QString());
}
