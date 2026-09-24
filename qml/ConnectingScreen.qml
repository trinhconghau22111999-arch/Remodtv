import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property string deviceName: "TV"
    property string ip: ""
    property bool   hasCast: true
    property bool   hasDial: true

    // Theo dõi trạng thái kết nối
    Connections {
        target: remote
        function onCastStateChanged() { updateCastUI() }
        function onDialStateChanged() { updateDialUI() }
        function onAnyReadyChanged()  { if (remote.anyReady) checkBothDone() }
    }

    function updateCastUI() {
        var s = remote.castState
        if (s === "connected") {
            castStatus.text  = "Đã kết nối · Cast"
            castStatus.color = theme.green
            castInd.state    = "ok"
            progress.value   = hasDial ? 0.6 : 1.0
        } else if (s === "connecting") {
            castStatus.text  = "Đang kết nối..."
            castStatus.color = theme.t2
            castInd.state    = "spin"
        } else if (s === "error") {
            castStatus.text  = "Không thể kết nối Cast"
            castStatus.color = theme.red
            castInd.state    = "fail"
        }
    }

    function updateDialUI() {
        var s = remote.dialState
        if (s === "connected") {
            dialStatus.text  = "Đã kết nối · DIAL"
            dialStatus.color = theme.green
            dialInd.state    = "ok"
            progress.value   = 1.0
        } else if (s === "connecting") {
            dialStatus.text  = "Đang kết nối..."
            dialStatus.color = theme.t2
            dialInd.state    = "spin"
        } else if (s === "error") {
            dialStatus.text  = "Không hỗ trợ DIAL"
            dialStatus.color = theme.t3
            dialInd.state    = "fail"
        }
    }

    function checkBothDone() {
        var castDone = !hasCast || remote.castState === "connected" || remote.castState === "error"
        var dialDone = !hasDial || remote.dialState === "connected" || remote.dialState === "error"
        if (castDone && dialDone)
            doneTimer.start()
    }

    Timer {
        id: doneTimer; interval: 400; repeat: false
        onTriggered: root.Window.window.goRemote()
    }

    Component.onCompleted: {
        remote.connectToDevice(ip, hasCast, hasDial, deviceName)
        // Init UI
        if (!hasCast) { castStatus.text = "Không hỗ trợ"; castStatus.color = theme.t3; castInd.state = "fail" }
        if (!hasDial) { dialStatus.text = "Không hỗ trợ"; dialStatus.color = theme.t3; dialInd.state = "fail" }
        if (hasCast)  progress.value = 0.3
    }

    ColumnLayout {
        anchors { fill: parent; margins: 20 }
        spacing: 0

        // Header
        RowLayout {
            Layout.topMargin: 16
            spacing: 10
            Rectangle { width: 34; height: 34; radius: 9; color: theme.red
                Text { anchors.centerIn: parent; text: "📺"; font.pixelSize: 17 } }
            ColumnLayout {
                spacing: 1
                Text { text: root.deviceName; font.pixelSize: 15; font.weight: Font.Medium; color: theme.tx }
                Text { text: "Đang kết nối song song..."; font.pixelSize: 11; color: theme.t3 }
            }
        }

        Item { height: 32 }

        // Cast step
        Rectangle {
            Layout.fillWidth: true
            height: 64
            radius: 12
            color: theme.c1
            border.color: theme.c2; border.width: 0.5

            RowLayout {
                anchors { fill: parent; margins: 12 }
                spacing: 10
                Rectangle { width: 36; height: 36; radius: 9; color: Qt.rgba(0.04,0.52,1,0.15)
                    Text { anchors.centerIn: parent; text: "📡"; font.pixelSize: 18 } }
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 2
                    Text { text: "Google Cast"; font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx }
                    Text { id: castStatus; text: "Chờ..."; font.pixelSize: 10; color: theme.t2 }
                }
                Indicator { id: castInd; state: hasCast ? "spin" : "fail" }
            }
        }

        Item { height: 8 }

        // DIAL step
        Rectangle {
            Layout.fillWidth: true
            height: 64
            radius: 12
            color: theme.c1
            border.color: theme.c2; border.width: 0.5

            RowLayout {
                anchors { fill: parent; margins: 12 }
                spacing: 10
                Rectangle { width: 36; height: 36; radius: 9; color: Qt.rgba(0.75,0.35,0.95,0.15)
                    Text { anchors.centerIn: parent; text: "📶"; font.pixelSize: 18 } }
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 2
                    Text { text: "DIAL Protocol"; font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx }
                    Text { id: dialStatus; text: hasCast ? "Chờ Cast xong..." : "Đang kết nối..."; font.pixelSize: 10; color: theme.t3 }
                }
                Indicator { id: dialInd; state: hasDial ? "wait" : "fail" }
            }
        }

        Item { height: 20 }

        // Progress bar
        Rectangle {
            Layout.fillWidth: true; height: 4; radius: 2; color: theme.c2
            Rectangle {
                id: progress
                property real value: 0
                width: parent.width * value; height: parent.height; radius: parent.radius
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0; color: theme.blue }
                    GradientStop { position: 1; color: theme.purple }
                }
                Behavior on width { NumberAnimation { duration: 400; easing.type: Easing.OutCubic } }
            }
        }

        Item { Layout.fillHeight: true }

        Button {
            Layout.fillWidth: true; height: 44
            text: "Huỷ"
            onClicked: root.Window.window.goConnect()
            contentItem: Text { text: parent.text; color: theme.t2; font.pixelSize: 13; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            background: Rectangle { radius: 10; color: theme.c1; border.color: theme.c2; border.width: 0.5 }
        }
        Item { height: 24 }
    }
}

// Indicator component inline
component Indicator: Rectangle {
    id: ind
    width: 22; height: 22; radius: 11; color: "transparent"
    property string state: "wait" // wait | spin | ok | fail

    // Spin
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: "transparent"
        border.color: theme.blue; border.width: 2
        visible: ind.state === "spin"
        RotationAnimation on rotation {
            running: ind.state === "spin"
            from: 0; to: 360; duration: 800; loops: Animation.Infinite
        }
    }
    // OK
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.green; visible: ind.state === "ok"
        Text { anchors.centerIn: parent; text: "✓"; color: "white"; font.pixelSize: 12; font.weight: Font.Bold }
    }
    // Wait
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.c2; visible: ind.state === "wait"
    }
    // Fail
    Rectangle {
        anchors.fill: parent; radius: parent.radius
        color: theme.c2; visible: ind.state === "fail"
        Text { anchors.centerIn: parent; text: "–"; color: theme.t3; font.pixelSize: 14 }
    }
}
