/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.15
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kcmutils as KCM

import org.kde.kinfocenter.about_distro.private 1.0

KCM.SimpleKCM {
    id: root

    // bugger off with all your buttons, we don't need any!
    KCM.ConfigModule.buttons: KCM.ConfigModule.NoAdditionalButton

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20

    // Scoot away from the edges! Otherwise the KCM looks overly cramped.
    topPadding: Kirigami.Units.gridUnit
    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit


    ServiceRunner {
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
                    onLinkActivated: Qt.openUrlExternally(link)

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
                    readonly property bool hidden: entry.isHidden()

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
                                    icon.name: "edit-copy"
                                    onTriggered: kcm.storeInClipboard(subtitle)
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
                        Keys.onShortcutOverride: event.accepted = (valueLabel.activeFocus && valueLabel.selectedText && event.matches(StandardKey.Copy))
                        Keys.onPressed: {
                            if (event.matches(StandardKey.Copy)) {
                                valueLabel.copy();
                                event.accepted = true;
                            }
                        }
                    }

                    QQC2.Button {
                        visible: hidden
                        property var dialog: null
                        icon.name: "view-visible"
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

            Kirigami.Separator {
                Kirigami.FormData.label: i18nc("@title:group", "Software")
                Kirigami.FormData.isSection: true
            }

            Repeater {
                model: kcm.softwareEntries
                delegate: entryComponent
            }

            Kirigami.Separator {
                Kirigami.FormData.label: i18nc("@title:group", "Hardware")
                Kirigami.FormData.isSection: true
            }

            Repeater {
                model: kcm.hardwareEntries
                delegate: entryComponent
            }
        }

        Item {
            visible: !kcm.isThisKInfoCenter && kicRunner.canRun
            implicitHeight: Kirigami.Units.largeSpacing
        }

        QQC2.Button {
            visible: !kcm.isThisKInfoCenter && kicRunner.canRun

            Layout.alignment: Qt.AlignHCenter

            text: i18nc("@action:button launches kinfocenter from systemsettings", "Launch %1", kicRunner.genericName)
            icon.name: kicRunner.iconName
            onClicked: kicRunner.run()
        }
    }


    actions: [

        Kirigami.Action {
            visible: kcm.isEnglish

            icon.name: "edit-copy"
            text: i18nc("@action:button", "Copy Details")
            onTriggered: kcm.copyToClipboard()
        },

        Kirigami.Action {
            visible: !kcm.isEnglish

            icon.name: "edit-copy"
            text: i18nc("@action:button", "Copy Details")

            Kirigami.Action {
                text: i18nc("@action:button Copy Details...", "In current language")
                onTriggered: kcm.copyToClipboard()
                shortcut: StandardKey.Copy
            }

            Kirigami.Action {
                text: i18nc("@action:button Copy Details...", "In English")
                onTriggered: kcm.copyToClipboardInEnglish()
            }
        }
    ]
}
