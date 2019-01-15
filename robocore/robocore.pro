TEMPLATE = app
include(../libqtgpio/libqtgpio.pri)
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/gpioexamples/$${TARGET}
!isEmpty(target.path): INSTALLS += target

QT += mqtt

SOURCES += main.cpp \
    mqttclient.cpp \
    videostreamcontroller.cpp \
    qubotcorenode.cpp \
    qubotnodesmanager.cpp

HEADERS += \
    mqttclient.h \
    videostreamcontroller.h \
    qubotcorenode.h \
    qubotnodesmanager.h
