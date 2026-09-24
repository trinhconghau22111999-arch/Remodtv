#pragma once
#include <QObject>
#include "IRBlaster.h"
#include "IRDatabase.h"

// IRController — expose ra QML, quản lý:
// - Chọn hãng TV
// - Gửi lệnh IR
// - Học mã từ remote
class IRController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool       irAvailable  READ irAvailable  NOTIFY irAvailableChanged)
    Q_PROPERTY(bool       learning     READ learning     NOTIFY learningChanged)
    Q_PROPERTY(QString    brand        READ brand        WRITE setBrand NOTIFY brandChanged)
    Q_PROPERTY(QStringList brands      READ brands       NOTIFY brandsChanged)
    Q_PROPERTY(bool       deviceReady  READ deviceReady  NOTIFY deviceReadyChanged)

public:
    explicit IRController(QObject *parent = nullptr);

    bool        irAvailable() const { return m_blaster->available(); }
    bool        learning()    const { return m_blaster->learning(); }
    QString     brand()       const { return m_brand; }
    QStringList brands()      const { return IRDatabase::instance()->availableBrands(); }
    bool        deviceReady() const { return !m_device.id.isEmpty(); }

    void setBrand(const QString &b);

public slots:
    // Lệnh điều khiển TV — gọi từ QML
    Q_INVOKABLE void power();
    Q_INVOKABLE void volumeUp();
    Q_INVOKABLE void volumeDown();
    Q_INVOKABLE void mute();
    Q_INVOKABLE void channelUp();
    Q_INVOKABLE void channelDown();
    Q_INVOKABLE void up();
    Q_INVOKABLE void down();
    Q_INVOKABLE void left();
    Q_INVOKABLE void right();
    Q_INVOKABLE void ok();
    Q_INVOKABLE void back();
    Q_INVOKABLE void home();
    Q_INVOKABLE void inputSource();
    Q_INVOKABLE void hdmi(int port);
    Q_INVOKABLE void netflix();
    Q_INVOKABLE void youtube();

    // Học mã
    Q_INVOKABLE void startLearning(const QString &commandName);
    Q_INVOKABLE void stopLearning();

    // Lưu thiết bị đã học
    Q_INVOKABLE void saveLearned();

signals:
    void irAvailableChanged();
    void learningChanged();
    void brandChanged();
    void brandsChanged();
    void deviceReadyChanged();
    void commandResult(bool success, const QString &command);
    void learningComplete(const QString &command);

private:
    void send(IRCommand cmd);

    IRBlaster  *m_blaster;
    IRDevice    m_device;
    QString     m_brand;
    QString     m_learningCommand;
};
