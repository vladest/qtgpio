TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS = examples
include(libqtgpio/libqtgpio.pri)
include(examples/examples.pro)
