#-------------------------------------------------
#
# Project created by QtCreator 2011-10-21T18:27:45
#
#-------------------------------------------------

QT       += core gui webkit sql network xml

TARGET = Larss
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    feedmodel.cpp \
    rssparser.cpp \
    feedpoller.cpp

HEADERS  += mainwindow.h \
    feedmodel.h \
    rssparser.h \
    feedpoller.h

FORMS    += mainwindow.ui
