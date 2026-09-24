import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: ibw
    property string lbl: ""
    property string sub: ""
    property string iconShape: ""   // "" = dùng lbl (text) | "home"
    signal tapped()
    Layout.fillWidth: true; height: 52; radius: 10
    color: ma2.pressed ? theme.c3 : theme.c1
    border.color: theme.c2; border.width: 0.5
    Behavior on color { ColorAnimation { duration: 80 } }

    ColumnLayout {
        anchors.centerIn: parent; spacing: 3

        // Icon "Home" — hình nhà vẽ vector
        Item {
            Layout.alignment: Qt.AlignHCenter
            visible: ibw.iconShape === "home"
            width: 18; height: 17
            Rectangle {
                id: body
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: 12; height: 8
                color: theme.tx
            }
            Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                width: 11; height: 11
                rotation: 45
                color: theme.tx
            }
        }

        Text {
            visible: ibw.iconShape === ""
            Layout.alignment: Qt.AlignHCenter; text: ibw.lbl; font.pixelSize: 20; color: theme.tx
        }
        Text { Layout.alignment: Qt.AlignHCenter; text: ibw.sub; font.pixelSize: 9; color: theme.t3 }
    }
    MouseArea { id: ma2; anchors.fill: parent; onClicked: ibw.tapped() }
    scale: ma2.pressed ? 0.94 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
