/***************************************************************************
 *   Copyright (C) 2015 Kai Uwe Broulik <kde@privat.broulik.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

import org.kde.kquickcontrolsaddons 2.0
import org.kde.kinfocenter.energy.private 1.0

//We need units from it
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.plasma.workspace.components 2.0 as WorkspaceComponents

Item {
    id: root
    property QtObject currentBattery: null
    property string currentUdi: ""
    property bool compact: (root.width / units.gridUnit) < 25

    onCurrentBatteryChanged: {
        if (!currentBattery) {
            currentBattery = kcm.batteries.get(0)
            currentUdi = kcm.batteries.udi(0)
        }
    }

    SystemPalette { id: sysPal; colorGroup: SystemPalette.Active }
    
    property bool showWakeUps: true
    property int historyType: HistoryModel.ChargeType

    readonly property var details: [
        {
            title: i18n("Battery"),
            data: [
                {label: i18n("Rechargeable"), value: "rechargeable"},
                {label: i18n("Charge state"), value: "chargeState", modifier: "chargeState"}
            ]
        },
        {
            title: i18n("Energy"),
            data: [
                {label: i18nc("current power draw from the battery in W", "Consumption"), value: "energyRate", unit: i18nc("Watt", "W"), precision: 2},
                {label: i18n("Voltage"), value: "voltage", unit: i18nc("Volt", "V"), precision: 2},
                {label: i18n("Capacity"), value: "capacity", unit: i18n("%"), precision: 0},
                {label: i18n("Current"), value: "energy", unit: i18nc("Watt-hours", "Wh"), precision: 2},
                {label: i18n("Last full"), value: "energyFull", unit: i18nc("Watt-hours", "Wh"), precision: 2},
                {label: i18n("Full design"), value: "energyFullDesign", unit: i18nc("Watt-hours", "Wh"), precision: 2}
            ]
        },
        {
            title: i18n("System"),
            data: [
                {label: i18n("Has power supply"), value: "powerSupply"}
            ]
        },
        {
            title: i18n("Environment"),
            data: [
                {label: i18n("Temperature"), value: "temperature", unit: i18nc("Degree Celsius", "°C"), precision: 2}
            ]
        },
        {
            title: i18n("Manufacturer"),
            data: [
                {label: i18n("Vendor"), value: "vendor"},
                {label: i18n("Model"), value: "model"},
                {label: i18n("Serial Number"), value: "serial"}
            ]
        }
    ]

    function modifier_chargeState(value) {
        switch(value) {
        case 0: return i18n("Not charging")
        case 1: return i18n("Charging")
        case 2: return i18n("Discharging")
        case 3: return i18n("Fully charged")
        }
    }

    Component.onCompleted: {
        currentBattery = kcm.batteries.get(0)
        currentUdi = kcm.batteries.udi(0)
    }

    width: units.gridUnit * 25
    height: !!currentBattery ? units.gridUnit * 25 : units.gridUnit * 12

    SystemPalette {
        id: syspal
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

        ColumnLayout {
            id: column

            width: scrollView.viewport.width
            spacing: units.largeSpacing

            ScrollView {
                id: tabView
                Layout.fillWidth: true
                Layout.minimumHeight: units.gridUnit * 3
                Layout.maximumHeight: Layout.minimumHeight

                frameVisible: true
                visible: kcm.batteries.count > 1

                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff

                Row {
                    Repeater {
                        model: kcm.batteries

                        Button {
                            id: button
                            width: height
                            height: tabView.viewport.height
                            checked: model.battery == root.currentBattery
                            checkable: true
                            onClicked: {
                                root.currentUdi = model.udi
                                root.currentBattery = model.battery
                                // override checked property
                                checked = Qt.binding(function() {
                                    return model.battery == root.currentBattery
                                })

                                showWakeUps = (index === 0)
                            }

                            ColumnLayout {
                                anchors {
                                    fill: parent
                                    margins: units.smallSpacing
                                }
                                spacing: 0

                                WorkspaceComponents.BatteryIcon {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    hasBattery: true
                                    batteryType: {
                                        switch(model.battery.type) {
                                        case 3: return "Battery"
                                        case 2: return "Ups"
                                        case 9: return "Monitor"
                                        case 4: return "Mouse"
                                        case 5: return "Keyboard"
                                        case 1: return "Pda"
                                        case 7: return "Phone"
                                        default: return "Unknown"
                                        }
                                    }
                                    percent: model.battery.chargePercent
                                    //pluggedIn: model.battery.chargeState === 1 // Makes it hard to see
                                }

                                ProgressBar { // TODO make progress bar not eat mouse events
                                    Layout.fillWidth: true
                                    minimumValue: 0
                                    maximumValue: 100
                                    value: model.battery.chargePercent
                                    enabled: button.checked ? false : true
                                }
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: units.smallSpacing
                visible: !!currentBattery

                GridLayout {
                    Layout.fillWidth: true
                    columns: !compact ? 5 : 3

                    Button {
                        id: chargeButton
                        checked: true
                        checkable: true
                        text: i18n("Charge Percentage")
                        onClicked: {
                            historyType = HistoryModel.ChargeType
                            rateButton.checked = false
                        }
                    }

                    Button {
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

                    ComboBox {
                        id: timespanCombo
                        Layout.minimumWidth: units.gridUnit * 6
                        visible: graph.visible
                        model: [
                            {text: i18n("Last hour"), value: 3600},
                            {text: i18n("Last 2 hours"), value: 7200},
                            {text: i18n("Last 12 hours"), value: 43200},
                            {text: i18n("Last 24 hours"), value: 86400},
                            {text: i18n("Last 48 hours"), value: 172800},
                            {text: i18n("Last 7 days"), value: 604800}
                        ]
                        Accessible.name: i18n("Timespan")
                        Accessible.description: i18n("Timespan of data to display")
                    }

                    Button {
                        iconName: "view-refresh"
                        visible: graph.visible
                        tooltip: i18n("Refresh")
                        Accessible.name: tooltip
                        onClicked: history.refresh()
                    }
                }

                HistoryModel {
                    id: history
                    duration: timespanCombo.model[timespanCombo.currentIndex].value
                    device: currentUdi
                    type: root.historyType
                }

                Graph {
                    id: graph
                    Layout.fillWidth: true
                    Layout.minimumHeight: column.width / 3
                    Layout.maximumHeight: column.width / 3
                    data: history.points

                    xMin: history.firstDataPointTime
                    xMax: history.lastDataPointTime

                    yUnits: root.historyType == HistoryModel.RateType ? i18nc("Shorthand for Watts","W") : i18nc("literal percent sign","%")
                    yMax: {
                        if (root.historyType == HistoryModel.RateType) {
                            var max = history.largestValue
                            var modulo = max % 10
                            if (modulo > 0) {
                                max = max - modulo + 10 // ceil to nearest 10s
                            }
                            return max;
                        } else {
                            return 100;
                        }
                    }
                    yStep: root.historyType == HistoryModel.RateType ? 10 : 20
                    visible: history.count > 1
                }

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: column.width / 3
                    Layout.maximumHeight: column.width / 3
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("This type of history is currently not available for this device.")
                    wrapMode: Text.WordWrap
                    visible: !graph.visible
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: units.smallSpacing
                visible: showWakeUps && kcm.wakeUps.count > 0

                RowLayout {
                    Layout.fillWidth: true
                    PlasmaExtras.Heading {
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                        color: sysPal.text
                        level: 4
                        text: i18n("Application Energy Consumption")
                    }
                }

                GridLayout {
                    id: wakeUpsGrid
                    Layout.fillWidth: true
                    rows: compact ? kcm.wakeUps.count : kcm.wakeUps.count / 2
                    flow: GridLayout.TopToBottom
                    rowSpacing: units.smallSpacing
                    columnSpacing: units.smallSpacing
                    property int barWidth: (compact ? root.width - units.gridUnit: root.width / 2) - units.smallSpacing * 2

                    Repeater {
                        model: showWakeUps ? kcm.wakeUps : null

                        PlasmaCore.ToolTipArea { // FIXME use widget style tooltip
                            Layout.minimumWidth: wakeUpsGrid.barWidth
                            Layout.maximumWidth: wakeUpsGrid.barWidth
                            height: childrenRect.height
                            z: 2 // since the progress bar eats mouse events
                            mainText: model.prettyName || model.name
                            subText: {
                                var text = ""
                                if (model.prettyName && model.name !== model.prettyName) {
                                    text += i18n("Path: %1", model.name) + "\n"
                                }
                                if (model.pid) {
                                    text += i18n("PID: %1", model.pid) + "\n"
                                }
                                // FIXME format decimals
                                text += i18n("Wakeups per second: %1 (%2%)", Math.round(model.wakeUps * 100) / 100, Math.round(model.wakeUps / kcm.wakeUps.total * 100)) + "\n"
                                if (model.details) {
                                    text += i18n("Details: %1", model.details)
                                }
                                return text
                            }
                            icon: model.iconName

                            RowLayout {
                                id: wakeUpItemRow
                                width: parent.width

                                QIconItem {
                                    width: units.iconSizes.medium
                                    height: width
                                    icon: model.iconName
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 0

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Label {
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                            text: model.prettyName || model.name
                                        }

                                        /*Label {
                                            text: i18n("System Service")
                                            visible: !model.userSpace
                                            opacity: 0.6
                                        }*/
                                    }

                                    ProgressBar {
                                        Layout.fillWidth: true
                                        minimumValue: 0
                                        maximumValue: 100
                                        value: model.wakeUps / kcm.wakeUps.total * 100
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#ccc" // FIXME palette
                //Layout.topMargin: (compact ? units.gridUnit * 2 : 0)
                visible: wakeUpsGrid.visible
            }

            ColumnLayout {
                id: detailsColumn
                property int legendWidth: 10

                Layout.fillWidth: true
                spacing: 0
                visible: !!currentBattery

                Repeater {
                    model: root.details

                    ColumnLayout {
                        spacing: 0//units.smallSpacing

                        PlasmaExtras.Heading {
                            level: 4
                            color: sysPal.text
                            text: modelData.title
                            // HACK hide section header if all labels are invisible
                            visible: {
                                for (var i = 0, length = detailsRepeater.count; i < length; ++i) {
                                    var item = detailsRepeater.itemAt(i)
                                    if (item && item.visible) {
                                        return true
                                    }
                                }

                                return false
                            }
                        }

                        Repeater {
                            id: detailsRepeater
                            model: modelData.data || []

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: units.smallSpacing * 2
                                visible: valueLabel.text !== ""

                                Label {
                                    Layout.minimumWidth: detailsColumn.legendWidth + units.gridUnit
                                    horizontalAlignment: Text.AlignRight
                                    text: i18n("%1:", modelData.label)
                                    wrapMode: Text.NoWrap
                                    opacity: 0.8
                                    onPaintedWidthChanged: {
                                       if (paintedWidth > detailsColumn.legendWidth) {
                                           detailsColumn.legendWidth = paintedWidth
                                       }
                                    }
                                }

                                Label {
                                    id: valueLabel
                                    Layout.fillWidth: true
                                    text: {
                                        var value = currentBattery[modelData.value]

                                        if (typeof value === "boolean") {
                                            if (value) {
                                                return i18n("Yes")
                                            } else {
                                                return i18n("No")
                                            }
                                        }

                                        if (!value) {
                                            return ""
                                        }

                                        var precision = modelData.precision
                                        if (typeof precision === "number") { // round to decimals
                                            value = Number(value).toLocaleString(Qt.locale(), "f", precision)
                                        }

                                        if (modelData.modifier && root["modifier_" + modelData.modifier]) {
                                            value = root["modifier_" + modelData.modifier](value)
                                        }

                                        if (modelData.unit) {
                                            value = i18nc("%1 is value, %2 is unit", "%1 %2", value, modelData.unit)
                                        }

                                        return value
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
