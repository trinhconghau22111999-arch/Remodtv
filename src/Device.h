#pragma once
#include <QString>

enum class Protocol {
    None  = 0,
    Cast  = 1 << 0,
    Dial  = 1 << 1,
    Both  = Cast | Dial
};
Q_DECLARE_FLAGS(Protocols, Protocol)
Q_DECLARE_OPERATORS_FOR_FLAGS(Protocols)

struct Device {
    QString id;           // unique id
    QString name;         // "Sony Bravia XR"
    QString ip;           // "192.168.1.105"
    int     port = 8008;  // Cast default port
    int     dialPort = 56790;
    Protocols protocols = Protocol::None;
    int     signalStrength = 0; // 0-3

    bool supportsCast() const { return protocols.testFlag(Protocol::Cast); }
    bool supportsDial() const { return protocols.testFlag(Protocol::Dial); }

    QString protocolLabel() const {
        if (supportsCast() && supportsDial()) return "Cast + DIAL";
        if (supportsCast()) return "Cast";
        if (supportsDial()) return "DIAL";
        return "Unknown";
    }
};
