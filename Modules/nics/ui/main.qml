/*
 * SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.14 as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kcmutils as KCM
import org.kde.kinfocenter.nic.private 1.0

KCM.SimpleKCM {
    clip: true

    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        spacing: 0
        Repeater {
            id: repeater
            model: NetworkModel { id: networkModel }

            ColumnLayout {
                spacing: 0
                FormCard.FormHeader {
                    id: header
                    title: model.name
                    trailing: QQC2.Label {
                        text: model.address
                        topPadding: header.topPadding
                        bottomPadding: header.bottomPadding
                        leftPadding: header.leftPadding
                        rightPadding: header.rightPadding
                    }
                }

                FormCard.FormCard {
                    FormCard.FormTextDelegate {
                        text: i18nc("@label", "Network Mask:")
                        description: model.netmask
                    }

                    FormCard.FormDelegateSeparator {}

                    FormCard.FormTextDelegate {
                        text: i18nc("@label", "Type:")
                        description: model.type
                    }

                    FormCard.FormDelegateSeparator {}

                    FormCard.FormTextDelegate {
                        text: i18nc("@label", "Hardware Address:")
                        description: model.hardwareAddress
                        visible: description.length > 0
                    }

                    FormCard.FormDelegateSeparator {
                        visible: model.hardwareAddress.length > 0
                    }

                    FormCard.FormTextDelegate {
                        text: i18nc("@label", "State:")
                        description: model.state ? i18nc("State of network card is connected", "Up")
                                          : i18nc("State of network card is disconnected", "Down")
                        trailing: Rectangle {
                            implicitWidth: Kirigami.Units.gridUnit
                            implicitHeight: width
                            radius: width
                            color: model.state ? Kirigami.Theme.positiveTextColor : Kirigami.Theme.negativeBackgroundColor
                        }
                    }
                }
            }
        }
    }

    actions: [
        Kirigami.Action {
            icon.name: "view-refresh"
            text: i18nc("@action:button", "Refresh")
            onTriggered: networkModel.update()
        }
    ]
}
