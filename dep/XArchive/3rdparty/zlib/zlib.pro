QT       -= core gui

TARGET = zlib
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/zlib
DEPENDPATH += $$PWD/zlib

include(../../build.pri)

win32{
    TARGET = zlib-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = zlib-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = zlib-macos-$${QT_ARCH}
}

SOURCES += \
    $$PWD/src/deflate.c \
    $$PWD/src/inflate.c \
    $$PWD/src/adler32.c \
    $$PWD/src/crc32.c \
    $$PWD/src/inffast.c \
    $$PWD/src/inftrees.c \
    $$PWD/src/trees.c \
    $$PWD/src/zutil.c

TARGETLIB_PATH = $$PWD

DESTDIR=$${TARGETLIB_PATH}/libs