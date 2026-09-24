import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: btn
    property string lbl: "▲"
    property string iconShape: ""   // "" = dùng lbl (text) | "mute" | "input"
    signal tapped()
    radius: 8; color: ma.pressed ? theme.c3 : theme.c2
    Behavior on color { ColorAnimation { duration: 80 } }

    property color fg: ma.pressed ? theme.tx : theme.t2

    Text {
        visible: btn.iconShape === ""
        anchors.centerIn: parent; text: btn.lbl; color: btn.fg; font.pixelSize: 18
    }

    // Icon "tắt tiếng" — vòng tròn gạch chéo, vẽ vector
    Item {
        visible: btn.iconShape === "mute"
        anchors.centerIn: parent
        width: 20; height: 20
        Rectangle {
            anchors.fill: parent
            radius: width / 2
            color: "transparent"
            border.color: btn.fg; border.width: 2
        }
        Rectangle {
            anchors.centerIn: parent
            width: 2.5; height: 26
            rotation: 45
            color: theme.red
        }
    }

    // Icon "chọn nguồn vào" — 3 vạch ngang, vẽ vector
    ColumnLayout {
        visible: btn.iconShape === "input"
        anchors.centerIn: parent
        spacing: 3
        Rectangle { Layout.alignment: Qt.AlignHCenter; width: 18; height: 2.5; radius: 1; color: btn.fg }
        Rectangle { Layout.alignment: Qt.AlignHCenter; width: 12; height: 2.5; radius: 1; color: btn.fg }
        Rectangle { Layout.alignment: Qt.AlignHCenter; width: 18; height: 2.5; radius: 1; color: btn.fg }
    }

    MouseArea { id: ma; anchors.fill: parent; onClicked: btn.tapped() }
    scale: ma.pressed ? 0.92 : 1.0
    Behavior on scale { NumberAnimation { duration: 80 } }
}
