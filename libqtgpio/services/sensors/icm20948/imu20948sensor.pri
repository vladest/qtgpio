QT -= gui
QT += core
CONFIG += c++17

INCLUDEPATH += $$PWD $$PWD/util

SOURCES += \
        $$PWD/imu20948sensor.cpp \
        $$PWD/ICM_20948.cpp \
        $$PWD/util/*.c
HEADERS += \
        $$PWD/imu20948sensor.h \
        $$PWD/ICM_20948.h \
        $$PWD/util/*.h
