/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCMUtils

import org.kde.kinfocenter.about_distro.private as Private

KCMUtils.SimpleKCM {
    id: root

    // bugger off with all your buttons, we don't need any!
    KCMUtils.ConfigModule.buttons: KCMUtils.ConfigModule.NoAdditionalButton

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20

    // Scoot away from the edges! Otherwise the KCM looks overly cramped.
    topPadding: Kirigami.Units.gridUnit
    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit

    Private.ServiceRunner {
        id: kicRunner
        desktopFileName: "org.kde.kinfocenter"
    }

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.largeSpacing

            Kirigami.Icon {
                source: kcm.distroLogo
                implicitWidth: Kirigami.Units.iconSizes.enormous
                implicitHeight: Kirigami.Units.iconSizes.enormous
            }

            ColumnLayout {
                Item { Layout.fillHeight: true }

                Kirigami.Heading {
                    text: kcm.distroNameVersion
                    level: 1
                    type: Kirigami.Heading.Type.Primary
                }

                Kirigami.Heading {
                    visible: kcm.distroVariant !== ""
                    text: kcm.distroVariant
                    level: 2
                    type: Kirigami.Heading.Type.Secondary
                }

                QQC2.Label {
                    visible: kcm.distroUrl !== ""
                    text: "<a href='%1'>%1</a>".arg(kcm.distroUrl)
                    textFormat: Text.RichText
                    onLinkActivated: link => Qt.openUrlExternally(link)

                    HoverHandler { cursorShape: Qt.PointingHandCursor }
                }

                Item { Layout.fillHeight: true }
            }
        }

        Kirigami.FormLayout {
            Layout.fillHeight: true

            Component {
                id: entryComponent
                RowLayout {
                    Kirigami.FormData.label: entry.localizedLabel()
                    Kirigami.FormData.labelAlignment: idealAlignment
                    Layout.alignment: idealAlignment
                    readonly property int idealAlignment: valueLabel.lineCount > 1 ? Qt.AlignTop : Qt.AlignVCenter // looks tidier this way
                    readonly property bool hidden: entry.isHidden()
                    readonly property string hint: entry.localizedHint().text
                    readonly property color hintColorForeground: {
                        switch (entry.localizedHint().color) {
                            case Private.Hint.Color.One: return Kirigami.Theme.linkColor
                            case Private.Hint.Color.Two: return Kirigami.Theme.positiveTextColor
                            case Private.Hint.Color.Three: return Kirigami.Theme.alternateTextColor
                        }
                    }
                    readonly property color hintColorBackground: {
                        switch (entry.localizedHint().color) {
                            case Private.Hint.Color.One: return Kirigami.Theme.linkBackgroundColor
                            case Private.Hint.Color.Two: return Kirigami.Theme.positiveBackgroundColor
                            case Private.Hint.Color.Three: return Kirigami.Theme.alternateBackgroundColor
                        }
                    }

                    Component {
                        id: unhideDialog
                        Kirigami.PromptDialog {
                            // NB: should we ever have other entries that need this dialog then this needs refactoring on the Entry side.
                            //  Do NOT simply add if else logic here!
                            title: i18nc("@title", "Serial Number")
                            subtitle: entry.localizedValue()
                            flatFooterButtons: true
                            standardButtons: Kirigami.Dialog.NoButton
                            customFooterActions: [
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Copy")
                                    icon.name: "edit-copy-symbolic"
                                    onTriggered: source => kcm.storeInClipboard(subtitle)
                                    shortcut: StandardKey.Copy
                                }
                            ]
                            onClosed: destroy();
                        }
                    }

                    Kirigami.SelectableLabel {
                        id: valueLabel
                        visible: !hidden
                        text: entry.localizedValue()
                        Keys.onShortcutOverride: event => {
                            event.accepted = (valueLabel.activeFocus && valueLabel.selectedText && event.matches(StandardKey.Copy));
                        }
                        Keys.onPressed: event => {
                            if (event.matches(StandardKey.Copy)) {
                                valueLabel.copy();
                                event.accepted = true;
                            }
                        }
                    }

                    QQC2.Label {
                        Kirigami.Theme.colorSet: Kirigami.Theme.Window
                        visible: hint !== ""

                        // Vertical padding accounts for the difference in normal label height and the content height of this small label
                        readonly property real verticalPadding: (hintMetrics.height - contentHeight) / 2
                        // Horizontal padding also accounts for the difference in content height and the font's pixelSize to better balance the text
                        readonly property real horizontalPadding: ((hintMetrics.height - contentHeight) + (contentHeight - font.pixelSize)) / 2

                        TextMetrics {
                            // Necessary as valueLabel could be multiple lines
                            id: hintMetrics
                            text: " "
                        }

                        topPadding: verticalPadding
                        bottomPadding: verticalPadding
                        leftPadding: horizontalPadding
                        rightPadding: horizontalPadding

                        text: hint
                        color: hintColorForeground
                        font.bold: true
                        font.pixelSize: Kirigami.Theme.smallFont.pixelSize

                        background: Rectangle {
                            color: hintColorBackground
                            radius: Kirigami.Units.cornerRadius
                        }
                    }

                    QQC2.Button {
                        visible: hidden
                        property var dialog: null
                        icon.name: "view-visible-symbolic"
                        text: i18nc("@action:button show a hidden entry in an overlay", "Show")
                        onClicked: {
                            if (!dialog) {
                                dialog = unhideDialog.createObject(root, {});
                            }
                            dialog.open();
                        }
                    }
                }
            }

            Item {
                Kirigami.FormData.label: i18nc("@title:group", "Software")
                Kirigami.FormData.isSection: true
            }

            Repeater {
                model: kcm.softwareEntries
                delegate: entryComponent
            }

            Item {
                Kirigami.FormData.label: i18nc("@title:group", "Hardware")
                Kirigami.FormData.isSection: true
            }

            Repeater {
                model: kcm.hardwareEntries
                delegate: entryComponent
            }
        }
    }


    actions: [
        Kirigami.Action {
            visible: !kcm.isThisKInfoCenter && kicRunner.canRun

            icon.name: kicRunner.iconName
            text: i18nc("@action:button launches kinfocenter from systemsettings", "More System Information")
            tooltip: i18nc("@info:tooltip", "Launch %1", kicRunner.genericName)
            onTriggered: source => kicRunner.run()
        },

        Kirigami.Action {
            visible: kcm.isEnglish

            icon.name: "edit-copy-symbolic"
            text: i18nc("@action:button", "Copy Details")
            onTriggered: source => kcm.copyToClipboard()
        },

        Kirigami.Action {
            visible: !kcm.isEnglish

            icon.name: "edit-copy-symbolic"
            text: i18nc("@action:button", "Copy Details")

            Kirigami.Action {
                text: i18nc("@action:button Copy Details...", "In current language")
                onTriggered: source => kcm.copyToClipboard()
                shortcut: StandardKey.Copy
            }

            Kirigami.Action {
                text: i18nc("@action:button Copy Details...", "In English")
                onTriggered: source => kcm.copyToClipboardInEnglish()
            }
        }
    ]
}
