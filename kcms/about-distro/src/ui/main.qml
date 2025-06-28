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
                    visible: kcm.distroVariant.length > 0
                    text: kcm.distroVariant
                    level: 2
                    type: Kirigami.Heading.Type.Secondary
                }

                QQC2.Label {
                    visible: kcm.distroUrl.length > 0
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

                    spacing: Kirigami.Units.smallSpacing

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

                    Kirigami.Badge {
                        visible: text.length > 0
                        padding: 1
                        backgroundColor: {
                            switch (entry.localizedHint().color) {
                                case Private.hint.Color.One: return Kirigami.Theme.activeBackgroundColor
                                case Private.hint.Color.Two: return Kirigami.Theme.positiveBackgroundColor
                                case Private.hint.Color.Three: return Kirigami.Theme.alternateBackgroundColor
                            }
                        }
                        text: entry.localizedHint().text
                    }

                    Kirigami.ContextualHelpButton {
                        visible: toolTipText.length > 0
                        toolTipText: entry.localizedHelp()
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
            text: i18nc("@action:button", "Launch %1…", kicRunner.genericName)
            tooltip: i18nc("@info:tooltip launches kinfocenter from systemsettings", "See more detailed system information")
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
