/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2024 Thomas Duckworth <tduck973564@gmail.com>
    SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>
*/

import QtQuick 2.15

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kcm.memory

KCM.SimpleKCM {
    required property KCMMemory kcm

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - (Kirigami.Units.largeSpacing * 8)
        visible: kcm.memoryInformation === ""
        icon.name: "data-warning"
        text: i18ndc("kinfocenter", "@info the KCM has no data to display", "No data available")
    }

    Kirigami.SelectableLabel {
        id: text
        text: kcm.memoryInformation
        font: Kirigami.Theme.fixedWidthFont
        visible: kcm.memoryInformation !== ""
    }
}
