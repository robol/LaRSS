#-------------------------------------------------
#
# Project created by QtCreator 2011-10-21T18:27:45
#
#-------------------------------------------------

QT       += core gui webkit sql network xml

TARGET = Larss
TEMPLATE = app


SOURCES += larss/main.cpp\
        larss/mainwindow.cpp \
    larss/feedmodel.cpp \
    larss/rssparser.cpp \
    larss/feedpoller.cpp \
    larss/editfeeddialog.cpp \
    larss/editcategorydialog.cpp

HEADERS  += include/mainwindow.h \
    include/feedmodel.h \
    include/rssparser.h \
    include/feedpoller.h \
    include/editfeeddialog.h \
    include/editcategorydialog.h

FORMS    += ui/mainwindow.ui \
    ui/editfeeddialog.ui \
    ui/editcategorydialog.ui

INCLUDEPATH += ./include
