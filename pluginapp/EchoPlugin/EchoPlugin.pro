TEMPLATE        = lib
CONFIG         += plugin
QT             += widgets
INCLUDEPATH    += ../MainWindow
TARGET          = $$qtLibraryTarget(echoplugin)
DESTDIR         = ../plugins


HEADERS += \
    XmlRead.h \
    echoplugin.h \
    imagedetectbase.h \
    imagegroupbase.h \
    test.h \
    tinyxml2.h \
    totalalg.h \
    totaltestwidget.h \
    totaltestwidgetr.h

SOURCES += \
    XmlRead.cpp \
    echoplugin.cpp \
    imagedetectbase.cpp \
    imagegroupbase.cpp \
    test.cpp \
    tinyxml2.cpp \
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
