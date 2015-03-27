#-------------------------------------------------
#
# Project created by QtCreator 2014-10-29T06:30:32
#
#-------------------------------------------------

QT       += core gui multimedia serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player-flat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fft.cpp \
    controls.cpp \
    fftcalc.cpp \
    mediainfo.cpp \
    playlistmodel.cpp \
    mydisplay.cpp

HEADERS  += mainwindow.h \
    fft.h \
    controls.h \
    fftcalc.h \
    mediainfo.h \
    abstractcontrol.h \
    abstractspectrograph.h \
    abstractmediainfo.h \
    playlistmodel.h \
    mydisplay.h
   fft.h

FORMS    += mainwindow.ui \
    controls.ui \
    mediainfo.ui

RESOURCES += \
    resources.qrc
