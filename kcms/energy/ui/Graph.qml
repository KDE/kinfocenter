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

    property real yMax: 100
    property real xMax: 100
    property real yMin: 0
    property real xMin: 0
    property real yStep: 20

    property var yLabel: ( value => value )  // A formatter function

    property real xDuration: 3600
    property real xDivisions: 6
    property real xDivisionWidth: 600000
    property real xTicksAt: xTicksAtDontCare

    readonly property real plotWidth: width - xPadding * 1.5
    readonly property real plotHeight: height - yPadding * 2

    onDataChanged: {
        canvas.requestPaint();
    }

    function scalePoint(plot /*: point*/, currentUnixTime : int) : point {
        const scaledX = (plot.x - (currentUnixTime / 1000 - xDuration)) / xDuration * plotWidth;
        const scaledY = (plot.y - yMin) * plotHeight / (yMax - yMin);

        return Qt.point(
            xPadding + scaledX,
            height - yPadding - scaledY
        );
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

        c.clearRect(0,0, width, height)

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
        const currentUnixTime = Date.now()
        const xMinUnixTime = currentUnixTime - xDuration * 1000

        c.beginPath();

        // Draw the line graph if we have enough points
        if (data.length >= 2) {
            let index = 0

            while ((index < data.length - 1) && (data[index].x < (xMinUnixTime / 1000))) {
                index++
            }

            const firstPoint = scalePoint(data[index], currentUnixTime)
            c.moveTo(firstPoint.x, firstPoint.y)

            let point
            for (let i = index + 1; i < data.length; i++) {
                if (data[i].x > (xMinUnixTime / 1000)) {
                    point = scalePoint(data[i], currentUnixTime)
                    c.lineTo(point.x, point.y)
                }
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
        for(let i = 0; i <=  yMax; i += yStep) {
            const y = scalePoint(Qt.point(0,i)).y;

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

        const xDivisions = xDuration / xDivisionWidth * 1000
        const xGridDistance = plotWidth / xDivisions
        let xTickPos
        let xTickDateTime
        let xTickDateStr
        let xTickTimeStr

        const currentDateTime = new Date()
        let lastDateStr = currentDateTime.toLocaleDateString(Qt.locale(), Locale.ShortFormat)

        const hours = currentDateTime.getHours()
        const minutes = currentDateTime.getMinutes()
        const seconds = currentDateTime.getSeconds()

        let diff

        switch (xTicksAt) {
            case xTicksAtTwelveOClock:
                diff = ((hours - 12) * 60 * 60 + minutes * 60 + seconds)
                break
            case xTicksAtFullHour:
                diff = (minutes * 60 + seconds)
                break
            case xTicksAtFullSecondHour:
                diff = (minutes * 60 + seconds)
                break
            case xTicksAtHalfHour:
                diff = ((minutes - 30) * 60 + seconds)
                break
            case xTicksAtTenMinutes:
                diff = ((minutes % 10) * 60 + seconds)
                break
            default:
                diff = 0
        }

        const xGridOffset = plotWidth * (diff / xDuration)
        let dateChanged = false

        const dashedLines = 50
        const dashedLineLength = plotHeight / dashedLines
        let dashedLineDutyCycle

        for (let i = xDivisions; i >= -1; i--) {
            xTickPos = i * xGridDistance + xPadding - xGridOffset

            if ((xTickPos > xPadding) && (xTickPos < plotWidth + xPadding))
            {
                xTickDateTime = new Date(currentUnixTime - (xDivisions - i) * xDivisionWidth - diff * 1000)
                xTickDateStr = xTickDateTime.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
                xTickTimeStr = xTickDateTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)

                if (lastDateStr != xTickDateStr) {
                    dateChanged = true
                }

                if  ((i % 2 == 0) || (xDivisions < 10))
                {
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
                } else {
                    dashedLineDutyCycle = 0.1
                }

                for (let j = 0; j < dashedLines; j++) {
                    c.moveTo(xTickPos, yPadding + j * dashedLineLength)
                    c.lineTo(xTickPos, yPadding + j * dashedLineLength + dashedLineDutyCycle * dashedLineLength)
                }
               lastDateStr = xTickDateStr
            }
        }
        c.stroke()
    }
}
