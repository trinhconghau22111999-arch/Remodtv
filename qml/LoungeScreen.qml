import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Local state
    property bool   playing:  true
    property int    volume:   80
    property int    rsMode:   0
    property int    speedIdx: 3
    property int    qualIdx:  0
    property bool   liked:    false

    readonly property var speeds:   [0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0]
    readonly property var qualities:["Auto","144p","360p","480p","720p","1080p"]
    readonly property var rsModes:  ["Lặp/Xáo","Lặp 1","Lặp tất cả","Xáo trộn"]
    readonly property var rsIcons:  ["↻","↺","↻","⇄"]

    Connections {
        target: lounge
        function onStatusChanged() {
            root.playing = lounge.playing
            root.volume  = lounge.volume
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

                    // Laptop icon
                    Rectangle {
                        width: 36; height: 36; radius: 9; color: theme.c2
                        Text { anchors.centerIn: parent; text: "💻"; font.pixelSize: 18 }
                    }

                    ColumnLayout {
                        spacing: 2
                        Text {
                            text: lounge.connected ? lounge.deviceName : "YouTube · Laptop"
                            font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx
                        }
                        RowLayout {
                            spacing: 5
                            Rectangle { width: 6; height: 6; radius: 3; color: lounge.connected ? theme.green : theme.t3 }
                            Text {
                                text: lounge.connected ? "Đã kết nối · Lounge API" : "Chưa kết nối"
                                font.pixelSize: 10
                                color: lounge.connected ? theme.green : theme.t3
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    // Real-time indicator
                    Rectangle {
                        visible: lounge.connected
                        radius: 20; color: Qt.rgba(0.19,0.82,0.35,0.1)
                        border.color: Qt.rgba(0.19,0.82,0.35,0.25); border.width: 0.5
                        width: rtRow.width + 16; height: 22
                        RowLayout {
                            id: rtRow; anchors.centerIn: parent; spacing: 5
                            Rectangle {
                                width: 6; height: 6; radius: 3; color: theme.green
                                SequentialAnimation on opacity { running: lounge.connected; loops: Animation.Infinite
                                    NumberAnimation { to: 0.3; duration: 700 }
                                    NumberAnimation { to: 1.0; duration: 700 }
                                }
                            }
                            Text { text: "Live"; font.pixelSize: 9; color: theme.green }
                        }
                    }

                    // Disconnect
                    RoundButton {
                        width: 30; height: 30; radius: 15
                        visible: lounge.connected
                        background: Rectangle { radius: parent.radius; color: "transparent" }
                        contentItem: Text { text: "⏏"; color: theme.t3; font.pixelSize: 15; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        onClicked: lounge.disconnect()
                    }
                }
            }

            // ── Chọn laptop nếu chưa kết nối ─────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 12
                height: scanCol.height + 20; radius: 12
                color: theme.c1; border.color: theme.c2; border.width: 0.5
                visible: !lounge.connected

                ColumnLayout {
                    id: scanCol
                    width: parent.width - 24
                    anchors.centerIn: parent
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "LAPTOP TÌM THẤY"; font.pixelSize: 10; color: theme.t3; font.letterSpacing: 0.4 }
                        Item { Layout.fillWidth: true }
                        Text {
                            text: loungeScanner.scanning ? "Đang quét..." : "Quét lại"
                            font.pixelSize: 10; color: theme.blue
                            MouseArea { anchors.fill: parent; onClicked: loungeScanner.startScan() }
                        }
                    }

                    Repeater {
                        model: loungeScanner.devices
                        delegate: Rectangle {
                            width: scanCol.width; height: 54; radius: 10
                            color: theme.c2; border.color: theme.c3; border.width: 0.5

                            RowLayout {
                                anchors { fill: parent; margins: 10 } spacing: 10
                                Text { text: modelData.browser === "Chrome" ? "🌐" : modelData.browser === "Firefox" ? "🦊" : "🌐"; font.pixelSize: 20 }
                                ColumnLayout {
                                    Layout.fillWidth: true; spacing: 2
                                    Text { text: modelData.browser + " · " + modelData.name; font.pixelSize: 12; font.weight: Font.Medium; color: theme.tx }
                                    Text { text: modelData.ip + " · " + modelData.os; font.pixelSize: 10; color: theme.t3 }
                                }
                                Rectangle {
                                    radius: 20; width: ytTxt.width + 12; height: 18
                                    color: modelData.youtubeOpen ? Qt.rgba(1,0,0,.15) : theme.c3
                                    Text { id: ytTxt; anchors.centerIn: parent; text: modelData.youtubeOpen ? "YouTube mở" : "Không có YouTube"; font.pixelSize: 9; color: modelData.youtubeOpen ? theme.red : theme.t3 }
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (modelData.youtubeOpen)
                                        lounge.connectToDeviceById(modelData.id)
                                }
                            }
                        }
                    }

                    // Không tìm thấy
                    Rectangle {
                        visible: loungeScanner.devices.length === 0
                        Layout.fillWidth: true; height: 48; radius: 10; color: theme.c2
                        ColumnLayout {
                            anchors.centerIn: parent; spacing: 3
                            Text { Layout.alignment: Qt.AlignHCenter; text: "Không tìm thấy laptop"; font.pixelSize: 12; color: theme.t2 }
                            Text { Layout.alignment: Qt.AlignHCenter; text: "Mở YouTube trên Chrome/Edge/Firefox"; font.pixelSize: 10; color: theme.t3 }
                        }
                    }
                }
            }

            // ── Now playing ───────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: lounge.connected ? 10 : 6
                height: 52; radius: 12; color: theme.c1; border.color: theme.c2; border.width: 0.5
                visible: lounge.connected

                RowLayout {
                    anchors { fill: parent; margins: 10 } spacing: 10
                    Rectangle { width: 44; height: 30; radius: 6; color: "#000"
                        Text { anchors.centerIn: parent; text: "▶"; color: theme.red; font.pixelSize: 15 } }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 1
                        Text {
                            text: lounge.videoTitle.length > 0 ? lounge.videoTitle : "Đang phát..."
                            font.pixelSize: 11; font.weight: Font.Medium; color: theme.tx
                            elide: Text.ElideRight; Layout.fillWidth: true
                        }
                        Text { text: "YouTube · " + (lounge.deviceName.length > 0 ? lounge.deviceName : "Laptop"); font.pixelSize: 10; color: theme.t3 }
                    }
                    Rectangle { radius: 4; color: theme.red; width: 28; height: 16
                        Text { anchors.centerIn: parent; text: "YT"; font.pixelSize: 8; color: "white"; font.weight: Font.Bold } }
                }
            }

            // ── Progress ──────────────────────────────────────
            ColumnLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 4
                visible: lounge.connected
                Rectangle {
                    Layout.fillWidth: true; height: 3; radius: 2; color: theme.c2
                    Rectangle { id: pf; width: parent.width * 0.42; height: 3; radius: 2; color: theme.red }
                    MouseArea { anchors.fill: parent
                        onClicked: { var p = mouseX/parent.width; pf.width = parent.width*p; lounge.seekTo(p*300) }
                    }
                }
                RowLayout {
                    Text { text: "2:07"; font.pixelSize: 9; color: theme.t3 }
                    Item { Layout.fillWidth: true }
                    Text { text: "5:42"; font.pixelSize: 9; color: theme.t3 }
                }
            }

            // ── Search ────────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 6; spacing: 8
                visible: lounge.connected
                Rectangle {
                    Layout.fillWidth: true; height: 38; radius: 10
                    color: theme.c1; border.color: theme.c2; border.width: 0.5
                    RowLayout { anchors { fill: parent; margins: 11 } spacing: 7
                        Text { text: "🔍"; font.pixelSize: 14 }
                        Text { text: "Tìm & phát trên laptop..."; font.pixelSize: 11; color: theme.t3 }
                    }
                    MouseArea { anchors.fill: parent; onClicked: lounge.searchAndPlay("") }
                }
                Rectangle { width: 38; height: 38; radius: 10; color: theme.c1; border.color: theme.c2; border.width: 0.5
                    Text { anchors.centerIn: parent; text: "🎙"; font.pixelSize: 16 }
                }
            }

            // ── Divider ───────────────────────────────────────
            Rectangle { Layout.fillWidth: true; height: 0.5; color: theme.c2; Layout.topMargin: 6; visible: lounge.connected }

            // ── Playback ──────────────────────────────────────
            Text {
                Layout.leftMargin: 14; Layout.topMargin: 6; Layout.bottomMargin: 3
                text: "PHÁT LẠI · LOUNGE API"; font.pixelSize: 9; color: theme.t3
                font.letterSpacing: 0.4; visible: lounge.connected
            }

            // Row 1
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 5
                visible: lounge.connected
                LBtn { lbl:"⏮"; sub:"Trước";  onTapped: lounge.skipPrev() }
                LBtn { id: playB; lbl: root.playing ? "⏸" : "▶"; sub: root.playing ? "Dừng" : "Phát"; accent: true
                    onTapped: { root.playing = !root.playing; root.playing ? lounge.play() : lounge.pause() } }
                LBtn { lbl:"⏭"; sub:"Tiếp";   onTapped: lounge.skipNext() }
                LBtn {
                    lbl: root.rsIcons[root.rsMode]; sub: root.rsModes[root.rsMode]
                    active: root.rsMode > 0
                    onTapped: {
                        root.rsMode = (root.rsMode+1)%4
                        var m=["REPEAT_OFF","REPEAT_SINGLE","REPEAT_ALL","SHUFFLE"]
                        lounge.setRepeatMode(m[root.rsMode])
                    }
                }
            }

            // Row 2
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 5
                visible: lounge.connected
                LBtn { lbl:"⏪"; sub:"-10s"; onTapped: lounge.seekTo(-10) }
                LBtn { lbl:"⏩"; sub:"+10s"; onTapped: lounge.seekTo(10) }
                LBtn { lbl: root.speeds[root.speedIdx]+"x"; sub:"Tốc độ"
                    onTapped: { root.speedIdx=(root.speedIdx+1)%root.speeds.length; lounge.setPlaybackRate(root.speeds[root.speedIdx]) } }
                LBtn { lbl:"⛶"; sub: root.playing ? "Toàn màn" : "Thu nhỏ"
                    onTapped: lounge.setFullscreen(root.playing) }
            }

            // Volume
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 10
                visible: lounge.connected
                Text { text: "🔊"; font.pixelSize: 14; color: theme.t3 }
                Slider {
                    id: vs; Layout.fillWidth: true; from: 0; to: 100; value: root.volume
                    onValueChanged: { root.volume = value; lounge.setVolume(value) }
                    background: Rectangle {
                        x: vs.leftPadding; y: vs.topPadding + vs.availableHeight/2 - height/2
                        width: vs.availableWidth; height: 3; radius: 2; color: theme.c2
                        Rectangle { width: vs.visualPosition * parent.width; height: 3; radius: 2; color: theme.tx }
                    }
                    handle: Rectangle {
                        x: vs.leftPadding + vs.visualPosition * vs.availableWidth - width/2
                        y: vs.topPadding + vs.availableHeight/2 - height/2
                        width: 15; height: 15; radius: 7.5; color: theme.tx
                    }
                }
                Text { text: Math.round(root.volume); font.pixelSize: 10; color: theme.t3; Layout.minimumWidth: 22 }
            }

            // Extra: Chất lượng + Phụ đề + Thích
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 5
                visible: lounge.connected
                LBtn { lbl:"⚙"; sub: root.qualities[root.qualIdx]
                    onTapped: { root.qualIdx=(root.qualIdx+1)%root.qualities.length; lounge.setQuality(root.qualities[root.qualIdx]) } }
                LBtn { lbl:"CC"; sub:"Phụ đề"; onTapped: lounge.setSubtitles(true) }
                LBtn { id: likeB; lbl: root.liked ? "♥":"♡"; sub: root.liked ? "Đã thích":"Thích"; active: root.liked
                    onTapped: root.liked = !root.liked }
            }

            Item { height: 12 }
        }
    }
}

// ── Inline button component ───────────────────────────────────
