#pragma once
#include <QObject>
#include <QList>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkAccessManager>
#include "Device.h"

// DeviceScanner dò tìm TV trong mạng LAN qua:
//   1. mDNS / Bonjour  → phát hiện Cast
//   2. SSDP / UPnP     → phát hiện DIAL
class DeviceScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(QVariantList devices READ devicesVariant NOTIFY devicesChanged)

public:
    explicit DeviceScanner(QObject *parent = nullptr);
    ~DeviceScanner();

    bool scanning() const { return m_scanning; }
    QVariantList devicesVariant() const;

public slots:
    void startScan();
    void stopScan();

signals:
    void scanningChanged();
    void devicesChanged();
    void deviceFound(const Device &device);

private slots:
    void onSsdpResponse();
    void onScanTimeout();

private:
    void sendSsdpDiscover();
    void sendMdnsQuery();
    void probeDevice(const QString &ip, int castPort, int dialPort);

    QUdpSocket          *m_udpSocket  = nullptr;
    QTimer              *m_scanTimer  = nullptr;
    QNetworkAccessManager *m_nam      = nullptr;
    QList<Device>        m_devices;
    bool                 m_scanning   = false;

    // SSDP multicast
    static constexpr auto SSDP_ADDR = "239.255.255.250";
    static constexpr int  SSDP_PORT = 1900;

    // Cast mDNS port
    static constexpr int  CAST_PORT  = 8008;
    static constexpr int  DIAL_PORT  = 56790;
};
