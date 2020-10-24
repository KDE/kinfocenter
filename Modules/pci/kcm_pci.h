/*
 *  SPDX-FileCopyrightText: 2008 Nicolas Ternisien <nicolas.ternisien@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCONTROL_KCM_PCI_H
#define KCONTROL_KCM_PCI_H

#include <KCModule>

#include "os_current.h"

class QTreeWidget;

class KCMPci : public KCModule
{
    Q_OBJECT

public:
    explicit KCMPci(QWidget *parent = nullptr, const QVariantList &list = QVariantList());
    ~KCMPci() override;

    void load() override;
    QString quickHelp() const override;

private:
    QTreeWidget *tree;
};

#endif
