#pragma once
#include <QObject>
#include <QList>
#include <QJsonDocument>
#include <QFile>
#include "IRDevice.h"

// IRDatabase — quản lý toàn bộ mã IR
// Nguồn: LIRC database + IRdb + tự học
class IRDatabase : public QObject {
    Q_OBJECT

public:
    explicit IRDatabase(QObject *parent = nullptr);

    // Tìm thiết bị theo hãng
    QList<IRDevice> findByBrand(const QString &brand) const;

    // Tìm thiết bị tốt nhất cho hãng TV
    IRDevice bestMatch(const QString &brand) const;

    // Lấy danh sách hãng có trong DB
    QStringList availableBrands() const;

    // Lưu / load thiết bị đã học
    void saveLearned(const IRDevice &device);
    QList<IRDevice> loadLearned() const;

    // Tải DB từ assets
    bool loadFromFile(const QString &path);

    static IRDatabase* instance();

private:
    void loadBuiltinCodes();
    IRDevice buildSamsung();
    IRDevice buildLG();
    IRDevice buildSony();
    IRDevice buildToshiba();
    IRDevice buildPhilips();
    IRDevice buildSharp();
    IRDevice buildTCL();
    IRDevice buildXiaomi();

    QList<IRDevice>  m_devices;
    QList<IRDevice>  m_learned;
    static IRDatabase* s_instance;
};
