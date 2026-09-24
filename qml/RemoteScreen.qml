import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // State local
    property bool   playing:  true
    property int    volume:   65
    property int    rsMode:   0   // 0=off 1=loop1 2=loopAll 3=shuffle
    property int    speedIdx: 3   // index vào speeds[]
    property int    qualIdx:  0
    property bool   liked:    false

    readonly property var speeds:   [0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0]
    readonly property var qualities:["Auto", "144p", "360p", "480p", "720p", "1080p"]
    readonly property var rsModes:  ["Lặp/Xáo", "Lặp 1", "Lặp tất cả", "Xáo trộn"]
    readonly property var rsIcons:  ["↻", "↺", "↻", "⇄"]

    Flickable {
        anchors.fill: parent
        contentHeight: mainCol.height + 40
        clip: true

        ColumnLayout {
            id: mainCol
            width: parent.width
            spacing: 0

            // ── Header ────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; height: 58
                color: theme.c1
                border.color: theme.c2; border.width: 0.5

                RowLayout {
                    anchors { fill: parent; margins: 12 }
                    spacing: 10

                    Rectangle { width: 36; height: 36; radius: 9; color: theme.c2
                        Text { anchors.centerIn: parent; text: "📺"; font.pixelSize: 18 } }

                    ColumnLayout {
                        spacing: 2
                        Text { text: remote.deviceName; font.pixelSize: 13; font.weight: Font.Medium; color: theme.tx }
                        RowLayout {
                            spacing: 5
                            Rectangle { width: 6; height: 6; radius: 3; color: theme.green }
                            Text { text: "Đã kết nối"; font.pixelSize: 10; color: theme.green }
                            Rectangle {
                                visible: remote.castReady
                                radius: 20; color: Qt.rgba(0.04,0.52,1,0.2); width: cl.width+10; height: 16
                                Text { id: cl; anchors.centerIn: parent; text: "Cast"; font.pixelSize: 9; color: theme.blue }
                            }
                            Rectangle {
                                visible: remote.dialReady
                                radius: 20; color: Qt.rgba(0.75,0.35,0.95,0.2); width: dl.width+10; height: 16
                                Text { id: dl; anchors.centerIn: parent; text: "DIAL"; font.pixelSize: 9; color: theme.purple }
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    // Real-time indicator
                    Rectangle {
                        radius: 20; color: Qt.rgba(0.19,0.82,0.35,0.1)
                        border.color: Qt.rgba(0.19,0.82,0.35,0.25); border.width: 0.5
                        width: rtRow.width + 16; height: 22
                        RowLayout {
                            id: rtRow; anchors.centerIn: parent; spacing: 5
                            Rectangle {
                                width: 7; height: 7; radius: 3.5; color: theme.green
                                SequentialAnimation on opacity { running: true; loops: Animation.Infinite
                                    NumberAnimation { to: 0.3; duration: 800 }
                                    NumberAnimation { to: 1.0; duration: 800 }
                                }
                            }
                            Text { text: "Real-time"; font.pixelSize: 9; color: theme.green }
                        }
                    }

                    RoundButton {
                        width: 32; height: 32; radius: 16
                        background: Rectangle { radius: parent.radius; color: "transparent" }
                        contentItem: Text { text: "×"; color: theme.t3; font.pixelSize: 18; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                        onClicked: { remote.disconnectAll(); root.Window.window.goConnect() }
                    }
                }
            }

            // ── Now Playing ───────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 10
                height: 52; radius: 12; color: theme.c1
                border.color: theme.c2; border.width: 0.5

                RowLayout {
                    anchors { fill: parent; margins: 10 } spacing: 10
                    Rectangle { width: 46; height: 30; radius: 6; color: "#000"
                        Text { anchors.centerIn: parent; text: "▶"; color: theme.red; font.pixelSize: 16 } }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 1
                        Text { text: "Lofi Hip Hop Radio — beats to relax/study"; font.pixelSize: 11; font.weight: Font.Medium; color: theme.tx; elide: Text.ElideRight; Layout.fillWidth: true }
                        Text { text: "Lofi Girl · YouTube"; font.pixelSize: 10; color: theme.t3 }
                    }
                    Rectangle { radius: 4; color: theme.red; width: liveTxt.width+10; height: 18
                        Text { id: liveTxt; anchors.centerIn: parent; text: "LIVE"; font.pixelSize: 9; color: "white"; font.weight: Font.Bold } }
                }
            }

            // ── Progress ──────────────────────────────────────
            ColumnLayout {
                Layout.fillWidth: true; Layout.leftMargin: 14; Layout.rightMargin: 14; spacing: 4
                Rectangle {
                    Layout.fillWidth: true; height: 3; radius: 2; color: theme.c2
                    Rectangle { id: progFill; width: parent.width * 0.35; height: 3; radius: 2; color: theme.red }
                    MouseArea { anchors.fill: parent
                        onClicked: { var p = mouseX/parent.width; progFill.width = parent.width*p; remote.seekTo(p*100) } }
                }
                RowLayout {
                    Text { text: "1:24:08"; font.pixelSize: 9; color: theme.t3 }
                    Item { Layout.fillWidth: true }
                    Text { text: "LIVE"; font.pixelSize: 9; color: theme.t3 }
                }
            }

            // ── Search ────────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 8; spacing: 8
                Rectangle {
                    Layout.fillWidth: true; height: 38; radius: 10
                    color: theme.c1; border.color: theme.c2; border.width: 0.5
                    RowLayout { anchors { fill: parent; margins: 11 } spacing: 7
                        Text { text: "🔍"; font.pixelSize: 14 }
                        Text { text: "Tìm kiếm trên TV..."; font.pixelSize: 11; color: theme.t3 }
                    }
                    MouseArea { anchors.fill: parent; onClicked: remote.searchOnTV("") }
                }
                Rectangle { width: 38; height: 38; radius: 10; color: theme.c1; border.color: theme.c2; border.width: 0.5
                    Text { anchors.centerIn: parent; text: "🎙"; font.pixelSize: 16 }
                    MouseArea { anchors.fill: parent; onClicked: remote.searchOnTV("voice") }
                }
            }

            Divider {}

            // ── Playback label ────────────────────────────────
            SectionLabel { text: "PHÁT LẠI · CAST" }

            // Row 1: Prev / Play / Next / Loop
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 5
                RemBtn { lbl: "⏮"; sub: "Trước";   pTag: "Cast"; onTapped: remote.skipPrev() }
                RemBtn { id: playBtn; lbl: root.playing ? "⏸" : "▶"; sub: root.playing ? "Dừng" : "Phát"; pTag: "Cast"; accent: true
                    onTapped: { root.playing = !root.playing; remote.togglePlay() } }
                RemBtn { lbl: "⏭"; sub: "Tiếp";    pTag: "Cast"; onTapped: remote.skipNext() }
                RemBtn {
                    lbl: root.rsIcons[root.rsMode]; sub: root.rsModes[root.rsMode]; pTag: "Cast"
                    active: root.rsMode > 0
                    onTapped: {
                        root.rsMode = (root.rsMode + 1) % 4
                        var modes = ["REPEAT_OFF","REPEAT_SINGLE","REPEAT_ALL","SHUFFLE"]
                        remote.setRepeat(modes[root.rsMode])
                    }
                }
            }

            // Row 2: -10s / +10s / Speed / Quality
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 5
                RemBtn { lbl: "⏪"; sub: "-10s"; pTag: "Cast"; onTapped: remote.seekTo(-10) }
                RemBtn { lbl: "⏩"; sub: "+10s"; pTag: "Cast"; onTapped: remote.seekTo(10) }
                RemBtn {
                    lbl: root.speeds[root.speedIdx] + "x"; sub: "Tốc độ"; pTag: "Cast"
                    onTapped: { root.speedIdx = (root.speedIdx+1) % root.speeds.length; remote.setSpeed(root.speeds[root.speedIdx]) }
                }
                RemBtn {
                    lbl: "⚙"; sub: root.qualities[root.qualIdx]; pTag: "Cast"
                    onTapped: { root.qualIdx = (root.qualIdx+1) % root.qualities.length; remote.setQuality(root.qualities[root.qualIdx]) }
                }
            }

            // Volume
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 0; spacing: 10
                Text { text: "🔊"; font.pixelSize: 15; color: theme.t3 }
                Slider {
                    id: volSlider; Layout.fillWidth: true; from: 0; to: 100; value: root.volume
                    onValueChanged: { root.volume = value; remote.setVolume(value) }
                    background: Rectangle {
                        x: volSlider.leftPadding; y: volSlider.topPadding + volSlider.availableHeight/2 - height/2
                        width: volSlider.availableWidth; height: 3; radius: 2; color: theme.c2
                        Rectangle { width: volSlider.visualPosition * parent.width; height: 3; radius: 2; color: theme.tx }
                    }
                    handle: Rectangle {
                        x: volSlider.leftPadding + volSlider.visualPosition * volSlider.availableWidth - width/2
                        y: volSlider.topPadding + volSlider.availableHeight/2 - height/2
                        width: 16; height: 16; radius: 8; color: theme.tx
                    }
                }
                Text { text: Math.round(root.volume); font.pixelSize: 11; color: theme.t3; Layout.minimumWidth: 24 }
            }

            Divider {}

            // ── D-Pad ─────────────────────────────────────────
            SectionLabel { text: "ĐIỀU HƯỚNG · DIAL" }

            DPad {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 4
                onUpPressed:    remote.navUp()
                onDownPressed:  remote.navDown()
                onLeftPressed:  remote.navLeft()
                onRightPressed: remote.navRight()
                onOkPressed:    remote.navOk()
            }

            // Nav row: Home / Back / Phụ đề / Thích
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 14; Layout.topMargin: 8; spacing: 5
                NavBtn { lbl: "🏠"; sub: "Home";    onTapped: remote.navHome() }
                NavBtn { lbl: "←"; sub: "Quay lại"; onTapped: remote.navBack() }
                NavBtn { lbl: "CC"; sub: "Phụ đề"; onTapped: remote.setSubtitles(true) }
                NavBtn {
                    id: likeBtn; lbl: root.liked ? "♥" : "♡"; sub: root.liked ? "Đã thích" : "Thích"
                    active: root.liked
                    onTapped: root.liked = !root.liked
                }
            }

            // ABC button
            Item { Layout.fillWidth: true; height: 10 }
            Rectangle {
                height: 42; width: 140
                radius: 10; color: theme.red
                Text { anchors.centerIn: parent; text: "⌨  ABC — Bàn phím"; color: "white"; font.pixelSize: 12; font.weight: Font.Medium }
                MouseArea { anchors.fill: parent; onClicked: root.Window.window.goConnect() }
            }
            Item { height: 24 }
        }
    }
}

// ── Inline components ─────────────────────────────────────────
