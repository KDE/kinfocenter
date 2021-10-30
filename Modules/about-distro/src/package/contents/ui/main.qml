/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

import QtQuick 2.15
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kcm 1.4 as KCM

KCM.SimpleKCM {
    id: root

    // bugger off with all your buttons, we don't need any!
    KCM.ConfigModule.buttons: KCM.ConfigModule.NoAdditionalButton

    implicitWidth: Kirigami.Units.gridUnit * 20
    implicitHeight: Kirigami.Units.gridUnit * 20

    // Scoot away from the edges! Otherwise the KCM looks overly cramped.
    topPadding: Kirigami.Units.largeSpacing * 4
    leftPadding: Kirigami.Units.largeSpacing * 4
    rightPadding: Kirigami.Units.largeSpacing * 4
    bottomPadding: Kirigami.Units.largeSpacing * 4

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
                QQC2.Label {
                    Kirigami.FormData.label: modelData.localizedLabel()
                    text: modelData.localizedValue()
                }
            }

            // TODO: the spacers were added for 5.23 because we can't fix Kirigami in time. See if later versions
            // are neat enough without the spacers. Also see https://invent.kde.org/frameworks/kirigami/-/merge_requests/357
            // Same for the spacer below.
            Item { implicitHeight: Kirigami.Units.gridUnit }
            Kirigami.Separator {
                Kirigami.FormData.label: i18nc("@title:group", "Software")
                Kirigami.FormData.isSection: true
            }
            Repeater {
                model: kcm.softwareEntries
                delegate: entryComponent
            }

            // TODO: same as for the spacer above
            Item { implicitHeight: Kirigami.Units.gridUnit }
            Kirigami.Separator {
                Kirigami.FormData.label: i18nc("@title:group", "Hardware")
                Kirigami.FormData.isSection: true
            }
            Repeater {
                model: kcm.hardwareEntries
                delegate: entryComponent
            }
        }
    }

    footer: QQC2.Control { // wrapping the footer to get control-style padding
        contentItem: Kirigami.ActionToolBar {
            flat: false

            actions: [
                Kirigami.Action {
                    text: i18nc("@action:button", "Copy to Clipboard")
                    icon.name: "edit-copy"
                    onTriggered: kcm.copyToClipboard()
                    shortcut: StandardKey.Copy
                },
                Kirigami.Action {
                    visible: !kcm.isEnglish
                    text: i18nc("@action:button", "Copy to Clipboard in English")
                    icon.name: "edit-copy"
                    onTriggered: kcm.copyToClipboardInEnglish()
                }
            ]
        }
    }
}