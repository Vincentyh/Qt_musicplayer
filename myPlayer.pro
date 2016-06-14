#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T16:52:08
#
#-------------------------------------------------

QT       += core gui

TARGET = myPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mywidget.cpp \
    myplaylist.cpp

HEADERS  += mywidget.h \
    myplaylist.h

FORMS    += mywidget.ui

RESOURCES +=

QT += phonon
