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
    Client/main.cpp \
    Client/helicalmainwindow.cpp \
    Client/helicalserverconnectionsdialog.cpp \
    Client/helicalterminaldialog.cpp \
    Client/helicalserverdetailsdialog.cpp \
    QtSSH/qtssh.cpp \
    QtSSH/qtsshchannel.cpp \
    QtTerminal/cterminal.cpp \
    QtTerminal/qtterminal.cpp \
    Client/helicalsftpdialog.cpp \
    QtSSH/qtsftp.cpp \
    Client/helicalfiletransfertask.cpp \
    Client/helicalserverconnectionsettings.cpp \
    Client/helicalfiletransfercontroller.cpp

HEADERS += \
    QtSSH/qtssh.h \
    QtSSH/qtsshchannel.h \
    Client/helicalmainwindow.h \
    Client/helicalserverconnectionsdialog.h \
    Client/helicalterminaldialog.h \
    Client/helicalserverdetailsdialog.h \
    QtTerminal/cterminal.h \
    QtTerminal/qtterminal.h \
    Client/helicalsftpdialog.h \
    QtSSH/qtsftp.h \
    Client/helicalfiletransfertask.h \
    Client/helicalserverconnectionsettings.h \
    Client/helicalfiletransfercontroller.h \
    Client/helical.h

FORMS += \
    Client/heilcalmainwindow.ui \
    Client/helicalserverconnectionsdialog.ui \
    Client/helicalterminaldialog.ui \
    Client/helicalserverdetailsdialog.ui \
    Client/helicalsftpdialog.ui

INCLUDEPATH += $$PWD/../../NetBeansProjects/Antikythera_mechanism/include
DEPENDPATH += $$PWD/../../NetBeansProjects/Antikythera_mechanism/include

unix: LIBS += -L/usr/local/lib -lboost_system
unix: LIBS += -L$$PWD/../../NetBeansProjects/Antik_Static_Library/dist/Debug/GNU-Linux/ -lantik_static_library

unix: PRE_TARGETDEPS += $$PWD/../../NetBeansProjects/Antik_Static_Library/dist/Debug/GNU-Linux/libantik_static_library.a

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libssh
unix: PKGCONFIG += libssh_threads

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -lboost_filesystem
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -lboost_filesystem
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -lboost_filesystem

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/release/libboost_filesystem.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/debug/libboost_filesystem.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/release/boost_filesystem.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/debug/boost_filesystem.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../../../usr/local/lib/libboost_filesystem.a
