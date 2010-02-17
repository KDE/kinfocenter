/*
 *  solidproc.cpp
 *
 *  Copyright (C) 2008 Ivo Anjo <knuckles@gmail.com>
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

#include "solidproc.h"

#include <QStringList>

#include <KPluginFactory>
#include <KPluginLoader>

#include <kaboutdata.h>
#include <kdialog.h>
#include <kdebug.h>

// Solid includes
#include <solid/devicenotifier.h>
#include <solid/deviceinterface.h>
#include <solid/processor.h>

#include "solidproc.moc"

K_PLUGIN_FACTORY(KCMSolidProcFactory,
    registerPlugin<KCMSolidProc>();
)
K_EXPORT_PLUGIN(KCMSolidProcFactory("kcmsolidproc"))

KCMSolidProc::KCMSolidProc(QWidget *parent, const QVariantList &)
    : KCModule(KCMSolidProcFactory::componentData(), parent)
{
    setupUi(this);
    layout()->setMargin(0);
    
    solidProcList = Solid::Device::listFromType(Solid::DeviceInterface::Processor, QString());

    // Populate processor list
    for (int i = 0; i < solidProcList.size(); i++) {
        Solid::Device device = solidProcList.at(i);
        if (!device.is<Solid::Processor>()) {
            kDebug() << "Device " << device.udi().toLatin1().constData() << " is not a processor.";
        }
        
        new QListWidgetItem(device.vendor() + (device.vendor().isEmpty()?"":" ") + \
                            device.product() + " (#" + QString::number(i) + ')', listWidgetProcessors);
    }
    
    // Connect signals to slots
    connect(listWidgetProcessors, SIGNAL(currentRowChanged(int)), this, SLOT(rowSelected(int)));
    
    // Set first processor in the list as active
    listWidgetProcessors->setCurrentItem(listWidgetProcessors->item(0));
    
    KAboutData *about =
    new KAboutData("kcmsolidproc", 0,
        ki18n("KCM Solid Processor Information"),
        0, KLocalizedString(), KAboutData::License_GPL,
        ki18n("(c) 2008 Ivo Anjo"));

    about->addAuthor(ki18n("Ivo Anjo"), KLocalizedString(), "knuckles@gmail.com");
    setAboutData(about);
}

void KCMSolidProc::rowSelected(int newRow)
{
    // Display info about selected processor
    Solid::Device device = solidProcList.at(newRow);
    Solid::Processor *processor = device.as<Solid::Processor>();
    
    lineVendor->setText(device.vendor());
    labelVendor->setVisible(!device.vendor().isEmpty());
    lineVendor->setVisible(!device.vendor().isEmpty());
    
    lineModel->setText(device.product());
    
    lineSpeed->setText(QString::number(processor->maxSpeed()));
    labelSpeed->setVisible(processor->maxSpeed() != 0);
    lineSpeed->setVisible(processor->maxSpeed() != 0);
    
    Solid::Processor::InstructionSets extensions = processor->instructionSets();
    QStringList ext;
    if (extensions & Solid::Processor::IntelMmx)  ext << "MMX";
    if (extensions & Solid::Processor::IntelSse)  ext << "SSE";
    if (extensions & Solid::Processor::IntelSse2) ext << "SSE2";
    if (extensions & Solid::Processor::IntelSse3) ext << "SSE3";
    if (extensions & Solid::Processor::IntelSse4) ext << "SSE4";
    if (extensions & Solid::Processor::Amd3DNow)  ext << "3DNow!";
    if (extensions & Solid::Processor::AltiVec)   ext << "AltiVec";
    
    lineExtensions->setText(ext.join(" "));
}
