# 📺 TV Remote — Dual Protocol (Cast + DIAL)

App điều khiển Android TV / Smart TV qua WiFi, viết bằng **C++ + Qt/QML**.

## Tính năng
- 🔵 **Google Cast** — điều khiển playback real-time (play/pause/seek/volume/speed/quality)
- 🟣 **DIAL Protocol** — điều hướng YouTube (lên/xuống/trái/phải/chọn/home)
- ⚡ **Dual-Protocol** — cả 2 kết nối song song, không ngắt quãng
- 🔍 Tự quét TV trong mạng LAN (mDNS + SSDP)
- 📱 Tích hợp vào bàn phím Android

## Yêu cầu

| Thứ | Phiên bản |
|---|---|
| Qt | 6.5+ |
| CMake | 3.16+ |
| Android NDK | r25+ |
| Android SDK | API 26+ |
| Compiler | Clang (NDK) |

## Cài đặt Qt

1. Tải Qt Online Installer: https://www.qt.io/download-open-source
2. Cài các component:
   - Qt 6.5 for Android
   - Qt Quick
   - Android NDK & SDK (hoặc cài riêng qua Android Studio)

## Build cho Android

```bash
# Cấu hình
cmake -B build-android \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DQt6_DIR=$QT_DIR/android_arm64_v8a/lib/cmake/Qt6 \
  -DCMAKE_BUILD_TYPE=Release

# Build APK
cmake --build build-android --target apk
```

## Build cho Desktop (test UI)

```bash
cmake -B build-desktop
cmake --build build-desktop
./build-desktop/TVRemote
```

## Cấu trúc project

```
TVRemote/
├── CMakeLists.txt
├── src/
│   ├── main.cpp              # Entry point
│   ├── Device.h              # Struct thiết bị
│   ├── DeviceScanner.h/cpp   # Quét mạng (mDNS + SSDP)
│   ├── CastProtocol.h/cpp    # Google Cast TCP
│   ├── DialProtocol.h/cpp    # DIAL HTTP REST
│   └── RemoteController.h/cpp # Controller trung tâm
├── qml/
│   ├── main.qml              # Root + navigation
│   ├── ConnectScreen.qml     # Màn hình kết nối
│   ├── ConnectingScreen.qml  # Đang kết nối song song
│   ├── RemoteScreen.qml      # Remote chính
│   └── DPad.qml              # Vòng D-pad tròn
└── android/
    ├── AndroidManifest.xml
    └── res/values/styles.xml
```

## TV được hỗ trợ

| TV | Cast | DIAL |
|---|---|---|
| Android TV / Google TV | ✅ | ✅ |
| Chromecast | ✅ | ✅ |
| Samsung Tizen | ❌ | ✅ |
| LG webOS | ❌ | ✅ |
| Sony (Android) | ✅ | ✅ |
| Xiaomi Mi Box | ✅ | ✅ |

## Lưu ý
- TV và điện thoại phải **cùng mạng WiFi**
- Cast dùng port **8009** (TLS) — production cần QSslSocket
- DIAL dùng port **56790** (HTTP)
- Mock devices được bật sẵn trong DeviceScanner để test UI
