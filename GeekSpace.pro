#-------------------------------------------------
#
# Project created by QtCreator 2017-07-11T09:05:49
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -std=c++11
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GeekSpace
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        geekspacewindow.cpp \
    gscallfunctionmessage.cpp \
    highlighter.cpp \
    gstextedit.cpp \
    gsmodule.cpp \
    gsabstractmessage.cpp \
    gsmodulesearcher.cpp \
    gsaddmoduledialog.cpp \
    gsobject.cpp \
    gstimer.cpp \
    gsaddshelduledialog.cpp \
    gsaddtimerdialog.cpp \
    gssheldule.cpp \
    gsprocess.cpp \
    gsaddprocessdialog.cpp \
    gsscript.cpp \
    gssavescriptdialog.cpp \
    gsscripteditor.cpp

HEADERS  += geekspacewindow.h \
    gsmessages.h \
    gscallfunctionmessage.h \
    highlighter.h \
    gstextedit.h \
    gsmodule.h \
    gsabstractmessage.h \
    gsmodulesearcher.h \
    gsaddmoduledialog.h \
    gsobject.h \
    gstimer.h \
    gsobjects.h \
    gsaddshelduledialog.h \
    gsaddtimerdialog.h \
    gssheldule.h \
    gsprocess.h \
    gsaddprocessdialog.h \
    gsscript.h \
    gssavescriptdialog.h \
    gsscripteditor.h

FORMS    += geekspacewindow.ui \
    gsappendmoduledialog.ui \
    gsaddshelduledialog.ui \
    gsaddtimerdialog.ui \
    gssavescriptdialog.ui \
    gsaddprocessdialog.ui \
    gsscripteditor.ui

RESOURCES += \
    images.qrc

STATECHARTS +=

DISTFILES +=
