#include "LoungeScanner.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>

LoungeScanner::LoungeScanner(QObject *parent)
    : QObject(parent)
    , m_udp(new QUdpSocket(this))
    , m_timer(new QTimer(this))
    , m_nam(new QNetworkAccessManager(this))
{
    connect(m_udp, &QUdpSocket::readyRead, this, &LoungeScanner::onMdnsResponse);
    m_timer->setSingleShot(true);
    m_timer->setInterval(6000);
    connect(m_timer, &QTimer::timeout, this, &LoungeScanner::onScanTimeout);
}

QVariantList LoungeScanner::devicesVariant() const {
    QVariantList list;
    for (const auto &d : m_devices) {
        QVariantMap m;
        m["id"]          = d.id;
        m["name"]        = d.displayName();
        m["ip"]          = d.ip;
        m["browser"]     = d.browser;
        m["os"]          = d.os;
        m["deviceType"]  = d.deviceType;
        m["youtubeOpen"] = d.youtubeOpen;
        m["loungeToken"] = d.loungeToken;
        m["screenId"]    = d.screenId;
        list.append(m);
    }
    return list;
}

void LoungeScanner::startScan() {
    if (m_scanning) return;
    m_devices.clear();
    m_scanning = true;
    emit scanningChanged();
    emit devicesChanged();

    // Bind UDP multicast
    m_udp->bind(QHostAddress::AnyIPv4, MDNS_PORT,
                QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    m_udp->joinMulticastGroup(QHostAddress(MDNS_ADDR));

    sendMdnsQuery();
    m_timer->start();

    // Mock devices để demo UI
    QTimer::singleShot(700, this, [this]() {
        LoungeDevice d1;
        d1.id = "chrome-macbook"; d1.name = "MacBook Pro";
        d1.ip = "192.168.1.201";  d1.port = 8008;
        d1.browser = "Chrome";    d1.os = "macOS";
        d1.deviceType = "LAPTOP"; d1.youtubeOpen = true;
        d1.loungeToken = "mock-token-001";
        d1.screenId    = "mock-screen-001";
        m_devices.append(d1);
        emit devicesChanged(); emit deviceFound(d1);
    });
    QTimer::singleShot(1100, this, [this]() {
        LoungeDevice d2;
        d2.id = "edge-winpc";  d2.name = "Windows PC";
        d2.ip = "192.168.1.202"; d2.port = 8008;
        d2.browser = "Edge";   d2.os = "Windows";
        d2.deviceType = "DESKTOP"; d2.youtubeOpen = true;
        d2.loungeToken = "mock-token-002";
        d2.screenId    = "mock-screen-002";
        m_devices.append(d2);
        emit devicesChanged(); emit deviceFound(d2);
    });
    QTimer::singleShot(1500, this, [this]() {
        LoungeDevice d3;
        d3.id = "firefox-linux"; d3.name = "Linux Desktop";
        d3.ip = "192.168.1.203"; d3.port = 8008;
        d3.browser = "Firefox";  d3.os = "Linux";
        d3.deviceType = "DESKTOP"; d3.youtubeOpen = false;
        m_devices.append(d3);
        emit devicesChanged(); emit deviceFound(d3);
    });
}

void LoungeScanner::stopScan() {
    m_scanning = false;
    m_timer->stop();
    m_udp->leaveMulticastGroup(QHostAddress(MDNS_ADDR));
    m_udp->close();
    emit scanningChanged();
}

// Gửi mDNS query tìm _googlecast._tcp.local
// Chrome browser tự broadcast khi YouTube đang mở
void LoungeScanner::sendMdnsQuery() {
    // mDNS query packet cho _googlecast._tcp.local
    // Transaction ID: 0x0000, Flags: 0x0000 (standard query)
    QByteArray query;
    query.append("\x00\x00", 2); // Transaction ID
    query.append("\x00\x00", 2); // Flags
    query.append("\x00\x01", 2); // Questions: 1
    query.append("\x00\x00", 2); // Answer RRs
    query.append("\x00\x00", 2); // Authority RRs
    query.append("\x00\x00", 2); // Additional RRs

    // _googlecast._tcp.local
    auto encodeName = [](const QString &name) {
        QByteArray result;
        for (const QString &part : name.split('.')) {
            result.append(static_cast<char>(part.size()));
            result.append(part.toUtf8());
        }
        result.append('\x00');
        return result;
    };

    query.append(encodeName("_googlecast._tcp.local"));
    query.append("\x00\x0c", 2); // Type: PTR
    query.append("\x00\x01", 2); // Class: IN

    m_udp->writeDatagram(query, QHostAddress(MDNS_ADDR), MDNS_PORT);
    qDebug() << "mDNS query sent for _googlecast._tcp.local";

    // Cũng probe Cast port trên các IP phổ biến
    // (scan subnet 192.168.1.x)
    for (int i = 1; i <= 254; i += 10) {
        QString ip = QString("192.168.1.%1").arg(i);
        QTimer::singleShot(i * 5, this, [this, ip]() {
            probeCastPort(ip);
        });
    }
}

// Probe port 8008 — Chrome mở Cast receiver tại đây
void LoungeScanner::probeCastPort(const QString &ip) {
    QUrl url(QString("http://%1:%2/ssdp/device-desc.xml").arg(ip).arg(CAST_PORT));
    QNetworkRequest req(url);
    req.setTransferTimeout(800);
    auto *reply = m_nam->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply, ip]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Có Cast receiver — lấy thêm thông tin
            QUrl infoUrl(QString("http://%1:%2/setup/eureka_info").arg(ip).arg(CAST_PORT));
            auto *infoReply = m_nam->get(QNetworkRequest(infoUrl));
            infoReply->setProperty("ip", ip);

            connect(infoReply, &QNetworkReply::finished, this, [this, infoReply, ip]() {
                if (infoReply->error() == QNetworkReply::NoError) {
                    auto doc = QJsonDocument::fromJson(infoReply->readAll());
                    if (!doc.isNull()) {
                        QJsonObject obj = doc.object();
                        QString name    = obj["name"].toString();
                        QString model   = obj["model_name"].toString();

                        // Tránh trùng
                        for (const auto &d : m_devices)
                            if (d.ip == ip) { infoReply->deleteLater(); return; }

                        LoungeDevice dev;
                        dev.ip   = ip; dev.port = CAST_PORT;
                        dev.id   = "cast-" + ip;
                        dev.name = name.isEmpty() ? "Chrome on " + ip : name;
                        dev.browser    = model.contains("Chrome") ? "Chrome" : "Browser";
                        dev.deviceType = "LAPTOP";
                        dev.youtubeOpen = true;

                        m_devices.append(dev);
                        emit devicesChanged();
                        emit deviceFound(dev);
                    }
                }
                infoReply->deleteLater();
            });
        }
        reply->deleteLater();
    });
}

