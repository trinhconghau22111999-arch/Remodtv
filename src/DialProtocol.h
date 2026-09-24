#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "Device.h"

// DialProtocol điều khiển app trên TV qua HTTP REST
// Spec: https://www.dial-multiscreen.org/
class DialProtocol : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString state   READ state    NOTIFY stateChanged)

public:
    explicit DialProtocol(QObject *parent = nullptr);
    ~DialProtocol();

    bool    connected() const { return m_connected; }
    QString state()     const { return m_state; }

public slots:
    void connectToDevice(const QString &ip, int port = 56790);
    void disconnect();

    // DIAL commands
    void launchYouTube();
    void stopYouTube();
    void getAppState();

    // YouTube-over-DIAL (dùng loungeToken để điều khiển)
    void navigateUp();
    void navigateDown();
    void navigateLeft();
    void navigateRight();
    void selectOk();
    void goBack();
    void goHome();

signals:
    void connectedChanged();
    void stateChanged();
    void appStateReceived(const QString &state);
    void error(const QString &message);

private slots:
    void onAppStateReply(QNetworkReply *reply);
    void pollAppState();

private:
    QString baseUrl() const {
        return QString("http://%1:%2").arg(m_ip).arg(m_port);
    }
    void sendRequest(const QString &path, const QByteArray &method = "GET",
                     const QByteArray &body = {});

    QNetworkAccessManager *m_nam    = nullptr;
    QTimer                *m_poll   = nullptr;
    QString                m_ip;
    int                    m_port   = 56790;
    bool                   m_connected = false;
    QString                m_state  = "disconnected";
    QString                m_loungeToken;

    static constexpr int POLL_MS = 3000;
    static constexpr auto YT_APP = "YouTube";
};
