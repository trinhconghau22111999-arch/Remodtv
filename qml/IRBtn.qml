import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    property string lbl: "▲"; signal tapped()
    radius: 8; color: ma.pressed ? theme.c3 : theme.c2
    Behavior on color { ColorAnimation { duration: 80 } }
    Text { anchors.centerIn: parent; text: parent.lbl; color: ma.pressed ? theme.tx : theme.t2; font.pixelSize: 18 }
    MouseArea { id: ma; anchors.fill: parent; onClicked: parent.tapped() }
    scale: ma.pressed ? 0.92 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
