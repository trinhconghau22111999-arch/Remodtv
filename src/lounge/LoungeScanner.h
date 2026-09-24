#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QList>
#include "LoungeDevice.h"

// LoungeScanner — dò tìm thiết bị đang mở YouTube qua:
//   1. mDNS (_googlecast._tcp.local) — Chromecast + Chrome browser
//   2. SSDP — một số thiết bị Cast
//   3. HTTP probe — thử kết nối trực tiếp các IP trong mạng
class LoungeScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantList devices READ devicesVariant NOTIFY devicesChanged)

public:
    explicit LoungeScanner(QObject *parent = nullptr);

    bool         scanning()       const { return m_scanning; }
    QVariantList devicesVariant() const;

public slots:
    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();

signals:
    void scanningChanged();
    void devicesChanged();
    void deviceFound(const LoungeDevice &dev);

private slots:
    void onMdnsResponse();
    void onScanTimeout();

private:
    void sendMdnsQuery();
    void probeCastPort(const QString &ip);
    void parseMdnsResponse(const QByteArray &data, const QString &senderIp);

    QUdpSocket            *m_udp     = nullptr;
    QTimer                *m_timer   = nullptr;
    QNetworkAccessManager *m_nam     = nullptr;
    QList<LoungeDevice>    m_devices;
    bool                   m_scanning = false;

    // mDNS multicast
    static constexpr auto MDNS_ADDR = "224.0.0.251";
    static constexpr int  MDNS_PORT = 5353;
    static constexpr int  CAST_PORT = 8008; // Cast discovery port
};
