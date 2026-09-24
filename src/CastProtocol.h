#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>
#include "Device.h"

// CastProtocol kết nối đến Cast receiver qua TCP port 8009
// Dùng Cast V2 protocol (protobuf-like framing)
class CastProtocol : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString state   READ state    NOTIFY stateChanged)

public:
    explicit CastProtocol(QObject *parent = nullptr);
    ~CastProtocol();

    bool    connected() const { return m_connected; }
    QString state()     const { return m_state; }

public slots:
    void connectToDevice(const QString &ip, int port = 8009);
    void disconnect();

    // Playback commands → dùng Cast media namespace
    void play();
    void pause();
    void seek(double position);
    void setVolume(int percent);
    void skipNext();
    void skipPrev();
    void setPlaybackRate(double rate);
    void loadVideo(const QString &videoId);
    void setRepeatMode(const QString &mode); // REPEAT_OFF / REPEAT_ALL / REPEAT_SINGLE
    void setSubtitles(bool enabled);
    void setQuality(const QString &quality);

signals:
    void connectedChanged();
    void stateChanged();
    void playbackStatusChanged(const QJsonObject &status);
    void error(const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError err);
    void sendHeartbeat();

private:
    void sendMessage(const QByteArray &namespace_, const QJsonObject &payload);
    void handleMessage(const QByteArray &namespace_, const QJsonObject &payload);

    QTcpSocket *m_socket    = nullptr;
    QTimer     *m_heartbeat = nullptr;
    bool        m_connected = false;
    QString     m_state     = "disconnected";
    QString     m_sessionId;
    int         m_requestId = 1;

    static constexpr int CAST_PORT = 8009;
    static constexpr int HEARTBEAT_MS = 5000;

    // Cast namespaces
    static constexpr auto NS_CONNECTION = "urn:x-cast:com.google.cast.tp.connection";
    static constexpr auto NS_HEARTBEAT  = "urn:x-cast:com.google.cast.tp.heartbeat";
    static constexpr auto NS_RECEIVER   = "urn:x-cast:com.google.cast.receiver";
    static constexpr auto NS_MEDIA      = "urn:x-cast:com.google.cast.media";
};
