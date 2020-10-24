/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef DEVICELISTING
#define DEVICELISTING

// QT
#include <QMap>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>

// Local
#include "soldevice.h"

// Kde

class InfoPanel;
class DevInfoPlugin;
class QContextMenuEvent;
class QAction;

class DeviceListing : public QTreeWidget
{
    Q_OBJECT

private:
    enum show {
        ALL = 0,
        RELEVANT,
    };

public:
    DeviceListing(QWidget *, InfoPanel *, DevInfoPlugin *);
    ~DeviceListing() override;

    static QTreeWidgetItem *getTreeWidgetItemFromUdi(QTreeWidget *widget, const QString &udi)
    {
        QTreeWidgetItemIterator treeWidget(widget);
        while (*treeWidget) {
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
    // NicSignals *nicSig;

public Q_SLOTS:
    void currentItemChangedSlot(QTreeWidgetItem *, QTreeWidgetItem *);
    void deviceAddedSlot(const QString &);
    void deviceRemovedSlot(const QString &);
    void collapseAllDevicesSlot();
    void expandAllDevicesSlot();
    void showAllDevicesSlot();
    void showRelevantDevicesSlot();
};

#endif // DEVICELISTING
