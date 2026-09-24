import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Kết nối scanner
    Connections {
        target: scanner
        function onScanningChanged() { scanBtn.text = scanner.scanning ? "Đang quét..." : "Quét lại" }
        function onDevicesChanged()  { deviceList.model = scanner.devices }
    }

    Component.onCompleted: scanner.startScan()

    Flickable {
        anchors.fill: parent
        contentHeight: col.height + 32
        clip: true

        ColumnLayout {
            id: col
            width: parent.width
            spacing: 0

            // Header
            RowLayout {
                Layout.fillWidth: true
                Layout.margins: 16
                Layout.topMargin: 20
                spacing: 10

                Rectangle {
                    width: 36; height: 36
                    radius: 9
                    color: theme.red
                    Text { anchors.centerIn: parent; text: "📺"; font.pixelSize: 18 }
                }
                ColumnLayout {
                    spacing: 1
                    Text { text: "TV Remote";        font.pixelSize: 16; font.weight: Font.Medium; color: theme.tx }
                    Text { text: "Dual-Protocol · WiFi"; font.pixelSize: 11; color: theme.t3 }
                }
                Item { Layout.fillWidth: true }
                Rectangle {
                    radius: 20; color: theme.c1
                    border.color: theme.c2; border.width: 0.5
                    width: wifiRow.width + 20; height: 28
                    RowLayout {
                        id: wifiRow
                        anchors.centerIn: parent
                        spacing: 5
                        Text { text: "⚡"; font.pixelSize: 12 }
                        Text { text: "HomeNet"; font.pixelSize: 11; color: theme.green }
                    }
                }
            }

            // Dual Protocol Banner
            Rectangle {
                Layout.fillWidth: true
                Layout.margins: 16
                height: 80
                radius: theme.radius
                color: Qt.rgba(0.04, 0.52, 1, 0.08)
                border.color: Qt.rgba(0.04, 0.52, 1, 0.2)
                border.width: 0.5

                RowLayout {
                    anchors { fill: parent; margins: 12 }
                    spacing: 10
                    Rectangle {
                        width: 36; height: 36; radius: 9
                        color: Qt.rgba(0.04, 0.52, 1, 0.15)
                        Text { anchors.centerIn: parent; text: "⚡"; font.pixelSize: 18 }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: "Kết nối song song"; font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx }
                        Text { text: "Cast + DIAL chạy cùng lúc — không ngắt quãng"; font.pixelSize: 10; color: theme.t2; wrapMode: Text.Wrap; Layout.fillWidth: true }
                        RowLayout {
                            spacing: 6
                            Rectangle { radius: 20; color: Qt.rgba(0.04,0.52,1,0.2); width: castLbl.width+12; height: 18
                                Text { id: castLbl; anchors.centerIn: parent; text: "Cast"; font.pixelSize: 9; color: theme.blue } }
                            Text { text: "+"; color: theme.t3; font.pixelSize: 11 }
                            Rectangle { radius: 20; color: Qt.rgba(0.75,0.35,0.95,0.2); width: dialLbl.width+12; height: 18
                                Text { id: dialLbl; anchors.centerIn: parent; text: "DIAL"; font.pixelSize: 9; color: theme.purple } }
                        }
                    }
                }
            }

            // IR Remote entry point
            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                Layout.bottomMargin: 4
                height: 64
                radius: theme.radius
                color: Qt.rgba(1, 0.62, 0.04, 0.08)
                border.color: Qt.rgba(1, 0.62, 0.04, 0.2)
                border.width: 0.5

                RowLayout {
                    anchors { fill: parent; margins: 12 }
                    spacing: 10
                    Rectangle {
                        width: 36; height: 36; radius: 9
                        color: Qt.rgba(1, 0.62, 0.04, 0.15)
                        Text { anchors.centerIn: parent; text: "📡"; font.pixelSize: 18 }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: "Điều khiển bằng IR"; font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx }
                        Text { text: "Dùng hồng ngoại của điện thoại — không cần WiFi"; font.pixelSize: 10; color: theme.t2; wrapMode: Text.Wrap; Layout.fillWidth: true }
                    }
                    Text { text: "›"; font.pixelSize: 20; color: theme.orange }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.Window.window.goIR()
                }
            }

            // Section header
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                Layout.topMargin: 8; Layout.bottomMargin: 6
                Text { text: "THIẾT BỊ TÌM THẤY"; font.pixelSize: 11; color: theme.t3; font.letterSpacing: 0.5 }
                Item { Layout.fillWidth: true }
                Button {
                    id: scanBtn
                    text: "Quét lại"
                    font.pixelSize: 11
                    flat: true
                    onClicked: scanner.startScan()
                    contentItem: Text { text: scanBtn.text; color: theme.blue; font.pixelSize: 11 }
                    background: Item {}
                }
            }

            // Device list
            ListView {
                id: deviceList
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                height: contentHeight
                spacing: 6
                interactive: false
                model: scanner.devices

                delegate: Rectangle {
                    width: deviceList.width
                    height: 76
                    radius: 14
                    color: theme.c1
                    border.color: theme.c2
                    border.width: 0.5

                    // Protocol color bar
                    Rectangle {
                        width: 3; height: parent.height - 16
                        anchors { left: parent.left; leftMargin: 0; verticalCenter: parent.verticalCenter }
                        radius: 3
                        color: modelData.hasCast && modelData.hasDial ? theme.blue
                             : modelData.hasCast ? theme.blue : theme.purple
                    }

                    RowLayout {
                        anchors { fill: parent; margins: 12; leftMargin: 16 }
                        spacing: 10

                        Rectangle {
                            width: 40; height: 40; radius: 10; color: theme.c2
                            Text { anchors.centerIn: parent; text: "📺"; font.pixelSize: 20 }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text { text: modelData.name; font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx }
                            Text { text: modelData.ip + " · Signal: " + modelData.signal + "/3"; font.pixelSize: 10; color: theme.t3 }
                            RowLayout {
                                spacing: 5
                                Rectangle {
                                    visible: modelData.hasCast
                                    radius: 20; color: Qt.rgba(0.04,0.52,1,0.18)
                                    width: cl.width+10; height: 17
                                    Text { id: cl; anchors.centerIn: parent; text: "Cast"; font.pixelSize: 9; color: theme.blue }
                                }
                                Rectangle {
                                    visible: modelData.hasDial
                                    radius: 20; color: Qt.rgba(0.75,0.35,0.95,0.18)
                                    width: dl.width+10; height: 17
                                    Text { id: dl; anchors.centerIn: parent; text: "DIAL"; font.pixelSize: 9; color: theme.purple }
                                }
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.Window.window.goConnecting(
                                modelData.name, modelData.ip,
                                modelData.hasCast, modelData.hasDial)
                        }
                    }
                }
            }

            // Manual IP
            Text {
                Layout.leftMargin: 16; Layout.topMargin: 14; Layout.bottomMargin: 6
                text: "THÊM THỦ CÔNG"; font.pixelSize: 11; color: theme.t3; font.letterSpacing: 0.5
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16; Layout.rightMargin: 16
                Layout.bottomMargin: 24
                spacing: 8
                TextField {
                    id: manualIp
                    Layout.fillWidth: true
                    placeholderText: "192.168.1.xxx"
                    color: theme.tx
                    placeholderTextColor: theme.t3
                    font.pixelSize: 12
                    background: Rectangle { radius: 10; color: theme.c1; border.color: theme.c2; border.width: 0.5 }
                    leftPadding: 12; rightPadding: 12
                    height: 40
                }
                Button {
                    text: "Kết nối"; height: 40
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { radius: 10; color: theme.blue }
                    onClicked: {
                        if (manualIp.text.length > 0)
                            root.Window.window.goConnecting(
                                "TV (" + manualIp.text + ")",
                                manualIp.text, true, true)
                    }
                }
            }
        }
    }
}
