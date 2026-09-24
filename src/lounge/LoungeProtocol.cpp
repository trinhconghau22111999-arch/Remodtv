#include "LoungeProtocol.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

LoungeProtocol::LoungeProtocol(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_heartbeat(new QTimer(this))
    , m_pollTimer(new QTimer(this))
{
    connect(m_heartbeat, &QTimer::timeout, this, &LoungeProtocol::sendHeartbeat);
    connect(m_pollTimer, &QTimer::timeout, this, &LoungeProtocol::startLongPoll);
}

// ── Kết nối ──────────────────────────────────────────────────

void LoungeProtocol::connectToDevice(const LoungeDevice &device) {
    m_loungeToken = device.loungeToken;
    m_screenId    = device.screenId;
    m_state       = "connecting";
    emit stateChanged();

    // Bước 1: GET bind để lấy gsessionid và sid
    // YouTube Lounge API endpoint
    QUrl url(baseUrl() + "/api/lounge/bc/bind");
    QUrlQuery q;
    q.addQueryItem("device",         "REMOTE_CONTROL");
    q.addQueryItem("id",             "tvremote-" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    q.addQueryItem("name",           "TV Remote App");
    q.addQueryItem("app",            "youtube-desktop");
    q.addQueryItem("loungeIdToken",  m_loungeToken);
    q.addQueryItem("VER",            "8");
    q.addQueryItem("v",              "2");
    q.addQueryItem("RID",            QString::number(m_requestId++));
    url.setQuery(q);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::UserAgentHeader,
                  "Mozilla/5.0 TVRemote/1.0");

    // POST bind với count=0 để khởi tạo session
    auto *reply = m_nam->post(req, QByteArray("count=0"));
    connect(reply, &QNetworkReply::finished, this,
            [this, reply]() { onBindReply(reply); });
}

void LoungeProtocol::onBindReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        m_state = "error";
        emit stateChanged();
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    // Response format: )]}'\n[sessionId, [[0, ["c", "sid", "", 8]]]]
    // Parse sid và gsessionid
    QString resp = QString::fromUtf8(data);

    // Extract SID
    int sidIdx = resp.indexOf("\"c\",\"");
    if (sidIdx != -1) {
        int start = sidIdx + 5;
        int end   = resp.indexOf("\"", start);
        m_sid = resp.mid(start, end - start);
        qDebug() << "Lounge SID:" << m_sid;
    }

    // Extract gsessionid từ header
    m_gsessionId = reply->rawHeader("X-HTTP-Session-Id");

    if (!m_sid.isEmpty()) {
        m_connected = true;
        m_state     = "connected";
        emit connectedChanged();
        emit stateChanged();

        // Bắt đầu heartbeat và long-poll
        m_heartbeat->start(HEARTBEAT_MS);
        m_pollTimer->start(POLL_MS);
        startLongPoll();

        qDebug() << "Lounge connected! SID:" << m_sid;
    }
    reply->deleteLater();
}

void LoungeProtocol::disconnect() {
    m_heartbeat->stop();
    m_pollTimer->stop();
    m_connected  = false;
    m_state      = "disconnected";
    m_sid.clear();
    m_gsessionId.clear();
    emit connectedChanged();
    emit stateChanged();
}

// ── Long polling — nhận trạng thái real-time ─────────────────

void LoungeProtocol::startLongPoll() {
    if (!m_connected) return;

    QUrl url(baseUrl() + "/api/lounge/bc/bind");
    QUrlQuery q;
    q.addQueryItem("device",        "REMOTE_CONTROL");
    q.addQueryItem("loungeIdToken", m_loungeToken);
    q.addQueryItem("SID",           m_sid);
    q.addQueryItem("gsessionid",    m_gsessionId);
    q.addQueryItem("VER",           "8");
    q.addQueryItem("v",             "2");
    q.addQueryItem("t",             "1");
    q.addQueryItem("RID",           "rpc");
    q.addQueryItem("TYPE",          "xmlhttp");
    q.addQueryItem("AID",           "0");
    url.setQuery(q);

    QNetworkRequest req(url);
    req.setTransferTimeout(30000); // 30s long-poll
    auto *reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this,
            [this, reply]() { onLongPollReply(reply); });
}

void LoungeProtocol::onLongPollReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        parseStatusUpdate(reply->readAll());
    }
    reply->deleteLater();
}

