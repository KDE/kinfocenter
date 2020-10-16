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

//QT
#include <QMap>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>

//Local
#include "soldevice.h"

//Kde

class InfoPanel;
class DevInfoPlugin;
class QContextMenuEvent;
class QAction;

class DeviceListing : public QTreeWidget
{
    Q_OBJECT

private:
    enum show {
        ALL = 0, RELEVANT
    };

public:
    DeviceListing(QWidget *, InfoPanel *, DevInfoPlugin *);
    ~DeviceListing() override;

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
        return nullptr;
    }

private:
    void populateListing(const show = RELEVANT);
    QTreeWidgetItem *createListItems(const Solid::DeviceInterface::Type &);
    void contextMenuEvent(QContextMenuEvent *) override;
    void createMenuActions();

    QMap<Solid::DeviceInterface::Type, SolDevice *> deviceMap;
    InfoPanel *iPanel = nullptr;
    QAction *colAct = nullptr;
    QAction *expAct = nullptr;
    QAction *allAct = nullptr;
    QAction *relAct = nullptr;
    DevInfoPlugin *status = nullptr;
    //NicSignals *nicSig;

public Q_SLOTS:
    void currentItemChangedSlot(QTreeWidgetItem *, QTreeWidgetItem *);
    void deviceAddedSlot(const QString &);
    void deviceRemovedSlot(const QString &);
    void collapseAllDevicesSlot();
    void expandAllDevicesSlot();
    void showAllDevicesSlot();
    void showRelevantDevicesSlot();
};

#endif //DEVICELISTING
