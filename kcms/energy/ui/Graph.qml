/*
 * SPDX-FileCopyrightText: 2015 David Edmundson <david@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2025 Ismael Asensio <isma.af@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
import QtQuick
import QtQuick.Controls as Controls
import QtGraphs
import QtQuick.Shapes
import org.kde.kirigami as Kirigami

/**
 * Original comment
 * We need to draw a graph, all other libs are not suitable as we are basically
 * a connected scatter plot with non linear X spacing.
 * Currently this is not available in kdeclarative nor kqtquickcharts
 *
 * We only paint once, so canvas is fast enough for our purposes.
 * It is designed to look identical to those in ksysguard.
 *
 * QtGraphs version
 * I think we can do everything we ever wanted now ?
 */
Item {
    id: graphRoot

    property list<point> points
    property string timeZone
    property int xDuration: 3600 // in seconds
    property string yLabel
    property int yMax: 100
    readonly property int yMin: 0
    property int yStep: 20

    Behavior on xDuration {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
    }

    onPointsChanged: {
        uSerie.replace(graphRoot.points);
    }

    GraphsView {
        id: graph

        function getYValue(timeOffset): int { // good enough
            let beginIndex = 0;
            let endIndex = graphRoot.points.length - 1;
            while (endIndex - beginIndex > 0) {
                const middleIndex = Math.floor((beginIndex + endIndex) / 2);
                if (timeOffset < graphRoot.points[middleIndex].x) {
                    endIndex = middleIndex - 1;
                } else {
                    beginIndex = middleIndex + 1;
                }
            }
            return graphRoot.points[endIndex].y;
        }
        function hoverHandler(position: point, value: point): void {
            if (value.y < graphRoot.yMin || value.y > graphRoot.yMax)
                return;
            const timeOffset = Math.round(aXe.min.getTime() + value.x);
            const _date = new Date(timeOffset);
            if (plotArea.x + position.x + coordinateLabel.width > graphRoot.width) {
                coordinateLabel.x = graphRoot.width - coordinateLabel.width - Kirigami.Units.smallSpacing;
            } else {
                coordinateLabel.x = plotArea.x + position.x;
            }
            coordinateLabel.y = (position.y - coordinateLabel.height);

            coordinateLabel.text = Qt.locale().toString(_date, Locale.ShortFormat) + ": " + getYValue(timeOffset);
        }

        anchors.fill: parent
        marginBottom: Kirigami.Units.smallSpacing
        marginLeft: Kirigami.Units.smallSpacing
        marginRight: Kirigami.Units.smallSpacing
        marginTop: Kirigami.Units.smallSpacing

        axisX: DateTimeAxis {
            id: aXe

            labelFormat: graphRoot.xDuration >= 3600 * 24 ? Qt.locale().dateTimeFormat(Locale.ShortFormat) : Qt.locale().timeFormat(Locale.ShortFormat)
            max: new Date()
            min: new Date(max - (graphRoot.xDuration * 1000))
            subTickCount: 1
            timeZone: timeZoneFromString(graphRoot.timeZone)

            labelDelegate: Component {
                Column {
                    property list<string> dateTime: text.split(" ")
                    property string text

                    spacing: 0

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: palette.text
                        font.pointSize: 8
                        text: visible ? parent.dateTime[1] : ""
                        visible: parent.dateTime.length > 1
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: palette.text
                        font.pointSize: (parent.dateTime.length > 1) ? 7 : 8
                        text: parent.dateTime[0]
                    }
                }
            }

            tickInterval: 6
        }
        axisY: ValueAxis {
            id: aYe

            labelFormat: "%.0f" + graphRoot.yLabel
            max: graphRoot.yMax
            min: graphRoot.yMin
            tickInterval: graphRoot.yMax === 100 ? 25.0 : graphRoot.yMax / 5

            labelDelegate: Component {
                Item {
                    property string text

                    Text {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        color: palette.text
                        font.pointSize: 9
                        text: parent.text
                    }
                }
            }
        }

        // - [ ] TODO : react to availability change event
        theme: GraphsTheme {
            axisY.mainColor: "#d3d3d6"
            axisY.mainWidth: grid.mainWidth
            axisY.subColor: "#dfdfdf"
            backgroundColor: palette.light
            borderColors: [palette.accent]
            colorScheme: GraphsTheme.ColorScheme.Automatic
            colorStyle: GraphsTheme.ColorStyle.RangeGradient
            grid.mainColor: "#d3d3d6"
            grid.mainWidth: 1.0
            grid.subColor: "#dfdfdf"
            seriesColors: [Qt.alpha(palette.accent, 0.2)]
            theme: GraphsTheme.Theme.UserDefined
        }

        onHover: (_, pos, val) => hoverHandler(pos, val)
        onHoverEnter: coordinateLabel.visible = true
        onHoverExit: coordinateLabel.visible = false

        SystemPalette {
            id: palette

            colorGroup: SystemPalette.Active

            onPaletteChanged: {}
        }
        AreaSeries {
            id: area

            hoverable: true
            name: "areaGraph"

            gradient: LinearGradient {
                x1: 0.0
                x2: 0.0
                y1: graph.plotArea.y
                y2: graph.plotArea.height

                GradientStop {
                    color: Qt.alpha(palette.accent, 0.3)
                    position: 0.0
                }
                GradientStop {
                    color: Qt.alpha(palette.accent, 0.1)
                    position: 1.0
                }
            }
            upperSeries: LineSeries {
                id: uSerie
            }
        }
    }
    Kirigami.PlaceholderMessage {
        text: i18nc("@info:status", "No history information for this time span")
        visible: graphRoot.points.length < 2
        width: graph.plotArea.width - (Kirigami.Units.largeSpacing * 4)
        x: graph.plotArea.x + graph.plotArea.width / 2 - width / 2
        y: graph.plotArea.y + graph.plotArea.height / 2 - height / 2
    }
    Controls.Label {
        id: coordinateLabel

        color: Kirigami.Theme.textColor
        font.pointSize: Kirigami.Theme.smallFont.pointSize
        padding: Kirigami.Units.smallSpacing
        visible: false

        background: Rectangle {
            border.color: Kirigami.Theme.textColor
            border.width: 1
            color: Kirigami.Theme.backgroundColor
            opacity: 0.9
            radius: Kirigami.Units.smallSpacing
        }
    }
}
