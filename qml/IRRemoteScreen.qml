import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    Connections {
        target: irCtrl
        function onCommandResult(success, cmd) {
            feedbackTxt.text    = success ? "✓ " + cmd : "✗ " + cmd
            feedbackTxt.color   = success ? theme.green : theme.red
            feedbackAnim.restart()
        }
        function onLearningComplete(cmd) {
            learnStatus.text  = "✓ Đã học: " + cmd
            learnStatus.color = theme.green
        }
    }

    Flickable {
        anchors.fill: parent
        contentHeight: col.height + 32
        clip: true

        ColumnLayout {
            id: col
            width: parent.width
            spacing: 0

            // ── Header ────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; height: 56
                color: theme.c1; border.color: theme.c2; border.width: 0.5

                RowLayout {
                    anchors { fill: parent; margins: 12 } spacing: 10
                    RoundButton {
                        width: 30; height: 30; radius: 15
                        background: Rectangle { radius: parent.radius; color: "transparent" }
                        contentItem: Text { text: "‹"; color: theme.tx; font.pixelSize: 20; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        onClicked: root.Window.window.goBack()
                    }
                    Rectangle { width: 34; height: 34; radius: 9; color: "#1a1a1a"
                        Text { anchors.centerIn: parent; text: "📺"; font.pixelSize: 18 } }
                    ColumnLayout {
                        spacing: 1
                        Text { text: "IR Remote"; font.pixelSize: 14; font.weight: Font.Medium; color: theme.tx }
                        Text {
                            text: irCtrl.irAvailable ? "IR Blaster sẵn sàng" : "Không có IR Blaster"
                            font.pixelSize: 10
                            color: irCtrl.irAvailable ? theme.green : theme.red
                        }
                    }
                    Item { Layout.fillWidth: true }
                    // Feedback
                    Text {
                        id: feedbackTxt; text: ""; font.pixelSize: 11; color: theme.green
                        opacity: 0
                        SequentialAnimation {
                            id: feedbackAnim
                            NumberAnimation { target: feedbackTxt; property: "opacity"; to: 1; duration: 100 }
                            PauseAnimation  { duration: 1200 }
                            NumberAnimation { target: feedbackTxt; property: "opacity"; to: 0; duration: 300 }
                        }
                    }
                }
            }

            // ── Chọn hãng TV ──────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 12
                height: 44; radius: 10
                color: theme.c1; border.color: theme.c2; border.width: 0.5
                visible: !irCtrl.deviceReady

                RowLayout {
                    anchors { fill: parent; margins: 10 } spacing: 8
                    Text { text: "📺"; font.pixelSize: 16 }
                    Text { text: "Chọn hãng TV:"; font.pixelSize: 12; color: theme.t2 }
                    ComboBox {
                        Layout.fillWidth: true
                        model: irCtrl.brands
                        font.pixelSize: 12
                        onCurrentTextChanged: irCtrl.brand = currentText
                        background: Rectangle { color: theme.c2; radius: 8 }
                        contentItem: Text { text: parent.displayText; color: theme.tx; font.pixelSize: 12; leftPadding: 10; verticalAlignment: Text.AlignVCenter }
                    }
                }
            }

            // Hãng đã chọn
            Rectangle {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 10
                height: 40; radius: 10
                color: Qt.rgba(0.04,0.52,1,0.1); border.color: Qt.rgba(0.04,0.52,1,0.25); border.width: 0.5
                visible: irCtrl.deviceReady

                RowLayout {
                    anchors { fill: parent; margins: 10 } spacing: 8
                    Text { text: "📺 " + irCtrl.brand + " · IR sẵn sàng"; font.pixelSize: 12; color: theme.blue }
                    Item { Layout.fillWidth: true }
                    Text {
                        text: "Đổi"; color: theme.blue; font.pixelSize: 11
                        MouseArea { anchors.fill: parent; onClicked: irCtrl.brand = "" }
                    }
                }
            }

            // ── Nguồn — nút to nhất ───────────────────────────
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 14
                width: 72; height: 72; radius: 36
                color: powerArea.pressed ? Qt.darker(theme.red, 1.3) : theme.red
                Behavior on color { ColorAnimation { duration: 100 } }
                // Icon nguồn vẽ bằng vector — không phụ thuộc font emoji của máy
                Item {
                    anchors.centerIn: parent
                    width: 26; height: 26
                    Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        color: "transparent"
                        border.color: "white"; border.width: 3
                    }
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top; anchors.topMargin: -3
                        width: 8; height: 6
                        color: powerArea.pressed ? Qt.darker(theme.red, 1.3) : theme.red
                    }
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top; anchors.topMargin: -3
                        width: 3; height: 13; radius: 1.5
                        color: "white"
                    }
                }
                MouseArea { id: powerArea; anchors.fill: parent; onClicked: irCtrl.power() }
                scale: powerArea.pressed ? 0.92 : 1.0
                Behavior on scale { NumberAnimation { duration: 80 } }
            }
            Text { Layout.alignment: Qt.AlignHCenter; Layout.topMargin: 4
                text: "Nguồn"; font.pixelSize: 10; color: theme.t2 }

            // ── Âm lượng + Kênh ──────────────────────────────
            Item { Layout.fillWidth: true; height: 14 }
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; spacing: 10

                // Âm lượng
                Rectangle {
                    Layout.fillWidth: true; height: 100; radius: 12
                    color: theme.c1; border.color: theme.c2; border.width: 0.5
                    ColumnLayout {
                        anchors { fill: parent; margins: 8 } spacing: 5
                        Text { Layout.alignment: Qt.AlignHCenter; text: "🔊 Âm lượng"; font.pixelSize: 10; color: theme.t3 }
                        IRBtn { Layout.fillWidth: true; Layout.fillHeight: true; lbl: "＋"; onTapped: irCtrl.volumeUp() }
                        IRBtn { Layout.fillWidth: true; Layout.fillHeight: true; lbl: "✕"; onTapped: irCtrl.mute() }
                        IRBtn { Layout.fillWidth: true; Layout.fillHeight: true; lbl: "－"; onTapped: irCtrl.volumeDown() }
                    }
                }

                // Kênh
                Rectangle {
                    Layout.fillWidth: true; height: 100; radius: 12
                    color: theme.c1; border.color: theme.c2; border.width: 0.5
                    ColumnLayout {
                        anchors { fill: parent; margins: 8 } spacing: 5
                        Text { Layout.alignment: Qt.AlignHCenter; text: "📺 Kênh"; font.pixelSize: 10; color: theme.t3 }
                        IRBtn { Layout.fillWidth: true; Layout.fillHeight: true; lbl: "∧"; onTapped: irCtrl.channelUp() }
                        IRBtn { Layout.fillWidth: true; Layout.fillHeight: true; lbl: "IN"; onTapped: irCtrl.inputSource() }
                        IRBtn { Layout.fillWidth: true; Layout.fillHeight: true; lbl: "∨"; onTapped: irCtrl.channelDown() }
                    }
                }
            }

            // ── D-Pad IR ──────────────────────────────────────
            Item { height: 8 }
            DPad {
                Layout.alignment: Qt.AlignHCenter
                onUpPressed:    irCtrl.up()
                onDownPressed:  irCtrl.down()
                onLeftPressed:  irCtrl.left()
                onRightPressed: irCtrl.right()
                onOkPressed:    irCtrl.ok()
            }

            // Nav: Home + Back
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 8; spacing: 5
                IRBtnWide { lbl: "🏠"; sub: "Home"; onTapped: irCtrl.home() }
                IRBtnWide { lbl: "←"; sub: "Back"; onTapped: irCtrl.back() }
            }

            // ── Input HDMI ────────────────────────────────────
            SectionLbl { text: "HDMI" }
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 5
                IRBtnWide { lbl: "1"; sub: "HDMI 1"; onTapped: irCtrl.hdmi(1) }
                IRBtnWide { lbl: "2"; sub: "HDMI 2"; onTapped: irCtrl.hdmi(2) }
                IRBtnWide { lbl: "3"; sub: "HDMI 3"; onTapped: irCtrl.hdmi(3) }
            }

            // ── Học mã ────────────────────────────────────────
            SectionLbl { text: "HỌC MÃ TỪ REMOTE THẬT" }
            Rectangle {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0
                height: learnCol.height + 16; radius: 12
                color: theme.c1; border.color: theme.c2; border.width: 0.5

                ColumnLayout {
                    id: learnCol
                    width: parent.width - 16
                    anchors.centerIn: parent
                    spacing: 8

                    Text { id: learnStatus; text: "Chọn nút cần học → hướng remote thật vào đỉnh điện thoại"
                        font.pixelSize: 10; color: theme.t2; wrapMode: Text.Wrap; Layout.fillWidth: true }

                    // Trạng thái đang học
                    Rectangle {
                        Layout.fillWidth: true; height: 36; radius: 8
                        color: Qt.rgba(0.04,0.52,1,0.1); border.color: Qt.rgba(0.04,0.52,1,0.2); border.width: 0.5
                        visible: irCtrl.learning
                        RowLayout { anchors.centerIn: parent; spacing: 8
                            Rectangle { width: 8; height: 8; radius: 4; color: theme.red
                                SequentialAnimation on opacity { running: irCtrl.learning; loops: Animation.Infinite
                                    NumberAnimation { to: 0.2; duration: 500 }
                                    NumberAnimation { to: 1.0; duration: 500 }
                                }
                            }
                            Text { text: "Đang chờ tín hiệu... hướng remote vào máy rồi bấm nút cần học"
                                font.pixelSize: 10; color: theme.blue }
                        }
                    }

                    GridLayout { columns: 3; columnSpacing: 5; rowSpacing: 5; Layout.fillWidth: true
                        Repeater {
                            model: ["Power","Volume Up","Volume Down","Mute","Channel Up","Channel Down","Up","Down","Left","Right","OK","Back"]
                            delegate: Rectangle {
                                Layout.fillWidth: true; height: 32; radius: 8
                                color: theme.c2; border.color: theme.c3; border.width: 0.5
                                Text { anchors.centerIn: parent; text: modelData; font.pixelSize: 9; color: theme.t2 }
                                MouseArea { anchors.fill: parent
                                    onClicked: { irCtrl.startLearning(modelData); learnStatus.text = "🎯 Học: " + modelData + " — bấm nút trên remote thật"; learnStatus.color = theme.blue }
                                }
                            }
                        }
                    }

                    RowLayout { Layout.fillWidth: true; spacing: 8
                        Button {
                            Layout.fillWidth: true; height: 36; text: "Dừng học"
                            visible: irCtrl.learning
                            onClicked: irCtrl.stopLearning()
                            contentItem: Text { text: parent.text; color: theme.red; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                            background: Rectangle { radius: 8; color: theme.c2; border.color: theme.red; border.width: 0.5 }
                        }
                        Button {
                            Layout.fillWidth: true; height: 36; text: "Lưu mã đã học"
                            onClicked: { irCtrl.saveLearned(); learnStatus.text = "✓ Đã lưu!"; learnStatus.color = theme.green }
                            contentItem: Text { text: parent.text; color: theme.blue; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                            background: Rectangle { radius: 8; color: theme.c2; border.color: theme.blue; border.width: 0.5 }
                        }
                    }
                }
            }
            Item { height: 24 }
        }
    }
}

// ── Inline components ─────────────────────────────────────────
