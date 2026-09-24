#include "RemoteController.h"
#include <QDebug>

RemoteController::RemoteController(QObject *parent)
    : QObject(parent)
    , m_cast(new CastProtocol(this))
    , m_dial(new DialProtocol(this))
{
    connect(m_cast, &CastProtocol::connectedChanged, this, [this]() {
        emit castStateChanged();
        emit castReadyChanged();
        emit anyReadyChanged();
    });
    connect(m_dial, &DialProtocol::connectedChanged, this, [this]() {
        emit dialStateChanged();
        emit dialReadyChanged();
        emit anyReadyChanged();
    });
    connect(m_cast, &CastProtocol::stateChanged, this, &RemoteController::castStateChanged);
    connect(m_dial, &DialProtocol::stateChanged, this, &RemoteController::dialStateChanged);
}

void RemoteController::connectToDevice(const QString &ip, bool hasCast,
                                        bool hasDial, const QString &name) {
    m_deviceName = name;
    emit deviceNameChanged();

    // Kết nối song song — cả 2 cùng lúc
    if (hasCast) m_cast->connectToDevice(ip, 8009);
    if (hasDial) m_dial->connectToDevice(ip, 56790);
}

void RemoteController::disconnectAll() {
    m_cast->disconnect();
    m_dial->disconnect();
}

// ── Playback ─────────────────────────────────────────────────

void RemoteController::togglePlay() {
    m_playing = !m_playing;
    if (castReady()) {
        m_playing ? m_cast->play() : m_cast->pause();
    }
    emit playingChanged();
}

void RemoteController::seekTo(double pos) {
    m_position = pos;
    if (castReady()) m_cast->seek(pos);
    emit positionChanged();
}

void RemoteController::setVolume(int percent) {
    m_volume = percent;
    if (castReady()) m_cast->setVolume(percent);
    emit volumeChanged();
}

void RemoteController::skipNext() {
    if (castReady()) m_cast->skipNext();
}

void RemoteController::skipPrev() {
    if (castReady()) m_cast->skipPrev();
}

void RemoteController::setSpeed(double rate) {
    if (castReady()) m_cast->setPlaybackRate(rate);
}

void RemoteController::setRepeat(const QString &mode) {
    if (castReady()) m_cast->setRepeatMode(mode);
}

void RemoteController::setSubtitles(bool on) {
    if (castReady()) m_cast->setSubtitles(on);
}

void RemoteController::setQuality(const QString &q) {
    if (castReady()) m_cast->setQuality(q);
}

void RemoteController::loadVideo(const QString &videoId) {
    if (castReady()) m_cast->loadVideo(videoId);
}

// ── Navigation qua DIAL ───────────────────────────────────────

void RemoteController::navUp()    { if (dialReady()) m_dial->navigateUp(); }
void RemoteController::navDown()  { if (dialReady()) m_dial->navigateDown(); }
void RemoteController::navLeft()  { if (dialReady()) m_dial->navigateLeft(); }
void RemoteController::navRight() { if (dialReady()) m_dial->navigateRight(); }
void RemoteController::navOk()    { if (dialReady()) m_dial->selectOk(); }
void RemoteController::navBack()  { if (dialReady()) m_dial->goBack(); }
void RemoteController::navHome()  { if (dialReady()) m_dial->goHome(); }

void RemoteController::searchOnTV(const QString &query) {
    // Load video tìm kiếm qua Cast
    if (castReady()) m_cast->loadVideo("search:" + query);
    // Hoặc điều hướng đến search qua DIAL
    else if (dialReady()) m_dial->launchYouTube();
}
