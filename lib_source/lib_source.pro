QT -= gui

TEMPLATE = lib
DEFINES += LIB_SOURCE_LIBRARY
CONFIG -= staticlib
CONFIG += dll

CONFIG += c++11
TARGET = die

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    die_lib.cpp

HEADERS += \
    die_lib.h

include(../build.pri)

!contains(XCONFIG, die_script) {
    XCONFIG += die_script
    include(../die_script/die_script.pri)
}