void LoungeScanner::onMdnsResponse() {
    while (m_udp->hasPendingDatagrams()) {
        QByteArray data;
        QHostAddress sender;
        quint16 port;
        data.resize(m_udp->pendingDatagramSize());
        m_udp->readDatagram(data.data(), data.size(), &sender, &port);
        parseMdnsResponse(data, sender.toString());
    }
}

void LoungeScanner::parseMdnsResponse(const QByteArray &data, const QString &senderIp) {
    // Parse mDNS response — tìm TXT record có "md=" (model) và "fn=" (friendly name)
    // Đây là simplified parser
    QString response = QString::fromLatin1(data);
    if (!response.contains("_googlecast")) return;

    for (const auto &d : m_devices)
        if (d.ip == senderIp) return; // Tránh trùng

    LoungeDevice dev;
    dev.ip          = senderIp;
    dev.port        = CAST_PORT;
    dev.id          = "mdns-" + senderIp;
    dev.deviceType  = "LAPTOP";
    dev.browser     = "Chrome";
    dev.youtubeOpen = true;

    // Extract friendly name từ TXT record
    int fnIdx = response.indexOf("fn=");
    if (fnIdx != -1) {
        int end = response.indexOf('\x00', fnIdx);
        dev.name = response.mid(fnIdx + 3, end - fnIdx - 3);
    } else {
        dev.name = "Chrome · " + senderIp;
    }

    m_devices.append(dev);
    emit devicesChanged();
    emit deviceFound(dev);
    qDebug() << "mDNS found:" << dev.name << "@" << senderIp;
}

void LoungeScanner::onScanTimeout() {
    stopScan();
}
