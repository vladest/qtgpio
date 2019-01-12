CONFIG += c++14

INCLUDEPATH += $$PWD $$PWD/rpi $$PWD/hats $$PWD/services

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/rpi/rpicpuinfo.cpp \
    $$PWD/rpi/bcm2835.c \
    $$PWD/hats/pca9685/pca9685.cpp \
    $$PWD/services/pwm/pwmsoftware.cpp \
    $$PWD/services/pwm/pwmpca9685.cpp \
    $$PWD/services/servo/servocontrol9685.cpp \
    $$PWD/services/servo/servobase.cpp \
    $$PWD/services/servo/servosoftware.cpp \
    $$PWD/services/sensors/hcsr04sensor.cpp

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioport.h \
    $$PWD/rpi/rpicpuinfo.h \
    $$PWD/rpi/bcm2835.h \
    $$PWD/hats/pca9685/pca9685.h \
    $$PWD/services/pwm/pwmsoftware.h \
    $$PWD/services/pwm/pwmpca9685.h \
    $$PWD/services/servo/servocontrol9685.h \
    $$PWD/services/servo/servobase.h \
    $$PWD/services/servo/servosoftware.h \
    $$PWD/services/pwm/pwmbase.h \
    $$PWD/services/sensors/hcsr04sensor.h
