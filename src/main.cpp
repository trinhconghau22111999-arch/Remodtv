#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "DeviceScanner.h"
#include "RemoteController.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("TV Remote");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("TVRemote");

    // Đăng ký C++ types với QML
    qmlRegisterType<DeviceScanner>   ("TVRemote", 1, 0, "DeviceScanner");
    qmlRegisterType<RemoteController>("TVRemote", 1, 0, "RemoteController");

    QQmlApplicationEngine engine;

    // Global instances
    DeviceScanner    scanner;
    RemoteController remote;

    engine.rootContext()->setContextProperty("scanner", &scanner);
    engine.rootContext()->setContextProperty("remote",  &remote);

    const QUrl url(u"qrc:/TVRemote/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    return app.exec();
}
