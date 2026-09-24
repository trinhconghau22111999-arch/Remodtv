#include "DeviceScanner.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDomDocument>
#include <QHostAddress>
#include <QDebug>

DeviceScanner::DeviceScanner(QObject *parent)
    : QObject(parent)
    , m_udpSocket(new QUdpSocket(this))
    , m_scanTimer(new QTimer(this))
    , m_nam(new QNetworkAccessManager(this))
{
    connect(m_udpSocket, &QUdpSocket::readyRead,
            this, &DeviceScanner::onSsdpResponse);

    m_scanTimer->setSingleShot(true);
    m_scanTimer->setInterval(5000); // 5 giây timeout
    connect(m_scanTimer, &QTimer::timeout,
            this, &DeviceScanner::onScanTimeout);
}

DeviceScanner::~DeviceScanner() = default;

QVariantList DeviceScanner::devicesVariant() const {
    QVariantList list;
    for (const auto &d : m_devices) {
        QVariantMap m;
        m["id"]            = d.id;
        m["name"]          = d.name;
        m["ip"]            = d.ip;
        m["port"]          = d.port;
        m["dialPort"]      = d.dialPort;
        m["hasCast"]       = d.supportsCast();
        m["hasDial"]       = d.supportsDial();
        m["protocolLabel"] = d.protocolLabel();
        m["signal"]        = d.signalStrength;
        list.append(m);
    }
    return list;
}

void DeviceScanner::startScan() {
    if (m_scanning) return;
    m_devices.clear();
    m_scanning = true;
    emit scanningChanged();
    emit devicesChanged();

    // Bind UDP cho SSDP
    m_udpSocket->bind(QHostAddress::AnyIPv4, 0,
                      QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    sendSsdpDiscover();
    m_scanTimer->start();

    // Mock devices để demo (xoá khi deploy thật)
    QTimer::singleShot(600, this, [this]() {
        Device d1;
        d1.id = "sony-bravia-xr"; d1.name = "Sony Bravia XR";
        d1.ip = "192.168.1.105";  d1.port = CAST_PORT;
        d1.dialPort = DIAL_PORT;  d1.protocols = Protocol::Cast | Protocol::Dial;
        d1.signalStrength = 3;
        m_devices.append(d1); emit devicesChanged(); emit deviceFound(d1);
    });
    QTimer::singleShot(1000, this, [this]() {
        Device d2;
        d2.id = "chromecast-4k"; d2.name = "Chromecast 4K";
        d2.ip = "192.168.1.120"; d2.port = CAST_PORT;
        d2.dialPort = DIAL_PORT; d2.protocols = Protocol::Cast | Protocol::Dial;
        d2.signalStrength = 2;
        m_devices.append(d2); emit devicesChanged(); emit deviceFound(d2);
    });
    QTimer::singleShot(1400, this, [this]() {
        Device d3;
        d3.id = "samsung-qled"; d3.name = "Samsung QLED 4K";
        d3.ip = "192.168.1.108"; d3.port = CAST_PORT;
        d3.dialPort = DIAL_PORT; d3.protocols = Protocol::Dial;
        d3.signalStrength = 2;
        m_devices.append(d3); emit devicesChanged(); emit deviceFound(d3);
    });
    QTimer::singleShot(1800, this, [this]() {
        Device d4;
        d4.id = "xiaomi-mibox"; d4.name = "Xiaomi Mi Box S";
        d4.ip = "192.168.1.130"; d4.port = CAST_PORT;
        d4.dialPort = DIAL_PORT; d4.protocols = Protocol::Cast;
        d4.signalStrength = 3;
        m_devices.append(d4); emit devicesChanged(); emit deviceFound(d4);
    });
}

void DeviceScanner::stopScan() {
    m_scanning = false;
    m_scanTimer->stop();
    m_udpSocket->close();
    emit scanningChanged();
}

void DeviceScanner::sendSsdpDiscover() {
    // SSDP M-SEARCH để tìm DIAL/Cast devices
    const QByteArray ssdp =
        "M-SEARCH * HTTP/1.1\r\n"
        "HOST: 239.255.255.250:1900\r\n"
        "MAN: \"ssdp:discover\"\r\n"
        "MX: 3\r\n"
        "ST: urn:dial-multiscreen-org:service:dial:1\r\n"
        "\r\n";

    m_udpSocket->writeDatagram(ssdp,
        QHostAddress(SSDP_ADDR), SSDP_PORT);

    // Cũng tìm Cast
    const QByteArray ssdpCast =
        "M-SEARCH * HTTP/1.1\r\n"
        "HOST: 239.255.255.250:1900\r\n"
        "MAN: \"ssdp:discover\"\r\n"
        "MX: 3\r\n"
        "ST: urn:cast-oem-com:service:CastApplication:1\r\n"
        "\r\n";
    m_udpSocket->writeDatagram(ssdpCast,
        QHostAddress(SSDP_ADDR), SSDP_PORT);
}

void DeviceScanner::onSsdpResponse() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray data;
        QHostAddress sender;
        quint16 senderPort;
        data.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(data.data(), data.size(), &sender, &senderPort);

        QString response = QString::fromUtf8(data);
        QString ip = sender.toString();
        qDebug() << "SSDP response from" << ip;

        // Probe thiết bị vừa tìm thấy
        probeDevice(ip, CAST_PORT, DIAL_PORT);
    }
}

void DeviceScanner::probeDevice(const QString &ip, int castPort, int dialPort) {
    // Probe Cast: GET http://ip:8008/ssdp/device-desc.xml
    QUrl castUrl(QString("http://%1:%2/ssdp/device-desc.xml").arg(ip).arg(castPort));
    auto *castReply = m_nam->get(QNetworkRequest(castUrl));
    castReply->setProperty("ip", ip);
    castReply->setProperty("type", "cast");

    connect(castReply, &QNetworkReply::finished, this, [this, castReply, ip, dialPort]() {
        bool hasCast = (castReply->error() == QNetworkReply::NoError);
        castReply->deleteLater();

        // Probe DIAL: GET http://ip:dialPort/dial/device-desc.xml
        QUrl dialUrl(QString("http://%1:%2/dial/device-desc.xml").arg(ip).arg(dialPort));
        auto *dialReply = m_nam->get(QNetworkRequest(dialUrl));

        connect(dialReply, &QNetworkReply::finished, this, [this, dialReply, ip, hasCast]() {
            bool hasDial = (dialReply->error() == QNetworkReply::NoError);
            dialReply->deleteLater();

            if (!hasCast && !hasDial) return;

            // Tạo device
            Device d;
            d.id = ip; d.ip = ip; d.name = "TV (" + ip + ")";
            d.port = CAST_PORT; d.dialPort = DIAL_PORT;
            if (hasCast) d.protocols |= Protocol::Cast;
            if (hasDial) d.protocols |= Protocol::Dial;
            d.signalStrength = 2;

            // Tránh trùng
            for (const auto &existing : m_devices)
                if (existing.ip == ip) return;

            m_devices.append(d);
            emit devicesChanged();
            emit deviceFound(d);
        });
    });
}

void DeviceScanner::onScanTimeout() {
    stopScan();
}
