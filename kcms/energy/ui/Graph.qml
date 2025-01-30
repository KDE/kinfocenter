/*
 * SPDX-FileCopyrightText: 2015 David Edmundson <david@davidedmundson.co.uk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

import QtQuick

/**
 * We need to draw a graph, all other libs are not suitable as we are basically
 * a connected scatter plot with non linear X spacing.
 * Currently this is not available in kdeclarative nor kqtquickcharts
 *
 * We only paint once, so canvas is fast enough for our purposes.
 * It is designed to look identical to those in ksysguard.
 */

Canvas
{
    id: canvas

    antialiasing: true

    property int xPadding: 45
    property int yPadding: 40

    property var data //expect an array of QPointF

    property int yMin: 0
    property int yMax: 100
    property int yStep: 20

    property var yLabel: ( value => value )  // A formatter function

    property int xDuration: 3600

    readonly property int plotWidth: Math.round(width - xPadding * 1.5)
    readonly property int plotHeight: height - yPadding * 2

    onDataChanged: {
        canvas.requestPaint();
    }

    function scalePoint(plot : point, currentUnixTime : int) : point {
        const scaledX = Math.round((plot.x - (currentUnixTime - xDuration)) / xDuration * plotWidth);
        const scaledY = Math.round((plot.y - yMin) * plotHeight / (yMax - yMin));

        return Qt.point(
            xPadding + scaledX,
            height - yPadding - scaledY
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

    onPaint: {
        const c = canvas.getContext('2d');

        c.clearRect(0, 0, width, height)

        //draw the background
        c.fillStyle = palette.base
        c.fillRect(xPadding, yPadding, plotWidth, plotHeight);

        //reset for fonts and stuff
        c.fillStyle = palette.text

        //Draw the lines

        c.lineWidth = 2;
        c.lineJoin = 'round';
        c.lineCap = 'round';

        const lineColor = palette.accent
        c.strokeStyle = lineColor;
        const gradient = c.createLinearGradient(0, 0, 0, height);
        gradient.addColorStop(0, Qt.alpha(lineColor, 0.2));
        gradient.addColorStop(1, Qt.alpha(lineColor, 0.05));
        c.fillStyle = gradient;

        // For scaling
        const currentUnixTime = Date.now() / 1000  // ms to s

        c.beginPath();

        // Draw the line graph if we have enough points
        if (data.length >= 2) {
            let firstPoint = null;
            let point;
            for (const dataPoint of data) {
                if (dataPoint.x < currentUnixTime - xDuration) {
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

            c.stroke();
            c.strokeStyle = 'transparent';
            c.lineTo(point.x, height - yPadding);
            c.lineTo(firstPoint.x, height - yPadding);
            c.fill();
        }

        c.closePath()

        // Draw the frame on top

        //draw an outline
        c.strokeStyle = Qt.rgba(0, 50, 0, 0.02);
        c.lineWidth = 1;
        c.rect(xPadding - 1, yPadding - 1, plotWidth + 2, plotHeight + 2);

        // Draw the Y value texts
        c.fillStyle = palette.text;
        c.textAlign = "right"
        c.textBaseline = "middle";

        for(let i = 0; i <= yMax; i += yStep) {
            const y = scalePoint(Qt.point(0, i)).y;

            c.fillText(canvas.yLabel(i), xPadding - 10, y);

            //grid line
            c.moveTo(xPadding, y)
            c.lineTo(plotWidth + xPadding, y)
        }
        c.stroke()

        // Draw the X value texts
        c.textAlign = "center"
        c.lineWidth = 1
        c.strokeStyle = Qt.alpha(palette.text, 0.15)

        const xStep = stepForDuration(xDuration)
        const xDivisions = xDuration / xStep
        const xGridDistance = plotWidth / xDivisions

        const currentDayTime = new Date()

        const xOffset = offsetForStep(xStep)
        const xGridOffset = plotWidth * (xOffset / xDuration)

        const dashedLines = 50
        const dashedLineLength = plotHeight / dashedLines

        let lastDateStr = currentDayTime.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
        let dateChanged = false

        for (let i = xDivisions; i >= -1; i--) {
            const xTickPos = i * xGridDistance + xPadding - xGridOffset

            if ((xTickPos > xPadding) && (xTickPos < plotWidth + xPadding)) {
                const xTickDateTime = new Date((currentUnixTime - (xDivisions - i) * xStep - xOffset) * 1000)
                const xTickDateStr = xTickDateTime.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
                const xTickTimeStr = xTickDateTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)

                if (lastDateStr != xTickDateStr) {
                    dateChanged = true
                }

                let dashedLineDutyCycle = 0.1
                if  ((i % 2 == 0) || (xDivisions < 10)) {
                    // Display the time
                    c.fillText(xTickTimeStr, xTickPos, canvas.height - yPadding / 2)

                    // If the date has changed and is not the current day in a <= 24h graph, display it
                    // Always display the date for 48h and 1 week graphs
                    if (dateChanged || (xDuration > (60*60*48))) {
                        c.fillText(xTickDateStr, xTickPos, canvas.height - yPadding / 4)
                        dateChanged = false
                    }

                    // Tick markers
                    c.moveTo(xTickPos, canvas.height - yPadding)
                    c.lineTo(xTickPos, canvas.height - (yPadding * 4) / 5)

                    dashedLineDutyCycle = 0.5
                }

                for (let j = 0; j < dashedLines; j++) {
                    c.moveTo(xTickPos, yPadding + j * dashedLineLength)
                    c.lineTo(xTickPos, yPadding + (j + dashedLineDutyCycle) * dashedLineLength)
                }

                lastDateStr = xTickDateStr
            }
        }
        c.stroke()
    }
}
