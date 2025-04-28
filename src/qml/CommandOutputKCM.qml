/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kquickcontrolsaddons 2.0
import org.kde.kcmutils as KCM
import org.kde.coreaddons as KCoreAddons

import org.kde.kinfocenter.private as Private

KCM.SimpleKCM {
    id: root

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    // The CommandOutputContext object.
    required property QtObject output
    property int wrapMode: TextEdit.NoWrap
    property int textFormat: output.textFormat

    flickable.contentWidth: wrapMode === TextEdit.NoWrap ? contentLoader.implicitWidth : undefined

    Clipboard { id: clipboard }

    Component {
        id: dataComponent

        Item {
            width: parent.width
            implicitHeight: scrollView.implicitHeight

            QQC2.ScrollView {
                id: scrollView

                anchors.fill: parent

                // Use a horizontal scrollbar if text wrapping is disabled. In all other cases we'll go with the defaults.
                QQC2.ScrollBar.horizontal.policy: wrapMode === TextEdit.NoWrap ? QQC2.ScrollBar.AsNeeded : QQC2.ScrollBar.AlwaysOff

                Kirigami.SelectableLabel {
                    id: text
                    padding: Kirigami.Units.largeSpacing
                    text: output.text
                    font.family: "monospace"
                    wrapMode: root.wrapMode
                    textFormat: root.textFormat
                    onLinkActivated: link => Qt.openUrlExternally(link)
                }
            }
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
                // only show the indicator after a brief timeout otherwise we can have a situation where loading takes a couple
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
                icon.name: errorNotFilter ? "action-unavailable-symbolic" : "edit-none"

                helpfulAction: Kirigami.Action {
                    enabled: placeholder.errorNotFilter
                    icon.name: "tools-report-bug"
                    text: i18n("Report to %1", KCoreAddons.KOSRelease.name)
                    onTriggered: {
                        Qt.openUrlExternally(output.bugReportUrl)
                    }
                }
            }
        }
    }

    contentItem: ColumnLayout {
        anchors.fill: parent
        spacing: 0

        QQC2.ToolBar {
            id: filterHeader

            property bool expanded: false

            Layout.fillWidth: true
            Layout.topMargin: expanded ? 0 : -implicitHeight

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Window

            visible: {
                const isVisibleState = (root.state === "" || !(root.state === "noData" && contentLoader.item.errorNotFilter))
                return isVisibleState && root.textFormat === TextEdit.PlainText
            }
            position: QQC2.ToolBar.Header

            function toggleExpanded() {
                expanded = !expanded;
                if (expanded) {
                    filterField.forceActiveFocus();
                } else {
                    filterField.clear();
                    output.filter = "";
                }
            }

            Keys.onEscapePressed: {
                if (expanded) {
                    toggleExpanded();
                }
            }

            // Animate the header to slide in and out from the top
            // Hacky use of topMargin
            Behavior on Layout.topMargin {
                NumberAnimation { 
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
            
            contentItem: RowLayout {
                id: filterLayout
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                Layout.preferredHeight: filterHeader.visible ? filterHeader.implicitHeight : 0
                
                Kirigami.SearchField {
                    id: filterField
                
                    Layout.fillWidth: true
                    
                    placeholderText: i18ndc("kinfocenter", "@label placeholder text to filter for something", "Filter…")
                    
                    Accessible.name: i18ndc("kinfocenter", "accessible name for filter input", "Filter")
                    Accessible.searchEdit: true
                    
                    onAccepted: {
                        root.output.filter = filterField.text
                    }
                }
                
                QQC2.ToolButton {
                    icon.name: "dialog-close"
                    onClicked: filterHeader.toggleExpanded()
                    display: QQC2.AbstractButton.IconOnly
                    text: i18ndc("kinfocenter", "@action:button close filter header", "Close")
                    Accessible.name: text

                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    QQC2.ToolTip.text: text
                }
            }
        }

        // This is a bit flimsy but we want to switch the content of the KCM around, based on the data state.
        // We could switch around visiblity but a Loader seems neater over all.
        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    actions: [
        Kirigami.Action {
            enabled: output.error === ''
            visible: output.autoRefresh && output.autoRefreshMs > 0

            icon.name: "media-playback-pause-symbolic"
            text: i18ndc("kinfocenter", "@action:button pauses automatic refreshing", "Pause Refreshing")
            onTriggered: output.autoRefresh = false
        },
        Kirigami.Action {
            enabled: output.error === ''
            visible: !output.autoRefresh && output.autoRefreshMs > 0

            icon.name: "media-playback-start-symbolic"
            text: i18ndc("kinfocenter", "@action:button resumes automatic refreshing", "Resume Refreshing")
            onTriggered: output.autoRefresh = true
        },
        Kirigami.Action {
            enabled: output.error === ''

            icon.name: "edit-copy-symbolic"
            text: i18ndc("kinfocenter", "@action:button copies all displayed text", "Copy to Clipboard")
            onTriggered: clipboard.content = output.text
        },
        Kirigami.Action {
            enabled: output.error === ''
            visible: root.textFormat === TextEdit.PlainText

            icon.name: "search"
            text: i18ndc("kinfocenter", "@action:button opens filter bar", "Filter")
            onTriggered: filterHeader.toggleExpanded();
            checkable: true
            checked: filterHeader.expanded
            shortcut: "Ctrl+I"
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
