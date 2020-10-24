/*
    SPDX-FileCopyrightText: 2010 Nicolas Ternisien <nicolas.ternisien@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KCONTROL_MEMORY_H
#define KCONTROL_MEMORY_H

#include <QPushButton>
#include <QTimer>
#include <QWidget>

#include <KAboutData>
#include <KCModule>

#include "base.h"

class QGroupBox;
class QPushButton;
class ChartWidget;

class KCMMemory : public KCModule
{
    Q_OBJECT

public:
    explicit KCMMemory(QWidget *parent = nullptr, const QVariantList &list = QVariantList());
    ~KCMMemory();

    QString quickHelp() const override;

private Q_SLOTS:
    void updateMemoryText();
    void updateMemoryGraphics();

    void updateDatas();

private:
    QGroupBox *initializeText();
    QGroupBox *initializeCharts();

    QTimer *timer;

    ChartWidget *totalMemory;
    ChartWidget *physicalMemory;
    ChartWidget *swapMemory;

    void fetchValues();
};

#endif
