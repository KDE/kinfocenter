/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "soldevice.h"

#include <solid/deviceinterface.h>
#include <solid/processor.h>

#include <QTreeWidget>

#include <KLocalizedString>

// Local
#include "qvlistlayout.h"

SolDevice::SolDevice(const Solid::DeviceInterface::Type &type)
    : QTreeWidgetItem()
    , deviceSet(false)
    , deviceTypeHolder(type)
{
    setText(0, Solid::DeviceInterface::typeToString(type));
}

SolDevice::SolDevice(QTreeWidgetItem *parent)
    : QTreeWidgetItem(parent)
    , deviceSet(false)
    , deviceTypeHolder(Solid::DeviceInterface::Unknown)
{
}

SolDevice::SolDevice(const Solid::DeviceInterface::Type &type, const QString &typeName)
    : QTreeWidgetItem()
    , deviceSet(false)
    , deviceTypeHolder(type)
{
    setText(0, typeName);

    setDefaultListing(type);
}

SolDevice::SolDevice(QTreeWidgetItem *parent, const Solid::Device &device)
    : QTreeWidgetItem(parent)
    , deviceTypeHolder(Solid::DeviceInterface::Unknown)
    , tiedDevice(device)
{
    deviceSet = device.isValid();
    setDefaultDeviceText();
    setDefaultDeviceIcon();
    setDefaultDeviceToolTip();
}

// Sets

void SolDevice::setDefaultListing(const Solid::DeviceInterface::Type &type)
{
    createDeviceChildren<SolDevice>(this, QString(), type);
}

void SolDevice::setDefaultDeviceText()
{
    QString ddtString = i18nc("unknown device", "Unknown");

    if (deviceSet) {
        ddtString = tiedDevice.product();
        if (tiedDevice.isDeviceInterface(Solid::DeviceInterface::StorageVolume)) {
            QString label = SolDevice::udi().section(QStringLiteral("/"), -1, -1);
            if (!label.isEmpty()) {
                ddtString = label;
            }
        }
    }
    setText(0, ddtString);
}

void SolDevice::setDefaultDeviceIcon()
{
    QIcon ddiString = QIcon::fromTheme(QStringLiteral("kde"));

    if (deviceSet) {
        ddiString = QIcon(tiedDevice.icon());
    }
    setDeviceIcon(ddiString);
}

void SolDevice::setDefaultDeviceToolTip()
{
    QString ddttString = i18nc("Default device tooltip", "A Device");

    if (deviceSet) {
        ddttString = tiedDevice.description();
    }
    setDeviceToolTip(ddttString);
}

void SolDevice::setDeviceIcon(const QIcon &icon)
{
    setIcon(0, icon);
}

void SolDevice::setDeviceText(const QString &text)
{
    setText(0, text);
}

void SolDevice::setDeviceToolTip(const QString &toolTipText)
{
    setToolTip(0, toolTipText);
}

// Gets

QVListLayout *SolDevice::infoPanelLayout()
{
    deviceInfoLayout = new QVListLayout();
    return deviceInfoLayout;
}

QIcon SolDevice::deviceIcon() const
{
    return icon(0);
}

Solid::DeviceInterface::Type SolDevice::deviceType() const
{
    return deviceTypeHolder;
}

Solid::Device *SolDevice::device()
{
    return &tiedDevice;
}

QString SolDevice::udi() const
{
    return tiedDevice.udi();
}

// Is

bool SolDevice::isDeviceSet()
{
    return deviceSet;
}

bool SolDevice::operator<(const QTreeWidgetItem &other) const
{
    const SolDevice *otherDevice = dynamic_cast<const SolDevice *>(&other);
    if (otherDevice) {
        if (deviceType() != otherDevice->deviceType()) {
            return deviceType() < otherDevice->deviceType();
        }
        switch (deviceType()) {
        case Solid::DeviceInterface::StorageVolume: {
            // Storage volumes are sorted in ascending order (i.e. sda, sda1, sda2...)
            return text(0) > other.text(0);
        }
        default:
            break;
        }
    }
    return text(0) < other.text(0);
}
