CONFIG += c++17

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/rpicpuinfo.cpp \
    $$PWD/servocontrol.cpp \
    $$PWD/rpi/bcm2835.c

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioport.h \
    $$PWD/rpicpuinfo.h \
    $$PWD/servocontrol.h \
    $$PWD/rpi/bcm2835.h
