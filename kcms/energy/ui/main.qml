/*
 *   SPDX-FileCopyrightText: 2015 Kai Uwe Broulik <kde@privat.broulik.de>
 *   SPDX-FileCopyrightText: 2025 Ismael Asensio <isma.af@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.kcmutils as KCM
import org.kde.kinfocenter.energy.private 1.0

KCM.SimpleKCM {
    id: root

    property QtObject currentBattery: null
    property string currentUdi: ""
    property string currentVendor: ""
    property string currentProduct: ""
    property bool compact: (root.width / Kirigami.Units.gridUnit) < 25

    property int historyType: HistoryModel.ChargeType

    readonly property var details: [
        {
            title: i18n("Battery"),
            data: [
                {label: i18n("Rechargeable"), value: "rechargeable"},
                {label: i18n("Charge state"), value: "chargeState", modifier: "chargeState"},
                {label: i18n("Current charge"), value: "chargePercent", unit: "%", precision: 0},
                {label: i18n("Health"), value: "capacity", unit: "%", precision: 0},
                {label: i18n("Charge Cycles"), value: "cycleCount", modifier: "cycleCount"},
                {label: i18n("Vendor"), value: "vendor", source:"Vendor"},
                {label: i18n("Model"), value: "model", source:"Product"},
                {label: i18n("Serial Number"), value: "serial"},
                {label: i18n("Technology"), value: "technology", modifier: "technology"}
            ]
        },
        {
            title: i18n("Energy"),
            data: [
                {label: i18nc("current power draw from the battery in W", "Consumption"), value: "energyRate", unit: i18nc("Watt", "W"), precision: 2},
                {label: i18n("Voltage"), value: "voltage", unit: i18nc("Volt", "V"), precision: 2},
                {label: i18n("Remaining energy"), value: "energy", unit: i18nc("Watt-hours", "Wh"), precision: 2},
                {label: i18n("Last full charge"), value: "energyFull", unit: i18nc("Watt-hours", "Wh"), precision: 2},
                {label: i18n("Original charge capacity"), value: "energyFullDesign", unit: i18nc("Watt-hours", "Wh"), precision: 2}
            ]
        },
        {
            title: i18n("Environment"),
            data: [
                {label: i18n("Temperature"), value: "temperature", unit: i18nc("Degree Celsius", "°C"), precision: 2}
            ]
        },
    ]

    function modifier_chargeState(value) {
        switch(value) {
        case Battery.NoCharge: return i18n("Not charging")
        case Battery.Charging: return i18n("Charging")
        case Battery.Discharging: return i18n("Discharging")
        case Battery.FullyCharged: return i18n("Fully charged")
        }
    }

    function modifier_technology(value) {
        switch(value) {
        case Battery.LithiumIon: return i18n("Lithium ion")
        case Battery.LithiumPolymer: return i18n("Lithium polymer")
        case Battery.LithiumIronPhosphate: return i18n("Lithium iron phosphate")
        case Battery.LeadAcid: return i18n("Lead acid")
        case Battery.NickelCadmium: return i18n("Nickel cadmium")
        case Battery.NickelMetalHydride: return i18n("Nickel metal hydride")
        default: return i18n("Unknown technology")
        }
    }

    function modifier_cycleCount(value) {
        // -1 means "not available", don't show this.
        return value > 0 ? value : "";
    }

    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: !!currentBattery ? Kirigami.Units.gridUnit * 30 : Kirigami.Units.gridUnit * 12

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: kcm.batteries.count <= 0 && history.count <= 0
        width: parent.width - (Kirigami.Units.largeSpacing * 4)
        text: i18nc("@info:status", "No energy information available on this system")
        icon.name: "utilities-energy-monitor"
    }

    header: ColumnLayout { // Required for padding and centering in the header
        QQC2.ScrollView {
            visible: kcm.batteries.count > 1

            Layout.margins: Kirigami.Units.smallSpacing
            Layout.alignment: Qt.AlignCenter

            Layout.preferredWidth: deviceList.count * Kirigami.Units.gridUnit * 12
            Layout.maximumWidth: root.header.width - Kirigami.Units.smallSpacing
            Layout.preferredHeight: deviceList.implicitHeight + effectiveScrollBarHeight

            contentItem: ListView {
                id: deviceList

                orientation: ListView.Horizontal
                spacing: Kirigami.Units.smallSpacing
                highlightMoveDuration: Kirigami.Units.longDuration

                implicitHeight: currentItem?.implicitHeight ?? Kirigami.Units.gridUnit * 4

                currentIndex: 0  // At initialization
                onCurrentItemChanged: {
                    if (!currentItem) {
                        return;
                    }
                    root.currentUdi = currentItem.udi
                    root.currentVendor = currentItem.vendor
                    root.currentProduct = currentItem.product
                    root.currentBattery = currentItem.battery
                }

                model: kcm.batteries

                delegate: QQC2.Button {
                    required property int index
                    required property string udi
                    required property string vendor
                    required property string product
                    required property QtObject battery

                    width: Math.floor(deviceList.width / deviceList.count - deviceList.spacing)

                    checked: index === ListView.view.currentIndex
                    checkable: true

                    onClicked: {
                        ListView.view.currentIndex = index
                    }

                    padding: Kirigami.Units.smallSpacing * 2
                    contentItem: ColumnLayout {
                        RowLayout {
                            Layout.fillWidth: true

                            // Reference: BatteryType enum in solid/devices/frontend/battery.h
                            Kirigami.Icon {
                                id: batteryIcon
                                Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                                Layout.preferredHeight: Layout.preferredWidth
                                source: {
                                    switch(battery.type) {
                                        case 1: return "phone"
                                        case 2: return "battery-ups"
                                        case 3: return battery.chargeState === 1 ? "battery-full-charging" : "battery-full"
                                        case 4: return "input-mouse"
                                        case 5: return "input-keyboard"
                                        case 6: return "input-keyboard" // TODO: New Icon Required?
                                        case 7: return "camera-photo"
                                        case 8: return "smartphone"
                                        case 9: return "monitor"
                                        case 10: return "input-gamepad"
                                        case 11: return "preferences-system-bluetooth-battery"
                                        case 12: return "input-tablet"
                                        case 13: return "headphone"
                                        case 14: return "headset"
                                        case 15: return "input-touchpad"
                                        default: return "paint-unknown"
                                    }
                                }
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 0

                                QQC2.Label {
                                    Layout.fillWidth: true
                                    text: {
                                        switch(battery.type) {
                                            case 1: return i18n("PDA Battery")
                                            case 2: return i18n("UPS Battery")
                                            case 3: return i18n("Internal Battery")
                                            case 4: return i18n("Mouse Battery")
                                            case 5: return i18n("Keyboard Battery")
                                            case 6: return i18n("Keyboard/Mouse Battery")
                                            case 7: return i18n("Camera Battery")
                                            case 8: return i18n("Phone Battery")
                                            case 9: return i18n("Monitor Battery")
                                            case 10: return i18n("Gaming Input Battery")
                                            case 11: return i18n("Bluetooth Battery")
                                            case 12: return i18n("Tablet Battery")
                                            case 13: return i18n("Headphone Battery")
                                            case 14: return i18n("Headset Battery")
                                            case 15: return i18n("Touchpad Battery")
                                            default: return i18n("Unknown Battery")
                                        }
                                    }
                                    elide: Text.ElideRight
                                    maximumLineCount : 1
                                }

                                QQC2.Label {
                                    Layout.fillWidth: true
                                    text: product
                                    color: Kirigami.Theme.disabledTextColor
                                    elide: Text.ElideRight
                                    maximumLineCount : 1
                                }
                            }
                        }

                        RowLayout {
                            QQC2.ProgressBar {
                                id: percentageSlider
                                Layout.fillWidth: true
                                from: 0
                                to: 100
                                value: battery.chargePercent
                            }

                            QQC2.Label {
                                text: battery.chargeState === 1 ?
                                    i18nc("Battery charge percentage", "%1% (Charging)", Math.round(percentageSlider.value)) :
                                    i18nc("Battery charge percentage", "%1%", Math.round(percentageSlider.value))
                            }
                        }
                    }
                }
            }
        }
    }

    HistoryModel {
        id: history
        duration: timespanCombo.duration
        device: currentUdi
        type: root.historyType
    }

    // Set the padding for KCMUtils.SimpleKCM to the Kirigami.Page defaults
    topPadding: Kirigami.Units.gridUnit
    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Kirigami.AbstractCard {
            visible: !!root.currentBattery && history.available

            Layout.fillWidth: true
            Layout.preferredHeight: root.width / 3
            Layout.minimumHeight: Kirigami.Units.gridUnit * 12
            Layout.maximumHeight: Kirigami.Units.gridUnit * 24

            contentItem: Graph {
                id: graph

                data: history.points
                xDuration: timespanCombo.duration

                yLabel: root.historyType == HistoryModel.RateType ? ( value => i18nc("Graph axis label: power in Watts","%1 W", value) )
                                                                  : ( value => i18nc("Graph axis label: percentage","%1%", value) )
                yMax: {
                    if (root.historyType == HistoryModel.RateType) {
                        // Ceil to next 10
                        return Math.floor(history.largestValue / 10) * 10 + 10;
                    } else {
                        return 100;
                    }
                }
                yStep: root.historyType == HistoryModel.RateType ? 10 : 20
            }

            // Reparented to keep the item outside of a layout and the graph canvas
            Kirigami.PlaceholderMessage {
                parent: graph
                visible: graph.data.length < 2
                x: graph.plotCenter.x - width / 2
                y: graph.plotCenter.y - height / 2
                width: graph.plot.width - (Kirigami.Units.largeSpacing * 4)
                text: i18nc("@info:status", "No history information for this time span")
            }

            footer: GridLayout {
                columns: !compact ? 5 : 3

                QQC2.Button {
                    id: chargeButton
                    checked: true
                    checkable: true
                    text: i18n("Charge Percentage")
                    onClicked: {
                        historyType = HistoryModel.ChargeType
                        rateButton.checked = false
                    }
                }

                QQC2.Button {
                    id: rateButton
                    checkable: true
                    text: i18n("Energy Consumption")
                    onClicked: {
                        historyType = HistoryModel.RateType
                        chargeButton.checked = false
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                QQC2.ComboBox {
                    id: timespanCombo
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 6

                    model: [
                        i18n("Last hour"),
                        i18n("Last 2 hours"),
                        i18n("Last 12 hours"),
                        i18n("Last 24 hours"),
                        i18n("Last 48 hours"),
                        i18n("Last 7 days")
                    ]

                    readonly property int duration : {
                        const hours = [1, 2, 12, 24, 48, 24 * 7];
                        return hours[currentIndex] * 3600;
                    }

                    Accessible.name: i18n("Timespan")
                    Accessible.description: i18n("Timespan of data to display")
                }

                QQC2.Button {
                    icon.name: "view-refresh-symbolic"
                    hoverEnabled: true
                    QQC2.ToolTip.text: i18n("Refresh")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    Accessible.name: QQC2.ToolTip.text
                    onClicked: history.refresh()
                }
            }
        }

        Repeater {
            id: titleRepeater
            property list<Kirigami.FormLayout> layouts

            visible: !!currentBattery

            model: root.details

            delegate: Kirigami.FormLayout {
                id: currentLayout

                Component.onCompleted: {
                    // ensure that all visible FormLayout share the same set of twinFormLayouts
                    titleRepeater.layouts.push(currentLayout);
                    for (let i = 0, length = titleRepeater.layouts.length; i < length; ++i) {
                        titleRepeater.layouts[i].twinFormLayouts = titleRepeater.layouts;
                    }
                }

                Kirigami.Heading {
                    text: modelData.title
                    Kirigami.FormData.isSection: true
                    level: 2
                    // HACK hide section header if all labels are invisible
                    visible:  [...Array(detailsRepeater.count).keys()].some(i => detailsRepeater.itemAt(i)?.visible ?? false)
                }

                Repeater {
                    id: detailsRepeater
                    model: modelData.data || []
                    delegate: Kirigami.SelectableLabel {
                        id: valueLabel
                        visible: text.length > 0
                        Layout.fillWidth: true

                        Keys.onPressed: {
                            if (event.matches(StandardKey.Copy)) {
                                valueLabel.copy();
                                event.accepted = true;
                            }
                        }

                        Kirigami.FormData.label: i18n("%1:", modelData.label)
                        text: {
                            let value = (modelData.source) ? root["current" + modelData.source]
                                                           : currentBattery[modelData.value]

                            if (typeof value === "boolean") {
                                return value ? i18n("Yes") : i18n("No")
                            }

                            if (!value) {
                                return ""
                            }

                            if (modelData.precision) { // round to decimals
                                value = Number(value).toLocaleString(Qt.locale(), "f", modelData.precision)
                            }

                            if (modelData.modifier && root["modifier_" + modelData.modifier]) {
                                value = root["modifier_" + modelData.modifier](value)
                            }

                            switch (modelData.unit) {
                            case undefined:
                                return value
                            case "%":
                                return i18nc("%1 is a percentage value", "%1%", value)
                            default:
                                return i18nc("%1 is value, %2 is unit", "%1 %2", value, modelData.unit)
                            }
                        }
                    }
                }
            }
        }
    }
}
