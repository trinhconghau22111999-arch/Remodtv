import QtQuick
import QtQuick.Controls

Item {
    id: root
    width: 170; height: 170

    signal upPressed()
    signal downPressed()
    signal leftPressed()
    signal rightPressed()
    signal okPressed()

    // Vòng nền
    Rectangle {
        anchors.fill: parent; radius: width / 2
        color: theme.c1; border.color: theme.c2; border.width: 0.5
    }

    // Đường chia +
    Rectangle { anchors.horizontalCenter: parent.horizontalCenter; width: 0.5; height: parent.height; color: theme.c2 }
    Rectangle { anchors.verticalCenter: parent.verticalCenter; height: 0.5; width: parent.width; color: theme.c2 }

    // UP
    DirButton {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top; anchors.topMargin: 4
        width: 54; height: 54
        label: "▲"
        onTapped: root.upPressed()
    }

    // DOWN
    DirButton {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom; anchors.bottomMargin: 4
        width: 54; height: 54
        label: "▼"
        onTapped: root.downPressed()
    }

    // LEFT
    DirButton {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left; anchors.leftMargin: 4
        width: 54; height: 54
        label: "◀"
        onTapped: root.leftPressed()
    }

    // RIGHT
    DirButton {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right; anchors.rightMargin: 4
        width: 54; height: 54
        label: "▶"
        onTapped: root.rightPressed()
    }

    // OK center
    Rectangle {
        width: 60; height: 60; radius: 30
        anchors.centerIn: parent
        color: okArea.pressed ? Qt.darker(theme.blue, 1.2) : theme.blue
        Behavior on color { ColorAnimation { duration: 100 } }

        ColumnLayout {
            anchors.centerIn: parent; spacing: 1
            Text { Layout.alignment: Qt.AlignHCenter; text: "✓"; color: "white"; font.pixelSize: 22; font.weight: Font.Medium }
            Text { Layout.alignment: Qt.AlignHCenter; text: "Chọn"; color: "white"; font.pixelSize: 9 }
        }

        MouseArea {
            id: okArea; anchors.fill: parent
            onClicked: root.okPressed()
        }
        scale: okArea.pressed ? 0.93 : 1.0
        Behavior on scale { NumberAnimation { duration: 80 } }
    }
}

// Nút hướng
component DirButton: Rectangle {
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
