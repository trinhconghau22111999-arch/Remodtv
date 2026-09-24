import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    width: 390; height: 844
    visible: true
    title: "TV Remote"
    color: "#111111"

    // Màu chủ đề dark
    QtObject {
        id: theme
        readonly property color bg:      "#111111"
        readonly property color c1:      "#1c1c1e"
        readonly property color c2:      "#2c2c2e"
        readonly property color c3:      "#3a3a3c"
        readonly property color tx:      "#f5f5f5"
        readonly property color t2:      "#aaaaaa"
        readonly property color t3:      "#666666"
        readonly property color red:     "#ff3b30"
        readonly property color blue:    "#0a84ff"
        readonly property color green:   "#30d158"
        readonly property color purple:  "#bf5af2"
        readonly property color orange:  "#ff9f0a"
        readonly property int   radius:  12
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: connectScreen
    }

    Component { id: connectScreen;   ConnectScreen   {} }
    Component { id: connectingScreen; ConnectingScreen {} }
    Component { id: remoteScreen;    RemoteScreen    {} }

    // Hàm chuyển màn hình
    function goConnect()    { stack.replace(connectScreen) }
    function goConnecting(name, ip, hasCast, hasDial) {
        stack.replace(connectingScreen, {
            "deviceName": name, "ip": ip,
            "hasCast": hasCast, "hasDial": hasDial
        })
    }
    function goRemote() { stack.replace(remoteScreen) }
}
