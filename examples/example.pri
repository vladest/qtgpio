QT -= gui
QT += core
CONFIG += c++17 console
CONFIG -= app_bundle
MOC_DIR = .moc
OBJECTS_DIR = .obj

message($$PWD)
QTGPIO_PATH = $$PWD/../libqtgpio
message($$QTGPIO_PATH)
INCLUDEPATH += $$PWD $$QTGPIO_PATH $$QTGPIO_PATH/rpi $$QTGPIO_PATH/hats $$QTGPIO_PATH/services $$QTGPIO_PATH/displays

LIBS += -L$$QTGPIO_PATH -lqgpio
PRE_TARGETDEPS += $$QTGPIO_PATH/libqgpio.a

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/gpioexamples/$${TARGET}
!isEmpty(target.path): INSTALLS += target
