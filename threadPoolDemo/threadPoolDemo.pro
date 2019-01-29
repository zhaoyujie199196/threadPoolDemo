#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T14:40:41
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = threadPoolDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    threadqueue.cpp \
    threadpool.cpp

HEADERS  += mainwindow.h \
    threadqueue.h \
    threadpool.h

FORMS    += mainwindow.ui
