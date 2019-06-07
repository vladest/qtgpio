CONFIG += c++14

INCLUDEPATH += $$PWD $$PWD/rpi $$PWD/hats $$PWD/services

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioi2cslave.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/rpi/rpicpuinfo.cpp \
    $$PWD/rpi/bcm2835.c \
    $$PWD/hats/pca9685/pca9685.cpp \
    $$PWD/hats/ultraborg/ultraborg.cpp \
    $$PWD/services/motor/motor_driver_hat.cpp \
    $$PWD/services/motor/motor_hat_stepper.cpp \
    $$PWD/services/pwm/pwmsoftware.cpp \
    $$PWD/services/pwm/pwmpca9685.cpp \
    $$PWD/services/pwm/pwmultraborg.cpp \
    $$PWD/services/sensors/octosonarsensor.cpp \
    $$PWD/services/sensors/ultraborgsensor.cpp \
    $$PWD/services/servo/servocontrol9685.cpp \
    $$PWD/services/servo/servobase.cpp \
    $$PWD/services/servo/servocontrolultraborg.cpp \
    $$PWD/services/servo/servosoftware.cpp \
    $$PWD/services/sensors/hcsr04sensor.cpp \
    $$PWD/services/motor/motorbase.cpp \
    $$PWD/services/motor/motor_driver_board.cpp

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioi2cslave.h \
    $$PWD/qgpioport.h \
    $$PWD/rpi/rpicpuinfo.h \
    $$PWD/rpi/bcm2835.h \
    $$PWD/hats/pca9685/pca9685.h \
    $$PWD/hats/ultraborg/ultraborg.h \
    $$PWD/services/motor/motor_driver_hat.h \
    $$PWD/services/motor/motor_hat_stepper.h \
    $$PWD/services/pwm/pwmsoftware.h \
    $$PWD/services/pwm/pwmpca9685.h \
    $$PWD/services/pwm/pwmultraborg.h \
    $$PWD/services/sensors/octosonarsensor.h \
    $$PWD/services/sensors/ultraborgsensor.h \
    $$PWD/services/servo/servocontrol9685.h \
    $$PWD/services/servo/servobase.h \
    $$PWD/services/servo/servocontrolultraborg.h \
    $$PWD/services/servo/servosoftware.h \
    $$PWD/services/pwm/pwmbase.h \
    $$PWD/services/sensors/hcsr04sensor.h \
    $$PWD/services/motor/motorbase.h \
    $$PWD/services/motor/motor_driver_board.h
