QT += core testlib

CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    TestController.cpp \
    TacticalVehicleController.cpp \
    TacticalVehicleData.cpp

HEADERS += \
    TacticalVehicleController.h \
    TacticalVehicleData.h \
    TacticalVehicle.h

RESOURCES += \
    resources.qrc
