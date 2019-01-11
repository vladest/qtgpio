CONFIG += c++17

INCLUDEPATH += $$PWD/modules

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/rpicpuinfo.cpp \
    $$PWD/rpi/bcm2835.c \
    $$PWD/modules/pca9685/pca9685.cpp \
    $$PWD/pwmsoftware.cpp \
    $$PWD/pwmpca9685.cpp \
    $$PWD/servocontrol9685.cpp \
    $$PWD/servobase.cpp \
    $$PWD/servosoftware.cpp

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioport.h \
    $$PWD/rpicpuinfo.h \
    $$PWD/rpi/bcm2835.h \
    $$PWD/modules/pca9685/pca9685.h \
    $$PWD/pwmsoftware.h \
    $$PWD/pwmpca9685.h \
    $$PWD/servocontrol9685.h \
    $$PWD/servobase.h \
    $$PWD/servosoftware.h \
    $$PWD/pwmbase.h
