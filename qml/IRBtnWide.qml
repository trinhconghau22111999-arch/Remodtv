import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: ibw
    property string lbl: "🏠"; property string sub: ""; signal tapped()
    Layout.fillWidth: true; height: 52; radius: 10
    color: ma2.pressed ? theme.c3 : theme.c1
    border.color: theme.c2; border.width: 0.5
    Behavior on color { ColorAnimation { duration: 80 } }
    ColumnLayout { anchors.centerIn: parent; spacing: 2
        Text { Layout.alignment: Qt.AlignHCenter; text: ibw.lbl; font.pixelSize: 20; color: theme.tx }
        Text { Layout.alignment: Qt.AlignHCenter; text: ibw.sub; font.pixelSize: 9; color: theme.t3 }
    }
    MouseArea { id: ma2; anchors.fill: parent; onClicked: ibw.tapped() }
    scale: ma2.pressed ? 0.94 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
