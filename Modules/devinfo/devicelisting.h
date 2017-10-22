/*
 *  devicelisting.h
 *
 *  Copyright (C) 2009 David Hubner <hubnerd@ntlworld.com>
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
 *
 */

#ifndef DEVICELISTING
#define DEVICELISTING

//Solid
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/processor.h>
#include <solid/devicenotifier.h>

//QT
#include <QMap>
#include <QTreeWidget>
#include <QAction>
#include <QContextMenuEvent>
#include <QTreeWidgetItemIterator>

//Local
#include "infopanel.h"
#include "soldevice.h"
#include "soldevicetypes.h"
#include "devinfo.h"
#include "solidhelper.h"
//#include "nicsignals.h"

//Kde

class InfoPanel;
class DevInfoPlugin;
//class NicSignals;

class DeviceListing : public QTreeWidget
{
    Q_OBJECT

private:
    enum show {
        ALL = 0, RELEVANT
    };

public:
    DeviceListing(QWidget *, InfoPanel *, DevInfoPlugin *);
    ~DeviceListing();

    static QTreeWidgetItem *getTreeWidgetItemFromUdi(QTreeWidget *widget, const QString &udi)
    {
        QTreeWidgetItemIterator treeWidget(widget);
        while (*treeWidget)
        {
            SolDevice *item = static_cast<SolDevice *>(*treeWidget);
            if (item->udi() == udi) {
                return *treeWidget;
            }
            ++treeWidget;
        }
        return NULL;
    }

private:
    void populateListing(const show = RELEVANT);
    QTreeWidgetItem *createListItems(const Solid::DeviceInterface::Type &);
    void contextMenuEvent(QContextMenuEvent *) Q_DECL_OVERRIDE;
    void createMenuActions();

    QMap<Solid::DeviceInterface::Type, SolDevice *> deviceMap;
    InfoPanel *iPanel;
    QAction *colAct, *expAct, *allAct, *relAct;
    DevInfoPlugin *status;
    //NicSignals *nicSig;

public Q_SLOTS:
    void itemActivatedSlot(QTreeWidgetItem *, const int);
    void deviceAddedSlot(const QString &);
    void deviceRemovedSlot(const QString &);
    void collapseAllDevicesSlot();
    void expandAllDevicesSlot();
    void showAllDevicesSlot();
    void showRelevantDevicesSlot();
};

#endif //DEVICELISTING
