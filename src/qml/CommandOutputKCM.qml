/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.5
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kquickcontrolsaddons 2.0
import org.kde.kcm 1.4 as KCM

KCM.SimpleKCM {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20
    // Use a horizontal scrollbar if text wrapping is disabled. In all other cases we'll go with the defaults.
    horizontalScrollBarPolicy: wrapMode === TextEdit.NoWrap ? Qt.ScrollBarAsNeeded : undefined

    // The CommandOutputContext object.
    required property QtObject output
    property int wrapMode: TextEdit.NoWrap

    Component {
        id: dataComponent

        QQC2.TextArea {
            readOnly: true
            text: output.text
            font.family: "monospace"
            wrapMode: root.wrapMode
        }
    }

    Component {
        id: loadingComponent
        QQC2.BusyIndicator {
            running: true
        }
    }

    Component {
        id: noDataComponent

        Kirigami.PlaceholderMessage {
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            text: output.error
            icon.name: "data-warning"
        }
    }

    // This is a bit flimsy but we want to switch the content of the KCM around, based on the data state.
    // We could switch around visiblity but a Loader seems neater over all.
    Loader {
        id: contentLoader
    }

    footer: ColumnLayout {
        Kirigami.SearchField {
            id: filterField
            visible: root.state === ""

            placeholderText: i18nc("@label placeholder text to filter for something", "Filter…")

            Accessible.name: i18nc("accessible name for filter input", "Filter")
            Accessible.searchEdit: true

            focusSequence: "Ctrl+I"
            Layout.fillWidth: true

            onAccepted: output.filter = text
        }
    }

    states: [
        State {
            name: "loading"
            when: !output.ready
            PropertyChanges { target: contentLoader; sourceComponent: loadingComponent }
        },
        State {
            name: "noData"
            when: output.text === "" || output.error !== ""
            PropertyChanges { target: contentLoader; sourceComponent: noDataComponent }
        },
        State {
            name: "" // default state
            PropertyChanges { target: contentLoader; sourceComponent: dataComponent }
        }
    ]
}
