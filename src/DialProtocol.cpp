#include "DialProtocol.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDebug>

DialProtocol::DialProtocol(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_poll(new QTimer(this))
{
    connect(m_poll, &QTimer::timeout, this, &DialProtocol::pollAppState);
}

DialProtocol::~DialProtocol() {
    disconnect();
}

void DialProtocol::connectToDevice(const QString &ip, int port) {
    m_ip   = ip;
    m_port = port;
    m_state = "connecting";
    emit stateChanged();

    // Thử GET /dial/device-desc.xml để xác nhận DIAL support
    QNetworkRequest req(QUrl(baseUrl() + "/dial/device-desc.xml"));
    req.setHeader(QNetworkRequest::UserAgentHeader, "TVRemote/1.0");
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            m_connected = true;
            m_state = "connected";
            emit connectedChanged();
            emit stateChanged();
            m_poll->start(POLL_MS);
            launchYouTube();
        } else {
            m_state = "error";
            emit stateChanged();
            emit error(reply->errorString());
        }
        reply->deleteLater();
    });
}

void DialProtocol::disconnect() {
    m_poll->stop();
    m_connected = false;
    m_state = "disconnected";
    emit connectedChanged();
    emit stateChanged();
}

void DialProtocol::launchYouTube() {
    // POST /apps/YouTube để mở app
    QNetworkRequest req(QUrl(baseUrl() + "/apps/" + YT_APP));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    req.setHeader(QNetworkRequest::UserAgentHeader, "TVRemote/1.0");
    auto *reply = m_nam->post(req, QByteArray(""));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "YouTube launched via DIAL";
            // Lấy loungeToken từ response header
            QString loc = reply->header(QNetworkRequest::LocationHeader).toString();
            qDebug() << "App instance URL:" << loc;
        }
        reply->deleteLater();
    });
}

void DialProtocol::stopYouTube() {
    // DELETE /apps/YouTube/run để đóng app
    QNetworkRequest req(QUrl(baseUrl() + "/apps/" + YT_APP + "/run"));
    req.setHeader(QNetworkRequest::UserAgentHeader, "TVRemote/1.0");
    auto *reply = m_nam->deleteResource(req);
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void DialProtocol::getAppState() {
    // GET /apps/YouTube để lấy trạng thái
    QNetworkRequest req(QUrl(baseUrl() + "/apps/" + YT_APP));
    req.setHeader(QNetworkRequest::UserAgentHeader, "TVRemote/1.0");
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onAppStateReply(reply);
    });
}

void DialProtocol::onAppStateReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString xml = QString::fromUtf8(reply->readAll());
        // Parse state từ XML đơn giản
        if (xml.contains("<state>running</state>"))
            emit appStateReceived("running");
        else if (xml.contains("<state>stopped</state>"))
            emit appStateReceived("stopped");
    }
    reply->deleteLater();
}

void DialProtocol::pollAppState() {
    if (m_connected) getAppState();
}

// ── Navigation commands qua YouTube Lounge API ────────────────
// DIAL không có navigation trực tiếp
// Dùng YouTube Lounge token để gửi keyevent

void DialProtocol::sendRequest(const QString &path,
                                const QByteArray &method,
                                const QByteArray &body) {
    QNetworkRequest req(QUrl(baseUrl() + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::UserAgentHeader, "TVRemote/1.0");

    if (method == "POST")
        m_nam->post(req, body)->deleteLater();
    else
        m_nam->get(req)->deleteLater();
}

// YouTube Lounge API navigation
// POST https://www.youtube.com/api/lounge/bc/bind
// với các tham số keyEvent

void DialProtocol::navigateUp() {
    sendRequest("/lounge/bc/bind", "POST", "keyCode=38"); // ArrowUp
}
void DialProtocol::navigateDown() {
    sendRequest("/lounge/bc/bind", "POST", "keyCode=40"); // ArrowDown
}
void DialProtocol::navigateLeft() {
    sendRequest("/lounge/bc/bind", "POST", "keyCode=37"); // ArrowLeft
}
void DialProtocol::navigateRight() {
    sendRequest("/lounge/bc/bind", "POST", "keyCode=39"); // ArrowRight
}
void DialProtocol::selectOk() {
    sendRequest("/lounge/bc/bind", "POST", "keyCode=13"); // Enter
}
void DialProtocol::goBack() {
    sendRequest("/lounge/bc/bind", "POST", "keyCode=8");  // Backspace
}
void DialProtocol::goHome() {
    launchYouTube(); // Re-launch về trang chủ YouTube
}
