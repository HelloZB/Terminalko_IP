#-------------------------------------------------
#
# Project created by QtCreator 2013-05-12T17:19:38
#
#-------------------------------------------------

QT       += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Terminalko_IP
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

RESOURCES += \
    res.qrc

RC_FILE += file.rc
OTHER_FILES += file.rc
