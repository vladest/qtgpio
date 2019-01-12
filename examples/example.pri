QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

include(../libqtgpio/libqtgpio.pri)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/gpioexamples/$${TARGET}
!isEmpty(target.path): INSTALLS += target
