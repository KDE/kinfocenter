/*
  Copyright (C) 2012 Harald Sitter <apachelogger@ubuntu.com>

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

#include <QShortcut>

#include <KAboutData>
#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KIcon>
#include <KPluginFactory>
#include <KStandardDirs>
#include <KToolInvocation>

#include <solid/device.h>
#include <solid/processor.h>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#endif
#include <sys/utsname.h>

#include "LSBRelease.h"
#include "Version.h"

K_PLUGIN_FACTORY_DECLARATION(KcmAboutDistroFactory);

static qlonglong calculateTotalRam()
{
    qlonglong ret = -1;
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0)
        ret = info.totalram;
#endif
    return ret;
}

Module::Module(QWidget *parent, const QVariantList &args) :
    KCModule(KcmAboutDistroFactory::componentData(), parent, args),
    ui(new Ui::Module)
{
    KAboutData *about = new KAboutData("kcm-about-distro", 0,
                                       ki18n("About Distribution"),
                                       global_s_versionStringFull,
                                       KLocalizedString(),
                                       KAboutData::License_GPL_V3,
                                       ki18n("Copyright 2012 Harald Sitter"),
                                       KLocalizedString(), QByteArray(),
                                       "apachelogger@ubuntu.com");

    about->addAuthor(ki18n("Harald Sitter"), ki18n("Author"), "apachelogger@ubuntu.com");
    setAboutData(about);

    ui->setupUi(this);

    QFont font = ui->nameVersionLabel->font();
    font.setPixelSize(24);
    font.setBold(true);
    ui->nameVersionLabel->setFont(font);

    ui->urlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);

    // We have no help so remove the button from the buttons.
    setButtons(buttons() ^ KCModule::Help ^ KCModule::Default ^ KCModule::Apply);

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onStyle()));
}

Module::~Module()
{
    delete ui;
}

void Module::load()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig("kcm-about-distrorc");
    KConfigGroup cg = KConfigGroup(config, "General");

    QString logoPath = cg.readEntry("LogoPath", QString());
    QPixmap logo;
    if (logoPath.isEmpty())
        logo = KIcon("start-here-kde").pixmap(128, 128);
    else
        logo = QPixmap(logoPath);
    ui->logoLabel->setPixmap(logo);

    QString url = cg.readEntry("Website", QString());
    if (url.isEmpty())
        ui->urlLabel->hide();
    else
        ui->urlLabel->setText(QString("<a href='%1'>%1</a>").arg(url));

    LSBRelease lsb;
    // We allow overriding of the LSB name for branding purposes.
    // For example LSB Ubuntu may be rebranded as Kubuntu. Also Kubuntu Active
    // as a product brand is different from Kubuntu.
    QString distroName = cg.readEntry("Name", lsb.id());
    ui->nameVersionLabel->setText(QString("%1 %2").arg(distroName, lsb.release()));

    ui->kdeLabel->setText(QLatin1String(KDE::versionString()));
    ui->qtLabel->setText(qVersion());

    struct utsname utsName;
    if(uname(&utsName) != 0) {
        ui->kernel->hide();
        ui->kernelLabel->hide();
    } else
        ui->kernelLabel->setText(utsName.release);

    const int bits = QT_POINTER_SIZE == 8 ? 64 : 32;
    ui->bitsLabel->setText(i18n("%1-bit", QString::number(bits)));

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
    for (auto it = processorMap.constBegin(); it != processorMap.constEnd(); ++it) {
        const int count = it.value();
        QString name = it.key();
        name.replace(QString("(TM)"), QChar(8482));
        name.replace(QString("(R)"), QChar(174));
        name = name.simplified();
        names.append(QString::fromUtf8("%1 × %2").arg(count).arg(name));
    }
    ui->processorLabel->setText(names.join(", "));

    const qlonglong totalRam = calculateTotalRam();
    ui->memoryLabel->setText(totalRam > 0
                             ? i18n("%1 of RAM", KGlobal::locale()->formatByteSize(totalRam))
                             : i18nc("Unknown amount of RAM", "Unknown"));
}

void Module::save()
{
}

void Module::defaults()
{
}

void Module::onStyle()
{
    KToolInvocation::invokeBrowser("http://www.youtube.com/watch?v=CX_4aGQWw_4");
}
