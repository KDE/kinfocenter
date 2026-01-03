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

        // splits the list<point> points into multiple series based on the interval between samples
        function splicePoints() {
        // si moins de 3 points : do nothing
        // cste interval = d * split ->
        //  (nb arbitraire) * 500 / (xDuration * 1000)
        //  for i in points[1:] :
        //      si points[i] - points[i-1] > interval :
        //          create new Serie and stop old Serieif it exists. Append all of it to seriesList
        }

        anchors.fill: parent
        marginBottom: Kirigami.Units.smallSpacing
        marginLeft: Kirigami.Units.smallSpacing
        marginRight: Kirigami.Units.smallSpacing
        marginTop: Kirigami.Units.smallSpacing

        // Date QML object extends Date JS Object
        axisX: DateTimeAxis {
            id: aXe

            labelFormat: graphRoot.xDuration >= 3600 * 24 ? Qt.locale().dateTimeFormat(Locale.ShortFormat) : Qt.locale().timeFormat(Locale.ShortFormat)
            // timeZone: QTimeZone.systemTimeZoneId()
            max: new Date() // here we have to pass a date object
            min: new Date(max.valueOf() - (graphRoot.xDuration * 1000)) // definitely wrong, and yet
            subTickCount: 1

            labelDelegate: Component {
                Column {
                    property list<string> dateTime: text.split(" ")
                    property string text // I'm really not happy about this

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

            // tickInterval: 6
            // TODO: remove this
            // Component.onCompleted: {
            //     print("min UTC, " + min.toUTCString());
            //     print("min locale, " + min.toLocaleString());
            //     print("min offset, " + min.getTimezoneOffset());
            //     print("max UTC, " + max.toUTCString());
            //     print("max locale, " + max.toLocaleString());
            //     print("max offset, " + max.getTimezoneOffset());
            // }
        }
        axisY: ValueAxis {
            id: aYe

            labelFormat: "%.0f" + graphRoot.yLabel
            max: graphRoot.yMax
            min: graphRoot.yMin
            tickInterval: graphRoot.yMax === 100 ? 25.0 : graphRoot.yMax / 5

            labelDelegate: Component {
                Item {
                    property string text // I'm not happy about this

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

/*
Canvas
{
    id: canvas

    antialiasing: true

    property list<point> points

    property int yMin: 0
    property int yMax: 100
    property int yStep: 20

    property var yLabel: ( value => value )  // A formatter function

    property int xDuration: 3600

    readonly property rect plot: Qt.rect(yLabelMetrics.labelWidth,
                                         yLabelMetrics.height / 2,
                                         width - yLabelMetrics.labelWidth - 5,
                                         height - yLabelMetrics.height * 3.5)
    readonly property point plotCenter: Qt.point(
        Math.round((plot.left + plot.right) / 2),
        Math.round((plot.top + plot.bottom) / 2))

    onPointsChanged: {
        canvas.requestPaint();
    }

    onXDurationChanged: {
        canvas.requestPaint();
    }

    Behavior on xDuration {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutQuad
        }
    }

    function scalePoint(point : point, currentUnixTime : int) : point {
        const scaledX = Math.round((point.x - (currentUnixTime - xDuration)) / xDuration * plot.width);
        const scaledY = Math.round((point.y - yMin) * plot.height / (yMax - yMin));

        return Qt.point(
            plot.left + scaledX,
            plot.bottom - scaledY
        );
    }

    // Spacing between x division lines, in seconds
    function stepForDuration(seconds : int) : int {
        const hours = seconds / 3600;
        if (hours <= 1) {
            return 60 * 10;      // Ten minutes
        } else if (hours <= 12) {
            return 60 * 30;      // Half an hour
        } else if (hours <= 24) {
            return 60 * 60;      // Full hour
        } else if (hours <= 48) {
            return 60 * 60 * 2;  // Two hours
        } else {
            return 60 * 60 * 12; // Full day
        }
    }

    // Offset to align the tick marks from current time
    function offsetForStep(step : int) : int {
        const now = new Date();
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const seconds = now.getSeconds();

        switch (step / 60) {  // step in minutes
            case 60 * 12:
                return (hours - 12) * 3600 + minutes * 60 + seconds;
            case 60 * 2:
            case 60:
                return minutes * 60 + seconds;
            case 30:
                return (minutes - 30) * 60 + seconds;
            case 10:
                return (minutes % 10) * 60 + seconds;
            default:
                return 0;
        }
    }

    SystemPalette {
        id: palette;
        colorGroup: SystemPalette.Active

        onPaletteChanged: {
            canvas.requestPaint();
        }
    }

    TextMetrics {
        id: yLabelMetrics
        text: canvas.yLabel(canvas.yMax)

        readonly property int labelWidth: width + 10  // Adds the right spacing
    }

    onAvailableChanged: {
        if (available) {
            const c = canvas.getContext('2d');
            yLabelMetrics.font = c.font;
        }
    }

    onPaint: {
        const c = canvas.getContext('2d');

        c.clearRect(0, 0, width, height)

        c.lineWidth = 2;
        c.lineJoin = 'round';
        c.lineCap = 'round';
        c.fillStyle = palette.text

        const lineColor = palette.accent
        c.strokeStyle = lineColor;
        const gradient = c.createLinearGradient(0, 0, 0, height);
        gradient.addColorStop(0, Qt.alpha(lineColor, 0.2));
        gradient.addColorStop(1, Qt.alpha(lineColor, 0.05));
        c.fillStyle = gradient;

        // For scaling
        const currentUnixTime = Date.now() / 1000  // ms to s

        c.beginPath();

        // Draw the data points in the graph
        let firstPoint = null;
        let point = null;
        for (const dataPoint of canvas.points) {
            if (!dataPoint || dataPoint.x < currentUnixTime - xDuration) {
                continue;
            }

            if (!firstPoint) {
                firstPoint = scalePoint(dataPoint, currentUnixTime);
                c.moveTo(firstPoint.x, firstPoint.y);
                continue;
            }

            point = scalePoint(dataPoint, currentUnixTime);
            c.lineTo(point.x, point.y);
        }

        // Finish the graph area
        if (point && firstPoint) {
            c.stroke();
            c.strokeStyle = 'transparent';
            c.lineTo(point.x, plot.bottom);
            c.lineTo(firstPoint.x, plot.bottom);
            c.fill();
        }

        c.closePath()

        // Draw the frame on top

        //draw an outline
        c.strokeStyle = Qt.rgba(0, 50, 0, 0.02);
        c.lineWidth = 1;
        c.rect(plot.left, plot.top, plot.width, plot.height);

        // Draw the Y value texts
        c.fillStyle = palette.text;
        c.textAlign = "right"
        c.textBaseline = "middle";

        for(let i = 0; i <= yMax; i += yStep) {
            const y = scalePoint(Qt.point(0, i)).y;

            c.fillText(canvas.yLabel(i), plot.left - 10, y);

            //grid line
            c.moveTo(plot.left, y)
            c.lineTo(plot.width + plot.left, y)
        }
        c.stroke()

        // Draw the X value texts
        c.textAlign = "center"
        c.lineWidth = 1
        c.strokeStyle = Qt.alpha(palette.text, 0.15)

        const xStep = stepForDuration(xDuration)
        const xDivisions = xDuration / xStep
        const xGridDistance = plot.width / xDivisions
        const bottomPadding = canvas.height - plot.bottom

        const currentDayTime = new Date()

        const xOffset = offsetForStep(xStep)
        const xGridOffset = plot.width * (xOffset / xDuration)

        const dashedLines = 50
        const dashedLineLength = plot.height / dashedLines

        let lastDateStr = currentDayTime.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
        let dateChanged = false

        for (let i = xDivisions; i >= -1; i--) {
            const xTickPos = i * xGridDistance + plot.left - xGridOffset

            if ((xTickPos > plot.left) && (xTickPos < plot.width + plot.left)) {
                const xTickDateTime = new Date((currentUnixTime - (xDivisions - i) * xStep - xOffset) * 1000)
                const xTickDateStr = xTickDateTime.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
                const xTickTimeStr = xTickDateTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)

                if (lastDateStr != xTickDateStr) {
                    dateChanged = true
                }

                let dashedLineDutyCycle = 0.1
                if  ((i % 2 == 0) || (xDivisions < 10)) {
                    // Display the time
                    c.fillText(xTickTimeStr, xTickPos, plot.bottom + bottomPadding / 2)

                    // If the date has changed and is not the current day in a <= 24h graph, display it
                    // Always display the date for 48h and 1 week graphs
                    if (dateChanged || xDuration > 60*60*48) {
                        c.fillText(xTickDateStr, xTickPos, plot.bottom + bottomPadding * 4/5)
                        dateChanged = false
                    }

                    // Tick markers
                    c.moveTo(xTickPos, plot.bottom)
                    c.lineTo(xTickPos, plot.bottom + bottomPadding * 1/5)

                    dashedLineDutyCycle = 0.5
                }

                for (let j = 0; j < dashedLines; j++) {
                    c.moveTo(xTickPos, plot.top + j * dashedLineLength)
                    c.lineTo(xTickPos, plot.top + (j + dashedLineDutyCycle) * dashedLineLength)
                }

                lastDateStr = xTickDateStr
            }
        }
        c.stroke()
    }
}
*/
