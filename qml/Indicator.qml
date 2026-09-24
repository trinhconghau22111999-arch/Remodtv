import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: ind
    width: 22; height: 22; radius: 11; color: "transparent"
    property string phase: "wait" // wait | spin | ok | fail

    // Spin
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: "transparent"
        border.color: theme.blue; border.width: 2
        visible: ind.phase === "spin"
        RotationAnimation on rotation {
            running: ind.phase === "spin"
            from: 0; to: 360; duration: 800; loops: Animation.Infinite
        }
    }
    // OK
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.green; visible: ind.phase === "ok"
        Text { anchors.centerIn: parent; text: "✓"; color: "white"; font.pixelSize: 12; font.weight: Font.Bold }
    }
    // Wait
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.c2; visible: ind.phase === "wait"
    }
    // Fail
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.c2; visible: ind.phase === "fail"
        Text { anchors.centerIn: parent; text: "–"; color: theme.t3; font.pixelSize: 14 }
    }
}
