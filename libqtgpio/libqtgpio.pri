CONFIG += c++17

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/rpicpuinfo.cpp \
    $$PWD/servocontrol.cpp \
    $$PWD/rpi/bcm2835.c \
    $$PWD/modules/pca9685/pca9685.cpp \
    $$PWD/pwmsoftware.cpp

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioport.h \
    $$PWD/rpicpuinfo.h \
    $$PWD/servocontrol.h \
    $$PWD/rpi/bcm2835.h \
    $$PWD/modules/pca9685/pca9685.h \
    $$PWD/pwmabstract.h \
    $$PWD/pwmsoftware.h
