cmake_minimum_required(VERSION 3.16)
project(TVRemote VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Network)

qt_standard_project_setup()

qt_add_executable(TVRemote
    src/main.cpp
    src/DeviceScanner.cpp
    src/DeviceScanner.h
    src/CastProtocol.cpp
    src/CastProtocol.h
    src/DialProtocol.cpp
    src/DialProtocol.h
    src/RemoteController.cpp
    src/RemoteController.h
    src/ir/IRDevice.h
    src/ir/IRDatabase.h
    src/ir/IRDatabase.cpp
    src/ir/IRBlaster.h
    src/ir/IRBlaster.cpp
    src/ir/IRController.h
    src/ir/IRController.cpp
    src/lounge/LoungeDevice.h
    src/lounge/LoungeScanner.h
    src/lounge/LoungeScanner.cpp
    src/lounge/LoungeProtocol.h
    src/lounge/LoungeProtocol.cpp
    src/Device.h
)

qt_add_qml_module(TVRemote
    URI TVRemote
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/ConnectScreen.qml
        qml/ConnectingScreen.qml
        qml/RemoteScreen.qml
        qml/DPad.qml
        qml/IRRemoteScreen.qml
        qml/LoungeScreen.qml
        qml/PlaybackBar.qml
        qml/components/ProtoTag.qml
        qml/components/DeviceCard.qml
)

target_link_libraries(TVRemote PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Network
)

# Android settings
if(ANDROID)
    set_target_properties(TVRemote PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
        QT_ANDROID_TARGET_SDK_VERSION 33
        QT_ANDROID_MIN_SDK_VERSION 26
    )
endif()
