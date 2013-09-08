/***************************************************************************
 *   Copyright 2013 Sven Brauch <svenbrauch@gmail.com>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

import QtQuick 1.1

Rectangle {
    width: 800; height: 600 // just for debugging purposes
    id: root
    color: "#99222222"

    Text {
        objectName: "text"
        text: "<no text>"
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20
    }

    Text {
        objectName: "subtitle"
        color: "#AAAAAA"
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 40
        text: "<no text>"
    }

    ListView {
        z: 2
        id: pbar
        objectName: "progressBar"
        property double progress: 0.0
        anchors.centerIn: parent
        rotation: 270
        model: 12
        spacing: 3
        property int childWidth: 8
        height: (childWidth+spacing) * model
        width: childWidth+4
        delegate: Rectangle {
            width: pbar.childWidth
            height: pbar.childWidth
            border.color: "#1B4AA1"
            border.width: 1
            opacity: ( index > (pbar.progress*pbar.model)-1 || pbar.progress == 0 ) ? 0.35 : 1.0
            color: "#296FF0"
        }
    }

    ListView {
        anchors.fill: parent
        model: Math.floor(parent.height / 2)
        spacing: 2
        delegate: Rectangle {
            width: root.width
            height: 1
            color: "black"
            opacity: 0.1
        }
    }
    gradient: Gradient {
         GradientStop { position: 0.0; color: "#99222222" }
         GradientStop { position: 0.5; color: "#B5222222" }
         GradientStop { position: 1.0; color: "#99222222" }
     }
}
