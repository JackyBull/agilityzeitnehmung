#-------------------------------------------------
#
# Project created by QtCreator 2019-10-02T11:56:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Zeitnehmung
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    worker.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    worker.h \
    about.h

#LIBS += libQt5SerialPort.a

FORMS    += mainwindow.ui \
    about.ui

CONFIG += serialport

QT += serialport

DISTFILES += \
    OpenItems
