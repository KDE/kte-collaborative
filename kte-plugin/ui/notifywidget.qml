import QtQuick 1.1

Rectangle {
    id: root
    property string username: "not set"
    property string widgetcolor: "#FF9900";
    property string wasVisibleBefore: "false"
    property bool outsideView: false
    property double defaultOpacity: root.outsideView ? 0.5 : 0.8
    color: "#00000000"
    // changed by the content animation on startup
    opacity: root.defaultOpacity
    property int arrowOffset: 0
    // TODO use states?
    function reset() {
        root.opacity = root.defaultOpacity
        content.x = 10;
    }
//     property string arrowAlignment: "top"
    Rectangle {
        x: 8
        y: 11
        id: content
        border.color: "black"
        border.width: 1
        Text {
            id: user_text
            text: root.username
            anchors.centerIn: parent
            font.pointSize: root.outsideView ? 6 : 8
        }
        anchors.margins: 5
        width: user_text.width + 12
        height: user_text.height + 6
        color: root.widgetcolor
        radius: 4
        SequentialAnimation {
            objectName: "hideAnimation"
            // Should add up to 3 seconds, as defined in the qtimer which hides the widget.
            // Shorter is ok too, but you should adjust the timer then.
            PauseAnimation { duration: 2600 }
            ParallelAnimation {
                NumberAnimation { target: root; property: "opacity"; to: 0; duration: 200 }
                NumberAnimation { target: content; property: "x"; to: 20; duration: 300 }
            }
            running: true
            loops: 1
        }
        // The little arrow
        Rectangle {
            visible: ! root.outsideView
            anchors.horizontalCenter: parent.left
            anchors.horizontalCenterOffset: +12 + root.arrowOffset
            anchors.top: parent.top
            color: "#00000000"
            Rectangle {
                id: arrow
                color: "#00000000"
                width: 8
                height: 5
                y: -5.5
                clip: true // magic trick to get only two borders
                rotation: 180
                Rectangle {
                    y: -8
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
    width: content.width + 2 + 30 // some extra space for animation
    height: content.height + arrow.height/2 + 2 + 10
}