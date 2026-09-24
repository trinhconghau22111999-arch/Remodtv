#include "IRController.h"
#include <QDebug>

IRController::IRController(QObject *parent)
    : QObject(parent)
    , m_blaster(new IRBlaster(this))
{
    connect(m_blaster, &IRBlaster::availableChanged,
            this, &IRController::irAvailableChanged);
    connect(m_blaster, &IRBlaster::learningChanged,
            this, &IRController::learningChanged);
    connect(m_blaster, &IRBlaster::codelearned,
            this, [this](const IRCode &code) {
        // Lưu mã học được vào thiết bị hiện tại
        if (!m_learningCommand.isEmpty()) {
            // Map tên lệnh → enum (đơn giản hoá)
            static const QMap<QString,IRCommand> nameToCmd = {
                {"Power", IRCommand::Power},
                {"Volume Up", IRCommand::VolumeUp},
                {"Volume Down", IRCommand::VolumeDown},
                {"Mute", IRCommand::Mute},
                {"Channel Up", IRCommand::ChannelUp},
                {"Channel Down", IRCommand::ChannelDown},
                {"Up", IRCommand::Up}, {"Down", IRCommand::Down},
                {"Left", IRCommand::Left}, {"Right", IRCommand::Right},
                {"OK", IRCommand::Ok}, {"Back", IRCommand::Back},
                {"Home", IRCommand::Home},
            };
            if (nameToCmd.contains(m_learningCommand))
                m_device.codes[nameToCmd[m_learningCommand]] = code;
            emit learningComplete(m_learningCommand);
            m_learningCommand.clear();
        }
    });
}

void IRController::setBrand(const QString &b) {
    if (m_brand == b) return;
    m_brand  = b;
    m_device = IRDatabase::instance()->bestMatch(b);
    emit brandChanged();
    emit deviceReadyChanged();
    qDebug() << "IR brand set to:" << b << "device ready:" << deviceReady();
}

void IRController::send(IRCommand cmd) {
    if (!m_blaster->available()) {
        qWarning() << "IR not available";
        emit commandResult(false, commandName(cmd));
        return;
    }
    bool ok = m_blaster->sendCommand(m_device, cmd);
    emit commandResult(ok, commandName(cmd));
    qDebug() << "IR send" << commandName(cmd) << (ok ? "OK" : "FAIL");
}

void IRController::power()        { send(IRCommand::Power); }
void IRController::volumeUp()     { send(IRCommand::VolumeUp); }
void IRController::volumeDown()   { send(IRCommand::VolumeDown); }
void IRController::mute()         { send(IRCommand::Mute); }
void IRController::channelUp()    { send(IRCommand::ChannelUp); }
void IRController::channelDown()  { send(IRCommand::ChannelDown); }
void IRController::up()           { send(IRCommand::Up); }
void IRController::down()         { send(IRCommand::Down); }
void IRController::left()         { send(IRCommand::Left); }
void IRController::right()        { send(IRCommand::Right); }
void IRController::ok()           { send(IRCommand::Ok); }
void IRController::back()         { send(IRCommand::Back); }
void IRController::home()         { send(IRCommand::Home); }
void IRController::inputSource()  { send(IRCommand::InputSource); }
void IRController::netflix()      { send(IRCommand::Netflix); }
void IRController::youtube()      { send(IRCommand::YouTube); }

void IRController::hdmi(int port) {
    switch (port) {
    case 1: send(IRCommand::HDMI1); break;
    case 2: send(IRCommand::HDMI2); break;
    case 3: send(IRCommand::HDMI3); break;
    default: send(IRCommand::InputSource);
    }
}

void IRController::startLearning(const QString &commandName) {
    m_learningCommand = commandName;
    m_blaster->startLearning();
}

void IRController::stopLearning() {
    m_learningCommand.clear();
    m_blaster->stopLearning();
}

void IRController::saveLearned() {
    if (!m_device.id.isEmpty())
        IRDatabase::instance()->saveLearned(m_device);
}
