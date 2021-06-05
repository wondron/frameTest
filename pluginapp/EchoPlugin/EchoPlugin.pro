TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../MainWindow
TARGET          = $$qtLibraryTarget(echoplugin)
DESTDIR         = ../plugins


HEADERS += \
    echoplugin.h \
    imagedetectbase.h \
    imagegroupbase.h \
    test.h \
    totalalg.h \
    totaltestwidget.h \
    totaltestwidgetr.h

SOURCES += \
    echoplugin.cpp \
    imagedetectbase.cpp \
    imagegroupbase.cpp \
    test.cpp \
    totalalg.cpp \
    totaltestwidget.cpp \
    totaltestwidgetr.cpp

FORMS += \
    imagedetectbase.ui \
    test.ui \
    totalalg.ui \
    totaltestwidget.ui \
    totaltestwidgetr.ui

INCLUDEPATH += $$PWD/../FrameWork
LIBS += $$PWD/../plugins/FrameWork.lib

message($$PWD)

include(../dependence/halcon/halcon.pri)
include(reverseSide/reverseSide.pri)
include(obverseSide/obverseSide.pri)
include(commonUsage/commonUsage.pri)

message(Plugins: $$[QT_INSTALL_PLUGINS])
