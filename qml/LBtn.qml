import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: lb
    property string lbl:    "▶"
    property string sub:    ""
    property bool   accent: false
    property bool   active: false
    signal tapped()

    Layout.fillWidth: true; height: 56; radius: 10
    color: accent ? Qt.rgba(0.04,0.52,1,0.18) : active ? Qt.rgba(0.19,0.82,0.35,0.12) : theme.c1
    border.color: accent ? Qt.rgba(0.04,0.52,1,0.35) : active ? Qt.rgba(0.19,0.82,0.35,0.3) : theme.c2
    border.width: 0.5

    ColumnLayout { anchors.centerIn: parent; spacing: 3
        Text { Layout.alignment: Qt.AlignHCenter; text: lb.lbl; font.pixelSize: 19
            color: lb.accent ? theme.blue : lb.active ? theme.green : theme.tx }
        Text { Layout.alignment: Qt.AlignHCenter; text: lb.sub; font.pixelSize: 9; color: theme.t2 }
    }
    MouseArea { id: ma; anchors.fill: parent; onClicked: lb.tapped() }
    scale: ma.pressed ? 0.94 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
