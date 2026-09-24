import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: rb
    property string lbl:    "▶"
    property string sub:    ""
    property string pTag:   ""
    property bool   accent: false
    property bool   active: false
    signal tapped()

    Layout.fillWidth: true; height: 58; radius: 10
    color: accent ? Qt.rgba(0.04,0.52,1,0.18) : active ? Qt.rgba(0.19,0.82,0.35,0.12) : theme.c1
    border.color: accent ? Qt.rgba(0.04,0.52,1,0.35) : active ? Qt.rgba(0.19,0.82,0.35,0.3) : theme.c2
    border.width: 0.5

    // Protocol tag
    Rectangle {
        visible: rb.pTag !== ""
        anchors.top: parent.top; anchors.right: parent.right
        anchors.topMargin: 4; anchors.rightMargin: 4
        radius: 4; color: rb.pTag === "Cast" ? Qt.rgba(0.04,0.52,1,0.2) : Qt.rgba(0.75,0.35,0.95,0.2)
        width: ptxt.width + 8; height: 14
        Text { id: ptxt; anchors.centerIn: parent; text: rb.pTag; font.pixelSize: 8
            color: rb.pTag === "Cast" ? theme.blue : theme.purple }
    }

    ColumnLayout {
        anchors.centerIn: parent; spacing: 2
        Text { Layout.alignment: Qt.AlignHCenter; text: rb.lbl; font.pixelSize: 20
            color: rb.accent ? theme.blue : rb.active ? theme.green : theme.tx }
        Text { Layout.alignment: Qt.AlignHCenter; text: rb.sub; font.pixelSize: 9; color: theme.t2 }
    }

    MouseArea { id: ma; anchors.fill: parent; onClicked: rb.tapped() }
    scale: ma.pressed ? 0.94 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
