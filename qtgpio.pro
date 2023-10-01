TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS = libqtgpio examples

plugin.depends = libqtgpio
import.depends = libqtgpio

