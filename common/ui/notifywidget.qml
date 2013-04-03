import QtQuick 1.1

Rectangle {
    id: root
    property string username: "not set"
    color: "#00000000"
    opacity: 0.7
    Rectangle {
        x: 10
        y: 10
        id: content
        border.color: "black"
        border.width: 1
        Text {
            id: user_text
            text: root.username
            anchors.centerIn: parent
        }
        anchors.margins: 5
        width: user_text.width + 20
        height: user_text.height + 10
        color: "#FF9900"
        radius: 4
        SequentialAnimation {
            PauseAnimation { duration: 2500 }
            NumberAnimation { target: root; property: "opacity"; to: 0; duration: 500 }
            running: true
            loops: 1
        }
        Rectangle {
            anchors.horizontalCenter: parent.left
            anchors.horizontalCenterOffset: +15
            anchors.top: parent.top
            color: "#00000000"
            Rectangle {
                id: arrow
                color: "#00000000"
                width: 8
                height: 8
                y: -5.5
                clip: true // magic trick to get only two borders
                rotation: 180
                Rectangle {
                    y: -5
                    width: 8
                    height: 8
                    border.width: content.border.width
                    border.color: content.border.color
                    color: content.color
                    rotation: -45+90
                }
            }
            width: arrow.width
            height: arrow.height
        }
    }
    width: content.width + 2 + 10
    height: content.height + arrow.height/2 + 2 + 10
}