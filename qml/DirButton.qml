import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    property string label: "▲"
    signal tapped()

    radius: width / 2; color: "transparent"
    MouseArea {
        id: ma; anchors.fill: parent
        onClicked: parent.tapped()
    }
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.blue; opacity: ma.pressed ? 0.15 : 0
        Behavior on opacity { NumberAnimation { duration: 80 } }
    }
    Text {
        anchors.centerIn: parent
        text: parent.label; color: theme.tx; font.pixelSize: 20
    }
    scale: ma.pressed ? 0.92 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
