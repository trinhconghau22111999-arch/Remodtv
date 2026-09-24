#pragma once
#include <QObject>
#include <QAndroidJniObject>
#include "IRDevice.h"

// IRBlaster — giao tiếp với Android ConsumerIrManager
// API: android.hardware.ConsumerIrManager (API 19+)
class IRBlaster : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(bool learning  READ learning  NOTIFY learningChanged)

public:
    explicit IRBlaster(QObject *parent = nullptr);

    bool available() const { return m_available; }
    bool learning()  const { return m_learning;  }

public slots:
    // Gửi lệnh IR từ database
    Q_INVOKABLE bool sendCommand(const IRCode &code);
    Q_INVOKABLE bool sendCommand(const IRDevice &device, IRCommand cmd);

    // Chế độ học mã từ remote thật
    Q_INVOKABLE void startLearning();
    Q_INVOKABLE void stopLearning();

    // Kiểm tra tần số hỗ trợ
    Q_INVOKABLE QList<int> supportedFrequencies() const;

signals:
    void availableChanged();
    void learningChanged();
    void commandSent(bool success);
    void codelearned(const IRCode &code);  // phát ra khi học xong

private:
    bool checkAvailability();
    bool transmit(int frequency, const QVector<int> &pattern);

    bool m_available = false;
    bool m_learning  = false;

    // Android JNI object
    QAndroidJniObject m_irManager;
};
