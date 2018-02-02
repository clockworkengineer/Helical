#-------------------------------------------------
#
# Project created by QtCreator 2018-01-25T10:55:07
#
#-------------------------------------------------

QT += core
QT += network
QT += gui

CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Helical
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    QtSSH/qtssh.cpp \
    QtSSH/qtsshchannel.cpp \
    helicalmainwindow.cpp \
    helicalserverconnectionsdialog.cpp \
    helicalterminaldialog.cpp \
    helicalserverdetailsdialog.cpp \
    QtTerminal/qtterminaltext.cpp

HEADERS += \
    QtSSH/qtssh.h \
    QtSSH/qtsshchannel.h \
    qtterminaltext.h \
    helicalmainwindow.h \
    helicalserverconnectionsdialog.h \
    helicalterminaldialog.h \
    helicalserverdetailsdialog.h \
    QtTerminal/qtterminaltext.h

FORMS += \
        heilcalmainwindow.ui \
    helicalserverconnectionsdialog.ui \
    helicalterminaldialog.ui \
    helicalserverdetailsdialog.ui

INCLUDEPATH += $$PWD/../../NetBeansProjects/Antikythera_mechanism/include
DEPENDPATH += $$PWD/../../NetBeansProjects/Antikythera_mechanism/include

unix: LIBS += -L/usr/local/lib -lboost_system
unix: LIBS += -L$$PWD/../../NetBeansProjects/Antik_Static_Library/dist/Debug/GNU-Linux/ -lantik_static_library

unix: PRE_TARGETDEPS += $$PWD/../../NetBeansProjects/Antik_Static_Library/dist/Debug/GNU-Linux/libantik_static_library.a

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libssh
unix: PKGCONFIG += libssh_threads

