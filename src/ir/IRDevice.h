#pragma once
#include <QString>
#include <QMap>
#include <QVector>

// Các lệnh chuẩn của remote TV
enum class IRCommand {
    // Nguồn
    Power, PowerOn, PowerOff,
    // Âm lượng
    VolumeUp, VolumeDown, Mute,
    // Kênh
    ChannelUp, ChannelDown,
    // Điều hướng
    Up, Down, Left, Right, Ok,
    Back, Home, Menu,
    // Input
    InputSource, HDMI1, HDMI2, HDMI3,
    // Playback
    Play, Pause, Stop, FastForward, Rewind,
    // Số
    Num0,Num1,Num2,Num3,Num4,
    Num5,Num6,Num7,Num8,Num9,
    // Màu sắc
    Red, Green, Yellow, Blue,
    // Khác
    Info, Guide, Subtitle, Settings,
    Netflix, YouTube, Prime
};

// Một mã IR (NEC / PRONTO hex / RAW)
struct IRCode {
    int         frequency  = 38000; // Hz — thường 38kHz
    QVector<int> pattern;           // RAW pattern (microseconds, on/off xen kẽ)
    QString     prontoHex;          // PRONTO hex string (backup)
    bool        learned    = false; // true = học từ remote thật
};

// Một thiết bị IR (TV hãng + model)
struct IRDevice {
    QString id;           // "samsung-ue55"
    QString brand;        // "Samsung"
    QString model;        // "UE55 Series"
    QString protocol;     // "NEC", "RC5", "RC6", "SONY", "SAMSUNG"

    QMap<IRCommand, IRCode> codes;

    bool hasCommand(IRCommand cmd) const {
        return codes.contains(cmd);
    }
};

// Chuyển enum → tên string
inline QString commandName(IRCommand cmd) {
    switch (cmd) {
    case IRCommand::Power:        return "Power";
    case IRCommand::VolumeUp:     return "Volume Up";
    case IRCommand::VolumeDown:   return "Volume Down";
    case IRCommand::Mute:         return "Mute";
    case IRCommand::ChannelUp:    return "Channel Up";
    case IRCommand::ChannelDown:  return "Channel Down";
    case IRCommand::Up:           return "Up";
    case IRCommand::Down:         return "Down";
    case IRCommand::Left:         return "Left";
    case IRCommand::Right:        return "Right";
    case IRCommand::Ok:           return "OK";
    case IRCommand::Back:         return "Back";
    case IRCommand::Home:         return "Home";
    case IRCommand::Menu:         return "Menu";
    case IRCommand::InputSource:  return "Input Source";
    case IRCommand::HDMI1:        return "HDMI 1";
    case IRCommand::HDMI2:        return "HDMI 2";
    case IRCommand::HDMI3:        return "HDMI 3";
    case IRCommand::Play:         return "Play";
    case IRCommand::Pause:        return "Pause";
    case IRCommand::Stop:         return "Stop";
    case IRCommand::Netflix:      return "Netflix";
    case IRCommand::YouTube:      return "YouTube";
    default:                      return "Unknown";
    }
}
