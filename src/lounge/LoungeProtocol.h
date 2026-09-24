#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonObject>
#include "LoungeDevice.h"

// LoungeProtocol — điều khiển YouTube qua YouTube Lounge API
// Đây là giao thức Chrome dùng để "Cast" YouTube lên TV
// Hoạt động với cả Chrome trên laptop và YouTube trên Smart TV
//
// Flow:
//   1. GET /api/lounge/bc/bind?loungeIdToken=... → nhận gsessionid
//   2. POST /api/lounge/bc/bind với commands → gửi lệnh
//   3. Long-poll GET → nhận trạng thái real-time
class LoungeProtocol : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool    connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString state     READ state     NOTIFY stateChanged)
    Q_PROPERTY(QString videoTitle READ videoTitle NOTIFY statusChanged)
    Q_PROPERTY(bool    playing   READ playing   NOTIFY statusChanged)
    Q_PROPERTY(int     volume    READ volume    NOTIFY statusChanged)

public:
    explicit LoungeProtocol(QObject *parent = nullptr);

    bool    connected()  const { return m_connected; }
    QString state()      const { return m_state; }
    QString videoTitle() const { return m_videoTitle; }
    bool    playing()    const { return m_playing; }
    int     volume()     const { return m_volume; }

public slots:
    // Kết nối tự động — dùng token từ mDNS discovery
    void connectToDevice(const LoungeDevice &device);
    void disconnect();

    // Playback commands
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void seekTo(double seconds);
    Q_INVOKABLE void setVolume(int percent);
    Q_INVOKABLE void skipNext();
    Q_INVOKABLE void skipPrev();
    Q_INVOKABLE void setPlaybackRate(double rate);
    Q_INVOKABLE void setSubtitles(bool on);
    Q_INVOKABLE void setQuality(const QString &quality);
    Q_INVOKABLE void loadVideo(const QString &videoId);
    Q_INVOKABLE void addToQueue(const QString &videoId);
    Q_INVOKABLE void setRepeatMode(const QString &mode);
    Q_INVOKABLE void setFullscreen(bool on);
    Q_INVOKABLE void searchAndPlay(const QString &query);

signals:
    void connectedChanged();
    void stateChanged();
    void statusChanged();
    void error(const QString &msg);

private slots:
    void onBindReply(QNetworkReply *reply);
    void startLongPoll();
    void onLongPollReply(QNetworkReply *reply);
    void sendHeartbeat();

private:
    void sendCommand(const QString &command, const QJsonObject &params = {});
    void parseStatusUpdate(const QByteArray &data);
    QString baseUrl() const { return "https://www.youtube.com"; }

    QNetworkAccessManager *m_nam       = nullptr;
    QTimer                *m_heartbeat = nullptr;
    QTimer                *m_pollTimer = nullptr;

    bool    m_connected  = false;
    QString m_state      = "disconnected";
    QString m_loungeToken;
    QString m_screenId;
    QString m_gsessionId;
    QString m_sid;
    int     m_requestId  = 0;
    int     m_ofs        = 0;

    // Playback state
    QString m_videoTitle;
    QString m_videoId;
    bool    m_playing    = false;
    int     m_volume     = 100;
    double  m_position   = 0;
    double  m_duration   = 0;

    static constexpr int HEARTBEAT_MS = 20000;
    static constexpr int POLL_MS      = 3000;
};
