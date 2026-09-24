#pragma once
#include <QObject>
#include "CastProtocol.h"
#include "DialProtocol.h"
#include "Device.h"

// RemoteController là lớp trung tâm expose ra QML
// Quản lý kết nối song song Cast + DIAL
class RemoteController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString castState   READ castState   NOTIFY castStateChanged)
    Q_PROPERTY(QString dialState   READ dialState   NOTIFY dialStateChanged)
    Q_PROPERTY(bool    castReady   READ castReady   NOTIFY castReadyChanged)
    Q_PROPERTY(bool    dialReady   READ dialReady   NOTIFY dialReadyChanged)
    Q_PROPERTY(bool    anyReady    READ anyReady    NOTIFY anyReadyChanged)
    Q_PROPERTY(bool    playing     READ playing     NOTIFY playingChanged)
    Q_PROPERTY(int     volume      READ volume      NOTIFY volumeChanged)
    Q_PROPERTY(double  position    READ position    NOTIFY positionChanged)
    Q_PROPERTY(QString deviceName  READ deviceName  NOTIFY deviceNameChanged)

public:
    explicit RemoteController(QObject *parent = nullptr);

    QString castState()  const { return m_cast->state(); }
    QString dialState()  const { return m_dial->state(); }
    bool    castReady()  const { return m_cast->connected(); }
    bool    dialReady()  const { return m_dial->connected(); }
    bool    anyReady()   const { return castReady() || dialReady(); }
    bool    playing()    const { return m_playing; }
    int     volume()     const { return m_volume; }
    double  position()   const { return m_position; }
    QString deviceName() const { return m_deviceName; }

public slots:
    // Kết nối song song cả 2 giao thức
    void connectToDevice(const QString &ip, bool hasCast, bool hasDial,
                         const QString &name);
    void disconnectAll();

    // Playback — ưu tiên Cast, fallback DIAL
    Q_INVOKABLE void togglePlay();
    Q_INVOKABLE void seekTo(double position);
    Q_INVOKABLE void setVolume(int percent);
    Q_INVOKABLE void skipNext();
    Q_INVOKABLE void skipPrev();
    Q_INVOKABLE void setSpeed(double rate);
    Q_INVOKABLE void setRepeat(const QString &mode);
    Q_INVOKABLE void setSubtitles(bool on);
    Q_INVOKABLE void setQuality(const QString &q);
    Q_INVOKABLE void loadVideo(const QString &videoId);

    // Navigation — dùng DIAL
    Q_INVOKABLE void navUp();
    Q_INVOKABLE void navDown();
    Q_INVOKABLE void navLeft();
    Q_INVOKABLE void navRight();
    Q_INVOKABLE void navOk();
    Q_INVOKABLE void navBack();
    Q_INVOKABLE void navHome();

    // Search
    Q_INVOKABLE void searchOnTV(const QString &query);

signals:
    void castStateChanged();
    void dialStateChanged();
    void castReadyChanged();
    void dialReadyChanged();
    void anyReadyChanged();
    void playingChanged();
    void volumeChanged();
    void positionChanged();
    void deviceNameChanged();
    void connectionFailed(const QString &reason);

private:
    CastProtocol *m_cast;
    DialProtocol *m_dial;

    bool    m_playing    = false;
    int     m_volume     = 65;
    double  m_position   = 0.0;
    QString m_deviceName;
};
