QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

DESTDIR = ../plugins

qtHaveModule(printsupport): QT += printsupport
qtHaveModule(opengl): QT += opengl


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(graphics/graphics.pri)
include(halconGraphic/halconGraphic.pri)
include(commonWidget/commonWidget.pri)
include(utils/utils.pri)

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

HEADERS +=

SOURCES +=

DISTFILES +=
