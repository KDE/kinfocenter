/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef SOLDEVICE
#define SOLDEVICE

// QT
#include <QDebug>
#include <QTreeWidgetItem>

// Solid
#include <solid/device.h>
// KDE

class QVListLayout;

class SolDevice : public QTreeWidgetItem
{
public:
    SolDevice(const Solid::DeviceInterface::Type &);
    SolDevice(const Solid::DeviceInterface::Type &, const QString &);
    SolDevice(QTreeWidgetItem *);
    SolDevice(QTreeWidgetItem *, const Solid::Device &);

    QIcon deviceIcon() const;
    Solid::Device *device();
    Solid::DeviceInterface::Type deviceType() const;

    template<class IFace>
    const IFace *interface()
    {
        if (deviceSet) {
            IFace *dev = tiedDevice.as<const IFace>();
            if (!dev) {
                qDebug() << "Device unable to be cast to correct device";
            }
            return dev;
        } else {
            return nullptr;
        }
    }

    template<class IFace>
    const IFace *interface(const Solid::Device &device)
    {
        IFace *dev = device.as<const IFace>();
        if (!dev) {
            qDebug() << "Device unable to be cast to correct device";
        }
        return dev;
    }

    template<class IFace>
    void createDeviceChildren(QTreeWidgetItem *treeParent, const QString &parentUid, const Solid::DeviceInterface::Type &type)
    {
        const QList<Solid::Device> list = Solid::Device::listFromType(type, parentUid);

        for (const Solid::Device &dev : list) {
            new IFace(treeParent, dev);
        }
    }

    void setDeviceIcon(const QIcon &);
    void setDeviceToolTip(const QString &);

    virtual QVListLayout *infoPanelLayout();
    virtual void addItem(const Solid::Device &dev)
    {
        new SolDevice(this, dev);
    }

    virtual void refreshName()
    {
        setDefaultDeviceText();
    }

    QString udi() const;
    bool isDeviceSet();

    bool operator<(const QTreeWidgetItem &other) const override;

protected:
    void setDeviceText(const QString &);

    virtual void setDefaultDeviceToolTip();
    virtual void setDefaultDeviceText();
    virtual void setDefaultDeviceIcon();
    virtual void setDefaultListing(const Solid::DeviceInterface::Type &);

    bool deviceSet;
    QVListLayout *deviceInfoLayout;
    Solid::DeviceInterface::Type deviceTypeHolder;
    Solid::Device tiedDevice;
};

#endif // SOLDEVICE
