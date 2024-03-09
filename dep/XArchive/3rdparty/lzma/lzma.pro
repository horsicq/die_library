QT       -= core gui

TARGET = lzma
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/lzma
DEPENDPATH += $$PWD/lzma

include(../../build.pri)

win32{
    TARGET = lzma-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = lzma-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = lzma-macos-$${QT_ARCH}
}

SOURCES += \
    $$PWD/src/LzmaDec.c \
    $$PWD/src/Ppmd7.c \
    $$PWD/src/Ppmd7Dec.c

TARGETLIB_PATH = $$PWD

DESTDIR=$${TARGETLIB_PATH}/libs
