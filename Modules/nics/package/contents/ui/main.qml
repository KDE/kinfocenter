/*
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.14 as Kirigami
import org.kde.kcm 1.3
import org.kde.kinfocenter.nic.private 1.0

SimpleKCM {
    clip: true

    Kirigami.CardsLayout {
        Repeater {
            id: repeater
            model: NetworkModel { id: networkModel }
            Kirigami.AbstractCard {
                Layout.fillHeight: true
                contentItem: Kirigami.FormLayout {
                    id: delegateLayout
                    // NB: do not make the repeated layouts twinFormLayouts!
                    //    It utterly doesn't scale and causes seconds long lockups while calculating the layout.
                    Layout.fillWidth: true
                    QQC2.Label {
                        Kirigami.FormData.label: i18nc("@label", "Name:")
                        text: model.name
                    }
                    QQC2.Label {
                        Kirigami.FormData.label: i18nc("@label", "Address:")
                        text: model.address
                    }
                    QQC2.Label {
                        Kirigami.FormData.label: i18nc("@label", "Network Mask:")
                        text: model.netmask
                    }
                    QQC2.Label {
                        Kirigami.FormData.label: i18nc("@label", "Type:")
                        text: model.type
                    }
                    QQC2.Label {
                        Kirigami.FormData.label: i18nc("@label", "Hardware Address:")
                        text: model.hardwareAddress
                        visible: text.length > 0
                    }
                    RowLayout {
                        Kirigami.FormData.label: i18nc("@label", "State:")
                        Rectangle {
                            width: Kirigami.Units.largeSpacing
                            height: width
                            radius: width / 2
                            color: model.state ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.negativeBackgroundColor
                        }
                        QQC2.Label {
                            text: model.state ? i18nc("State of network card is connected", "Up")
                                              : i18nc("State of network card is disconnected", "Down")
                        }
                    }
                }
            }
        }
    }


    footer: QQC2.ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing
            QQC2.Button {
                Layout.fillWidth: true
                icon.name: "view-refresh"
                text: i18nc("@action:button", "Refresh")
                onClicked: networkModel.update();
            }
        }
    }
}
