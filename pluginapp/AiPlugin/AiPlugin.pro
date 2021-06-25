TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets charts
INCLUDEPATH    += ../MainWindow
TARGET          = $$qtLibraryTarget(aiplugin)
DESTDIR         = ../plugins

INCLUDEPATH += $$PWD/../FrameWork
LIBS += $$PWD/../plugins/FrameWork.lib

include(../dependence/halcon/halcon.pri)

HEADERS += \
    WAiConstant.h \
    aidatapro.h \
    aiplugin.h \
    aiwidget.h \
    cgetimagewidget.h

SOURCES += \
    aidatapro.cpp \
    aiplugin.cpp \
    aiwidget.cpp \
    cgetimagewidget.cpp

FORMS += \
    aiwidget.ui \
    cgetimagewidget.ui
