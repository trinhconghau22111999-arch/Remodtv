#include "CastProtocol.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDataStream>
#include <QDebug>

CastProtocol::CastProtocol(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_heartbeat(new QTimer(this))
{
    connect(m_socket, &QTcpSocket::connected,    this, &CastProtocol::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &CastProtocol::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,    this, &CastProtocol::onReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred,
            this, &CastProtocol::onError);

    m_heartbeat->setInterval(HEARTBEAT_MS);
    connect(m_heartbeat, &QTimer::timeout, this, &CastProtocol::sendHeartbeat);
}

CastProtocol::~CastProtocol() {
    disconnect();
}

void CastProtocol::connectToDevice(const QString &ip, int port) {
    m_state = "connecting";
    emit stateChanged();
    // Cast dùng SSL (TLS) trên port 8009
    // Để đơn giản, thử TCP trước — production cần QSslSocket
    m_socket->connectToHost(ip, port);
}

void CastProtocol::disconnect() {
    m_heartbeat->stop();
    m_socket->disconnectFromHost();
    m_connected = false;
    m_state = "disconnected";
    emit connectedChanged();
    emit stateChanged();
}

void CastProtocol::onConnected() {
    m_connected = true;
    m_state = "connected";
    emit connectedChanged();
    emit stateChanged();

    // Gửi CONNECT message
    sendMessage(NS_CONNECTION, {{"type", "CONNECT"}});
    // Bắt đầu heartbeat
    m_heartbeat->start();
    // Lấy trạng thái receiver
    sendMessage(NS_RECEIVER, {{"type", "GET_STATUS"}, {"requestId", m_requestId++}});
}

void CastProtocol::onDisconnected() {
    m_connected = false;
    m_state = "disconnected";
    m_heartbeat->stop();
    emit connectedChanged();
    emit stateChanged();
}

void CastProtocol::onReadyRead() {
    // Cast framing: 4 bytes big-endian length + protobuf payload
    // Ở đây parse JSON đơn giản để demo
    QByteArray data = m_socket->readAll();
    qDebug() << "Cast received:" << data.size() << "bytes";

    // Parse JSON response
    if (data.size() > 4) {
        QByteArray json = data.mid(4);
        QJsonDocument doc = QJsonDocument::fromJson(json);
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString ns = obj["namespace"].toString();
            QJsonObject payload = obj["payload"].toObject();
            handleMessage(ns.toUtf8(), payload);
        }
    }
}

void CastProtocol::handleMessage(const QByteArray &ns, const QJsonObject &payload) {
    QString type = payload["type"].toString();

    if (ns == NS_HEARTBEAT && type == "PING") {
        sendMessage(NS_HEARTBEAT, {{"type", "PONG"}});
    } else if (ns == NS_RECEIVER && type == "RECEIVER_STATUS") {
        QJsonObject status = payload["status"].toObject();
        QJsonArray apps = status["applications"].toArray();
        if (!apps.isEmpty()) {
            m_sessionId = apps[0].toObject()["sessionId"].toString();
        }
        emit playbackStatusChanged(status);
    } else if (ns == NS_MEDIA && type == "MEDIA_STATUS") {
        emit playbackStatusChanged(payload);
    }
}

void CastProtocol::sendMessage(const QByteArray &namespace_, const QJsonObject &payload) {
    if (!m_connected && namespace_ != NS_CONNECTION) return;

    // Build Cast message (simplified — production cần protobuf)
    QJsonObject msg;
    msg["protocolVersion"] = 0;
    msg["sourceId"]  = "sender-0";
    msg["destinationId"] = "receiver-0";
    msg["namespace"] = QString(namespace_);
    msg["payloadType"] = 0;
    msg["payloadUtf8"] = QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));

    QByteArray json = QJsonDocument(msg).toJson(QJsonDocument::Compact);

    // 4-byte big-endian length header
    QByteArray frame;
    QDataStream stream(&frame, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << quint32(json.size());
    frame.append(json);

    m_socket->write(frame);
}

void CastProtocol::sendHeartbeat() {
    sendMessage(NS_HEARTBEAT, {{"type", "PING"}});
}

// ── Playback commands ──────────────────────────────────────────

void CastProtocol::play() {
    sendMessage(NS_MEDIA, {
        {"type", "PLAY"},
        {"mediaSessionId", 1},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::pause() {
    sendMessage(NS_MEDIA, {
        {"type", "PAUSE"},
        {"mediaSessionId", 1},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::seek(double position) {
    sendMessage(NS_MEDIA, {
        {"type", "SEEK"},
        {"mediaSessionId", 1},
        {"currentTime", position},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::setVolume(int percent) {
    double level = percent / 100.0;
    sendMessage(NS_RECEIVER, {
        {"type", "SET_VOLUME"},
        {"volume", QJsonObject{{"level", level}}},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::skipNext() {
    sendMessage(NS_MEDIA, {
        {"type", "QUEUE_NEXT"},
        {"mediaSessionId", 1},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::skipPrev() {
    sendMessage(NS_MEDIA, {
        {"type", "QUEUE_PREV"},
        {"mediaSessionId", 1},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::setPlaybackRate(double rate) {
    sendMessage(NS_MEDIA, {
        {"type", "SET_PLAYBACK_RATE"},
        {"playbackRate", rate},
        {"mediaSessionId", 1},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::loadVideo(const QString &videoId) {
    QJsonObject media;
    media["contentId"]   = "https://www.youtube.com/watch?v=" + videoId;
    media["contentType"] = "video/mp4";
    media["streamType"]  = "BUFFERED";

    sendMessage(NS_MEDIA, {
        {"type", "LOAD"},
        {"media", media},
        {"autoplay", true},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::setRepeatMode(const QString &mode) {
    sendMessage(NS_MEDIA, {
        {"type", "QUEUE_UPDATE"},
        {"repeatMode", mode},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::setSubtitles(bool enabled) {
    sendMessage(NS_MEDIA, {
        {"type", enabled ? "TEXT_TRACKS_STYLE" : "EDIT_TRACKS_INFO"},
        {"activeTrackIds", enabled ? QJsonArray{0} : QJsonArray{}},
        {"requestId", m_requestId++}
    });
}

void CastProtocol::setQuality(const QString &quality) {
    Q_UNUSED(quality)
    // YouTube app trên Cast tự xử lý quality
    // Gửi custom event đến YouTube receiver
    sendMessage("urn:x-cast:com.google.youtube.mdx", {
        {"type", "setConfig"},
        {"quality", quality}
    });
}

void CastProtocol::onError(QAbstractSocket::SocketError err) {
    qWarning() << "Cast socket error:" << err;
    m_state = "error";
    emit stateChanged();
    emit error(m_socket->errorString());
}
