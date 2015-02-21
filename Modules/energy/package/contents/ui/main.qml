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
//We need units from it
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.plasma.workspace.components 2.0 as WorkspaceComponents

Rectangle {
    id: root
    property QtObject currentBattery: null
    property string currentUdi: ""
    onCurrentBatteryChanged: {
        if (!currentBattery) {
            currentBattery = kcm.batteries.get(0)
            currentUdi = kcm.batteries.udi(0)
        }
        updateHistory()
    }

    property int historyType: 0
    onHistoryTypeChanged: updateHistory()

    property bool showWakeUps: true

    readonly property var details: [
        {
            title: i18n("Information"),
            data: [
                {label: i18n("Rechargeable"), value: "rechargeable"},
                {label: i18n("Charge state"), value: "chargeState", modifier: "chargeState"}
            ]
        },
        {
            title: i18n("Energy"),
            data: [
                {label: i18n("Capacity"), value: "capacity", unit: i18n("%"), precision: 0},
                {label: i18n("Voltage"), value: "voltage", unit: i18nc("Volt", "V"), precision: 2},
                {label: i18n("Current"), value: "energy", unit: i18nc("Watt-hours", "Wh"), precision: 2},
                {label: i18n("Last full"), value: "energyFull", unit: i18nc("Watt-hours", "Wh"), precision: 2},
                {label: i18n("Full design"), value: "energyFullDesign", unit: i18nc("Watt-hours", "Wh"), precision: 2}
            ]
        },
        {
            title: i18n("System"),
            data: [
                {label: i18n("Is power supply"), value: "powerSupply"}
            ]
        },
        {
            title: i18n("Environment"),
            data: [
                {label: i18n("Temperature"), value: "temperature", unit: i18nc("Degree celsius", "°C"), precision: 2}
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

    function updateHistory() {
        kcm.getHistory(currentUdi, historyType, timespanCombo.model[timespanCombo.currentIndex].value, 50)
    }

    Component.onCompleted: {
        currentBattery = kcm.batteries.get(0)
        currentUdi = kcm.batteries.udi(0)
    }

    width: units.gridUnit * 25
    height: !!currentBattery ? units.gridUnit * 25 : units.gridUnit * 12
    color: syspal.window

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

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        id: chargeButton
                        checked: true
                        checkable: true
                        text: i18n("Charge Percentage")
                        enabled: kcm.historyAvailable
                        onClicked: {
                            historyType = 0
                            rateButton.checked = false
                        }
                    }
                    Button {
                        id: rateButton
                        checkable: true
                        text: i18n("Energy Comsumption")
                        enabled: kcm.historyAvailable
                        onClicked: {
                            historyType = 1
                            chargeButton.checked = false
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    ComboBox {
                        id: timespanCombo
                        Layout.minimumWidth: units.gridUnit * 6
                        onCurrentIndexChanged: updateHistory()
                        enabled: kcm.historyAvailable
                        model: [
                            {text: i18n("Last 1 hour"), value: 3600},
                            {text: i18n("Last 24 hours"), value: 86400},
                            {text: i18n("Last 7 days"), value: 604800}
                        ]
                        Accessible.name: i18n("Timespan")
                        Accessible.description: i18n("Timespan of data to display")
                    }

                    Button {
                        iconName: "view-refresh"
                        tooltip: i18n("Refresh")
                        Accessible.name: tooltip
                        onClicked: updateHistory()
                    }
                }

                Plotter {
                    id: plotter
                    Layout.fillWidth: true
                    Layout.minimumHeight: column.width / 3
                    Layout.maximumHeight: column.width / 3
                    visible: kcm.historyAvailable

                    dataSets: [
                        PlotData {
                            color: theme.highlightColor // syspal?
                        }
                    ]
                }

                Repeater {
                    model: kcm.history

                    QtObject {
                        Component.onCompleted: plotter.addSample(model.value)
                    }
                }

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: column.width / 3
                    Layout.maximumHeight: column.width / 3

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("There is currently no history available for this device.")
                    visible: !kcm.historyAvailable
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
                        level: 4
                        text: i18n("Current Application Energy Consumption")
                    }
                }

                GridLayout {
                    id: wakeUpsGrid
                    Layout.fillWidth: true
                    rows: kcm.wakeUps.count / 2
                    flow: GridLayout.TopToBottom
                    rowSpacing: units.smallSpacing
                    columnSpacing: units.smallSpacing

                    Repeater {
                        model: showWakeUps ? kcm.wakeUps : null

                        PlasmaCore.ToolTipArea { // FIXME use widget style tooltip
                            Layout.minimumWidth: root.width / 2 - units.smallSpacing * 2
                            Layout.maximumWidth: root.width / 2 - units.smallSpacing * 2
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
                                text += i18n("Wakeups per second: %1 (%2%)", Math.round(model.wakeUps * 100) / 100, Math.round(model.wakeUps / kcm.wakeUps.total * 100))
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
                height: 1 // FIXME dpi
                color: "#ccc" // FIXME palette
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
                            text: modelData.title
                        }

                        Repeater {
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
                                            var tenPow = Math.pow(10, precision)
                                            // TODO format commas
                                            value = parseFloat(Math.round(value * tenPow) / tenPow).toFixed(precision)
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
