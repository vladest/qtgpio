CONFIG += c++17

INCLUDEPATH += $$PWD/modules

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/rpicpuinfo.cpp \
    $$PWD/servocontrol.cpp \
    $$PWD/rpi/bcm2835.c \
    $$PWD/modules/pca9685/pca9685.cpp \
    $$PWD/pwmsoftware.cpp \
    $$PWD/pwmpca9685.cpp

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioport.h \
    $$PWD/rpicpuinfo.h \
    $$PWD/servocontrol.h \
    $$PWD/rpi/bcm2835.h \
    $$PWD/modules/pca9685/pca9685.h \
    $$PWD/pwmabstract.h \
    $$PWD/pwmsoftware.h \
    $$PWD/pwmpca9685.h
