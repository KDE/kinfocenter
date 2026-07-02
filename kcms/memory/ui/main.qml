/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Thomas Duckworth <tduck973564@gmail.com>
    SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>
*/

import QtQuick 2.15
import QtQuick.Controls as QQC

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: kcm_memory
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    QQC.BusyIndicator {
        anchors.centerIn: parent
        running: !kcm.ready && !kcm.error && !delayBusyIndicatorTimer.running

        // Avoid BusyIndicator flashing when it just takes a few milliseconds.
        Timer {
            id: delayBusyIndicatorTimer
            running: true
            repeat: false
            interval: 500
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 8)
        visible: text !== ""
        icon.name: "edit-none"
        text: {
            if (kcm.error) {
                return kcm.error;
            } else if (kcm.ready && kcm.memoryInformation === "") {
                return i18ndc("kinfocenter", "@info the KCM has no data to display", "No data available");
            } else {
                return "";
            }
        }
    }

    Kirigami.SelectableLabel {
        text: kcm.memoryInformation
        font: Kirigami.Theme.fixedWidthFont
        visible: kcm.memoryInformation !== ""
    }
}
