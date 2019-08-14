/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2015  David Edmundson <david@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

import QtQuick 2.3

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
    width: 500
    height: 500
    id: canvas
    antialiasing: true

    property int xPadding: 45
    property int yPadding: 10

    property var data //expect an array of QPointF

    property real yMax: 100
    property real xMax: 100
    property real yMin: 0
    property real xMin: 0
    property real yStep: 20

    property string yUnits: ""
    property string xUnits: ""

    //internal

    property real plotWidth: width - xPadding
    property real plotHeight: height - yPadding *2

    onDataChanged: {
        canvas.requestPaint();
    }

    //take a QPointF
    function scalePoint(plot) {
        var scaledX = (plot.x - xMin) * plotWidth / (xMax-xMin);
        var scaledY = (plot.y - yMin)  * plotHeight / (yMax-yMin);

        return Qt.point(xPadding + scaledX,
            height - yPadding - scaledY);
    }

    SystemPalette {
        id: palette;
        colorGroup: SystemPalette.Active
    }


    onPaint: {
        var c = canvas.getContext('2d');

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
        c.strokeStyle = 'rgba(255, 0, 0, 1)';
        var gradient = c.createLinearGradient(0,0,0,height);
        gradient.addColorStop(0, 'rgba(255, 0, 0, 0.2)');
        gradient.addColorStop(1, 'rgba(255, 0, 0, 0.05)');
        c.fillStyle = gradient;

        // Draw the line graph
        c.beginPath();
        var point = scalePoint(data[0]);
        c.moveTo(point.x, point.y);
        for(var i = 1; i < data.length; i ++) {
            point = scalePoint(data[i])
            c.lineTo(point.x, point.y);
        }
        c.stroke();
        c.lineTo(point.x, height-yPadding);
        c.lineTo(xPadding, height-yPadding);
        c.fill();

        c.closePath()


        // Draw the frame on top

        //draw an outline
        c.strokeStyle = 'rgba(0,0,0,0.02)';
        c.lineWidth = 1;
        c.rect(xPadding-1, yPadding-1, plotWidth+2, plotHeight+2);

        // Draw the Y value texts
        c.fillStyle = palette.text;
        c.textAlign = "right"
        c.textBaseline = "middle";
        for(var i = 0; i <=  yMax; i += yStep) {
            var y = scalePoint(Qt.point(0,i)).y;

            c.fillText(i + canvas.yUnits, xPadding - 10, y);

            //grid line
            c.moveTo(xPadding,y)
            c.lineTo(canvas.width, y)
            c.stroke()
        }
    }
}
