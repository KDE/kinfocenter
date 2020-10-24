/*
 *  SPDX-FileCopyrightText: 2003 Alexander Neundorf <neundorf@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VIEW1394_H_
#define VIEW1394_H_

#include <KCModule>

#include <QMap>
#include <QSocketNotifier>

#include <QTimer>

#include "ui_view1394widget.h"

#include <libraw1394/raw1394.h>

class OuiDb
{
public:
    OuiDb();
    QString vendor(octlet_t guid);

private:
    void loadFromOuiTxt(const QString &filename);
    QMap<QString, QString> m_vendorIds;
};

class View1394Widget : public QWidget, public Ui::View1394Widget
{
public:
    View1394Widget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

class View1394 : public KCModule
{
    Q_OBJECT
public:
    View1394(QWidget *parent, const QVariantList &args);
    virtual ~View1394();

public Q_SLOTS:
    // Public slots
    void rescanBus();
    void generateBusReset();

private:
    View1394Widget *m_view;
    QList<raw1394handle_t> m_handles;
    QList<QSocketNotifier *> m_notifiers;
    bool readConfigRom(raw1394handle_t handle, nodeid_t nodeid, quadlet_t &firstQuad, quadlet_t &cap, octlet_t &guid);
    bool m_insideRescanBus;
    QTimer m_rescanTimer;
    OuiDb *m_ouiDb;
    OuiDb *ouiDb();
private Q_SLOTS:
    void callRaw1394EventLoop(int fd);
};
#endif
