/*
 *    SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
 *    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "DisplayEntry.h"
#include <QGuiApplication>

#include <KScreen/Config>
#include <KScreen/GetConfigOperation>
#include <KScreen/Mode>
#include <KScreen/Output>

DisplayEntry::DisplayEntry()
    : Entry(ki18n("Display Devices:"), displayInformation())
{
}

QString DisplayEntry::displayInformation()
{
    QString displayInfo;
    KScreen::GetConfigOperation op = KScreen::GetConfigOperation();
    if (op.exec() && op.config() && !op.hasError()) {
        const auto screens = op.config()->connectedOutputs();

        if (screens.empty()) {
            return ki18n("Unable to find any displays!").toString();
        }

        for (const auto &screen : screens) {
            displayInfo.append(QStringLiteral("%1 (%2) %3×%4@%5Hz %6%")
                                   .arg(screen->model(),
                                        screen->name(),
                                        QString::number(screen->currentMode()->size().width()),
                                        QString::number(screen->currentMode()->size().height()),
                                        QString::number(qRound(screen->currentMode()->refreshRate())),
                                        QString::number(screen->scale() * 100)));
            if (screen != screens.last()) {
                displayInfo.append(QStringLiteral("\n"));
            }
        }
    }

    return displayInfo;
}
