import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: nb
    property string lbl: "⌂"
    property string sub: ""
    property bool active: false
    signal tapped()

    Layout.fillWidth: true; height: 54; radius: 10
    color: active ? Qt.rgba(0.19,0.82,0.35,0.12) : theme.c1
    border.color: active ? Qt.rgba(0.19,0.82,0.35,0.3) : theme.c2; border.width: 0.5
    ColumnLayout { anchors.centerIn: parent; spacing: 2
        Text { Layout.alignment: Qt.AlignHCenter; text: nb.lbl; font.pixelSize: 19; color: nb.active ? theme.green : theme.tx }
        Text { Layout.alignment: Qt.AlignHCenter; text: nb.sub; font.pixelSize: 9; color: theme.t2 }
    }
    MouseArea { anchors.fill: parent; onClicked: nb.tapped() }
}
