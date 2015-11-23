#-------------------------------------------------
#
# Project created by QtCreator 2015-11-12T10:46:21
#
#-------------------------------------------------

QT       += core gui sql script network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FruttecSQL
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    json.cpp \
    fruttecget.cpp \
    fruttecset.cpp \
    fruttecprocess.cpp

HEADERS  += mainwindow.h \
    json.h \
    fruttecget.h \
    fruttecset.h \
    fruttecprocess.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
