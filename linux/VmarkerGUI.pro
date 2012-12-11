# VmarkerGUI by Robin Theunis 
# 2011-2012 
# For use with the vmarker IR sensor

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.

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




CONFIG += qt warn_on static



