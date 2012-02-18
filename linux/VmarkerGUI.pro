#-------------------------------------------------
#
# Project created by QtCreator 2011-10-15T17:16:53
#
#-------------------------------------------------

QT       += core gui

TARGET = VmarkerGUI
TEMPLATE = app


SOURCES += main.cpp\
        vmarkergui.cpp \
    vmarker.cpp \
    hidapi/hid-libusb.c

HEADERS  += vmarkergui.h \
    vmarker.h \
    hidapi/hidapi.h

FORMS    += vmarkergui.ui

LIBS    += `pkg-config libusb-1.0 libudev --libs`








