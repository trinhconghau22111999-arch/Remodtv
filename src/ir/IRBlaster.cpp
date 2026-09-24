#include "IRBlaster.h"
#include <QDebug>
#include <QTimer>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniEnvironment>
#include <QJniObject>
#include <QPermissions>
#endif

IRBlaster::IRBlaster(QObject *parent) : QObject(parent) {
    m_available = checkAvailability();
}

bool IRBlaster::checkAvailability() {
#ifdef Q_OS_ANDROID
    // Gọi Android ConsumerIrManager.hasIrEmitter()
    QJniObject activity(QNativeInterface::QAndroidApplication::context());

    // Lấy CONSUMER_IR_SERVICE
    QJniObject irService = activity.callObjectMethod(
        "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;",
        QJniObject::fromString("consumer_ir").object<jstring>()
    );

    if (irService.isValid()) {
        m_irManager = irService;
        bool hasEmitter = m_irManager.callMethod<jboolean>("hasIrEmitter");
        qDebug() << "IR Blaster available:" << hasEmitter;
        return hasEmitter;
    }
    return false;
#else
    // Desktop — mock available để test
    qDebug() << "IR Blaster: Desktop mode (mock)";
    return true;
#endif
}

bool IRBlaster::transmit(int frequency, const QVector<int> &pattern) {
    if (!m_available) {
        qWarning() << "IR not available";
        return false;
    }

#ifdef Q_OS_ANDROID
    // Chuyển QVector<int> → jintArray
    QJniEnvironment env;
    jintArray jPattern = env->NewIntArray(pattern.size());
    env->SetIntArrayRegion(
        jPattern, 0, pattern.size(),
        reinterpret_cast<const jint*>(pattern.constData())
    );

    // Gọi ConsumerIrManager.transmit(frequency, pattern)
    m_irManager.callMethod<void>(
        "transmit", "(I[I)V",
        static_cast<jint>(frequency),
        jPattern
    );

    env->DeleteLocalRef(jPattern);

    // checkAndClearExceptions() trả về true nếu có exception
    const bool hadException = env.checkAndClearExceptions();
    return !hadException;
#else
    // Desktop mock — in ra pattern để debug
    qDebug() << "IR transmit mock — freq:" << frequency
             << "pattern size:" << pattern.size();
    return true;
#endif
}

bool IRBlaster::sendCommand(const IRCode &code) {
    bool ok = transmit(code.frequency, code.pattern);
    emit commandSent(ok);
    return ok;
}

bool IRBlaster::sendCommand(const IRDevice &device, IRCommand cmd) {
    if (!device.hasCommand(cmd)) {
        qWarning() << "Command" << commandName(cmd) << "not found for" << device.brand;
        return false;
    }
    return sendCommand(device.codes[cmd]);
}

QList<int> IRBlaster::supportedFrequencies() const {
    QList<int> freqs;
#ifdef Q_OS_ANDROID
    if (!m_irManager.isValid()) return freqs;

    // getCarrierFrequencies() → CarrierFrequencyRange[]
    auto ranges = m_irManager.callObjectMethod(
        "getCarrierFrequencies",
        "()[Landroid/hardware/ConsumerIrManager$CarrierFrequencyRange;"
    );
    // Parse ranges...
    freqs << 38000 << 36000 << 40000 << 56000;
#else
    freqs << 38000 << 36000 << 40000 << 56000;
#endif
    return freqs;
}

// ── Learning mode ─────────────────────────────────────────────
// Android không có API học IR trực tiếp
// Giải pháp: dùng microphone capture + FFT để decode tín hiệu IR
// Hoặc dùng camera IR (một số thiết bị)
// Ở đây implement qua audio capture

void IRBlaster::startLearning() {
    if (m_learning) return;
    m_learning = true;
    emit learningChanged();

#ifdef Q_OS_ANDROID
    // Yêu cầu permission RECORD_AUDIO
    qApp->requestPermission(QMicrophonePermission{}, this,
        [this](const QPermission &permission) {
        if (permission.status() == Qt::PermissionStatus::Granted) {
            // Bắt đầu capture audio để học IR
            // TODO: implement AudioRecord + FFT decode
            qDebug() << "IR Learning: Audio capture started";
        }
    });
#else
    qDebug() << "IR Learning mock started";
    // Mock: phát ra code giả sau 2 giây
    QTimer::singleShot(2000, this, [this]() {
        IRCode mockCode;
        mockCode.frequency = 38000;
        mockCode.pattern = {9000,4500, 560,1690, 560,560, 560,1690, 560,560};
        mockCode.learned = true;
        emit codelearned(mockCode);
        stopLearning();
    });
#endif
}

void IRBlaster::stopLearning() {
    m_learning = false;
    emit learningChanged();
}