void LoungeProtocol::parseStatusUpdate(const QByteArray &data) {
    // YouTube Lounge response là format đặc biệt:
    // length\n[[[seq, ["type", {data}]], ...]]
    QString resp = QString::fromUtf8(data);

    // Tìm "nowPlaying" event
    if (resp.contains("nowPlaying")) {
        int idx = resp.indexOf("\"videoId\":\"");
        if (idx != -1) {
            int start = idx + 11;
            int end   = resp.indexOf("\"", start);
            m_videoId = resp.mid(start, end - start);
        }
        idx = resp.indexOf("\"title\":\"");
        if (idx != -1) {
            int start = idx + 9;
            int end   = resp.indexOf("\"", start);
            m_videoTitle = resp.mid(start, end - start);
        }
        // State: playing/paused
        m_playing = resp.contains("\"state\":\"1\"");
        emit statusChanged();
    }

    // Volume update
    if (resp.contains("\"volume\":")) {
        int idx = resp.indexOf("\"volume\":");
        if (idx != -1) {
            int start = idx + 9;
            int end   = resp.indexOf(",", start);
            if (end == -1) end = resp.indexOf("}", start);
            m_volume = resp.mid(start, end - start).toInt();
            emit statusChanged();
        }
    }
}

// ── Gửi lệnh ─────────────────────────────────────────────────

void LoungeProtocol::sendCommand(const QString &command,
                                  const QJsonObject &params) {
    if (!m_connected) return;

    QUrl url(baseUrl() + "/api/lounge/bc/bind");
    QUrlQuery q;
    q.addQueryItem("device",        "REMOTE_CONTROL");
    q.addQueryItem("loungeIdToken", m_loungeToken);
    q.addQueryItem("SID",           m_sid);
    q.addQueryItem("gsessionid",    m_gsessionId);
    q.addQueryItem("VER",           "8");
    q.addQueryItem("v",             "2");
    q.addQueryItem("RID",           QString::number(m_requestId++));
    url.setQuery(q);

    // Body: count=1&ofs=N&req0_=command&req0_param=value
    QUrlQuery body;
    body.addQueryItem("count", "1");
    body.addQueryItem("ofs",   QString::number(m_ofs++));
    body.addQueryItem("req0__sc", command);

    for (auto it = params.begin(); it != params.end(); ++it) {
        body.addQueryItem("req0_" + it.key(),
                          it.value().toVariant().toString());
    }

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/x-www-form-urlencoded");
    req.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 TVRemote/1.0");

    auto *reply = m_nam->post(req, body.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);

    qDebug() << "Lounge command:" << command << params;
}

void LoungeProtocol::sendHeartbeat() {
    sendCommand("getNowPlaying");
}

// ── Playback commands ─────────────────────────────────────────

void LoungeProtocol::play() {
    m_playing = true;
    emit statusChanged();
    sendCommand("play");
}

void LoungeProtocol::pause() {
    m_playing = false;
    emit statusChanged();
    sendCommand("pause");
}

void LoungeProtocol::seekTo(double seconds) {
    sendCommand("seekTo", {{"newTime", seconds}});
}

void LoungeProtocol::setVolume(int percent) {
    m_volume = percent;
    emit statusChanged();
    sendCommand("onUserActivity");
    sendCommand("setVolume", {{"volume", percent}, {"muted", false}});
}

void LoungeProtocol::skipNext() {
    sendCommand("next");
}

void LoungeProtocol::skipPrev() {
    sendCommand("previous");
}

void LoungeProtocol::setPlaybackRate(double rate) {
    sendCommand("setPlaybackRate", {{"playbackRate", rate}});
}

void LoungeProtocol::setSubtitles(bool on) {
    sendCommand("setSubtitlesEnabled", {{"enabled", on}});
}

void LoungeProtocol::setQuality(const QString &quality) {
    sendCommand("setPlaybackQuality", {{"quality", quality}});
}

void LoungeProtocol::loadVideo(const QString &videoId) {
    sendCommand("setPlaylist", {
        {"videoId",    videoId},
        {"listId",     ""},
        {"currentTime","0"},
        {"audioOnly",  "false"},
        {"playerParams", ""}
    });
}

void LoungeProtocol::addToQueue(const QString &videoId) {
    sendCommand("addVideo", {{"videoId", videoId}});
}

void LoungeProtocol::setRepeatMode(const QString &mode) {
    sendCommand("setLoop", {{"loop", mode == "REPEAT_ALL" ? "true" : "false"}});
}

void LoungeProtocol::setFullscreen(bool on) {
    sendCommand("setFullscreen", {{"fullscreen", on ? "true" : "false"}});
}

void LoungeProtocol::searchAndPlay(const QString &query) {
    // Mở YouTube search trên laptop
    sendCommand("setPlaylist", {
        {"videoId",    ""},
        {"listId",     ""},
        {"searchQuery", query}
    });
}
