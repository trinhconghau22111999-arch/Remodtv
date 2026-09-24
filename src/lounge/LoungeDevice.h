#pragma once
#include <QString>

// Thiết bị đang chạy YouTube trên trình duyệt
struct LoungeDevice {
    QString id;           // unique id từ mDNS
    QString name;         // "Chrome on MacBook Pro"
    QString ip;
    int     port = 8008;
    QString loungeToken;  // token để gửi lệnh
    QString screenId;     // YouTube screen ID
    QString deviceType;   // "CHROME", "LAPTOP", "DESKTOP"
    QString browser;      // "Chrome", "Firefox", "Edge"
    QString os;           // "macOS", "Windows", "Linux"
    bool    youtubeOpen = false;

    QString displayName() const {
        if (!browser.isEmpty() && !name.isEmpty())
            return browser + " · " + name;
        return name.isEmpty() ? ip : name;
    }
};
