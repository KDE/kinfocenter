/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kquickcontrolsaddons 2.0
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20
    // Use a horizontal scrollbar if text wrapping is disabled. In all other cases we'll go with the defaults.
    horizontalScrollBarPolicy: wrapMode === TextEdit.NoWrap ? QQC2.ScrollBar.AsNeeded : QQC2.ScrollBar.AlwaysOff

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    // The CommandOutputContext object.
    required property QtObject output
    property int wrapMode: TextEdit.NoWrap
    property int textFormat: TextEdit.PlainText

    Clipboard { id: clipboard }

    Component {
        id: dataComponent

        Kirigami.SelectableLabel {
            id: text
            text: output.text
            font.family: "monospace"
            wrapMode: root.wrapMode
            textFormat: root.textFormat
            onLinkActivated: link => Qt.openUrlExternally(link)
        }
    }

    Component {
        id: loadingComponent

        Item {
            width: parent.width
            implicitHeight: indicator.implicitHeight

            QQC2.BusyIndicator {
                id: indicator

                anchors.centerIn: parent

                running: false
                // only show the indicator after a brief timeout otherwise we can have a situtation where loading takes a couple
                // milliseconds during which time the indicator flashes up for no good reason
                Timer {
                    running: true
                    repeat: false
                    interval: 500
                    onTriggered: indicator.running = true
                }
            }
        }
    }

    Component {
        id: noDataComponent

        Item {
            width: parent.width
            implicitHeight: placeholder.implicitHeight

            Kirigami.PlaceholderMessage {
                id: placeholder
                readonly property bool errorNotFilter: output.filter === "" && output.error !== ""

                width: parent.width - (Kirigami.Units.largeSpacing * 8)
                anchors.centerIn: parent

                text: {
                    if (output.filter !== "") {
                        return i18ndc("kinfocenter", "@info", "No text matching the filter")
                    }
                    if (output.error !== "") {
                        return output.error
                    }
                    return i18ndc("kinfocenter", "@info the KCM has no data to display", "No data available")
                }
                explanation: {
                    if (errorNotFilter && output.explanation !== "") {
                        return output.explanation
                    }
                    return ""
                }
                icon.name: "data-warning"

                helpfulAction: Kirigami.Action {
                    enabled: placeholder.errorNotFilter
                    icon.name: "tools-report-bug"
                    text: i18n("Report this issue")
                    onTriggered: {
                        Qt.openUrlExternally(output.bugReportUrl)
                    }
                }
            }
        }
    }

    // This is a bit flimsy but we want to switch the content of the KCM around, based on the data state.
    // We could switch around visiblity but a Loader seems neater over all.
    Loader {
        id: contentLoader
        anchors.fill: parent
    }

    actions: [
        Kirigami.Action {
            icon.name: "edit-copy"
            text: i18ndc("kinfocenter", "@action:button copies all displayed text", "Copy to Clipboard")
            onTriggered: clipboard.content = output.text
        },
        Kirigami.Action {
            displayComponent: Kirigami.SearchField {
                visible: {
                    const isVisibleState = (root.state === "" || !(root.state === "noData" && contentLoader.item.errorNotFilter))
                    return isVisibleState && root.textFormat === TextEdit.PlainText
                }

                placeholderText: i18ndc("kinfocenter", "@label placeholder text to filter for something", "Filter…")

                Accessible.name: i18ndc("kinfocenter", "accessible name for filter input", "Filter")
                Accessible.searchEdit: true

                focusSequence: "Ctrl+I"

                onAccepted: output.filter = text
            }
        }
    ]

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
