/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "devinfo.h"

#include <KAboutData>
#include <KLocalizedString>
#include <QGridLayout>
#include <QLabel>
#include <QSplitter>

// Plugin
#include <KPluginFactory>

#include "devicelisting.h"
#include "infopanel.h"

K_PLUGIN_CLASS_WITH_JSON(DevInfoPlugin, "devinfo.json")

DevInfoPlugin::DevInfoPlugin(QWidget *parent, const QVariantList &)
    : KCModule(parent)
{
    const KAboutData *about =
        new KAboutData(i18n("kcmdevinfo"), i18n("Device Viewer"), QStringLiteral("0.70"), QString(), KAboutLicense::GPL, i18n("(c) 2010 David Hubner"));

    setAboutData(about);

    // Layout
    layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // top
    QSplitter *split = new QSplitter(Qt::Horizontal, this);
    split->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    split->setChildrenCollapsible(false);

    InfoPanel *info = new InfoPanel(split, this);
    DeviceListing *devList = new DeviceListing(split, info, this);

    split->setStretchFactor(1, 1);

    // bottom
    QWidget *bottom = new QWidget(this);
    bottom->setContentsMargins(0, 0, 0, 0);
    bottom->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottom);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    QFont boldFont;
    boldFont.setBold(true);

    QLabel *udiLabel = new QLabel(i18n("UDI: "));
    udiLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    udiLabel->setFont(boldFont);
    udiLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    udiStatus = new QLabel(this);
    udiStatus->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    udiStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
    udiStatus->setWhatsThis(i18nc("Udi Whats This", "Shows the current device's UDI (Unique Device Identifier)"));

    // Adding
    split->addWidget(devList);
    split->addWidget(info);
    layout->addWidget(split, 0, 0);

    bottomLayout->addWidget(udiLabel);
    bottomLayout->addWidget(udiStatus);
    layout->addWidget(bottom, 1, 0, 1, 0);

    // Misc
    setButtons(Help);
    updateStatus(i18nc("no device UDI", "None"));
}

DevInfoPlugin::~DevInfoPlugin()
{
    delete layout;
}

void DevInfoPlugin::updateStatus(const QString &message)
{
    udiStatus->setText(message);
}

#include "devinfo.moc"
