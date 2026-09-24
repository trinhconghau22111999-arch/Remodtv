import QtQuick

// Icon TV vẽ bằng vector — không phụ thuộc font/emoji của máy
Item {
    id: tvIcon
    property color iconColor: "white"
    width: 20; height: 20

    Rectangle {
        id: screen
        anchors.top: parent.top
        width: parent.width; height: parent.height * 0.66
        radius: 3
        color: "transparent"
        border.color: tvIcon.iconColor
        border.width: 2
    }
    Rectangle {
        anchors.top: screen.bottom
        anchors.topMargin: 3
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.42; height: 2
        radius: 1
        color: tvIcon.iconColor
    }
    Rectangle {
        anchors.top: screen.bottom
        anchors.topMargin: 6.5
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.68; height: 2
        radius: 1
        color: tvIcon.iconColor
    }
}
