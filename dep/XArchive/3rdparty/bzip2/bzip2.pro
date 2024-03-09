QT       -= core gui

TARGET = bzip2
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/bzip2
DEPENDPATH += $$PWD/bzip2

include(../../build.pri)

win32{
    TARGET = bzip2-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = bzip2-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = bzip2-macos-$${QT_ARCH}
}

SOURCES += \
    $$PWD/src/bzip2.c \
    $$PWD/src/crctable.c \
    $$PWD/src/bzlib.c \
    $$PWD/src/compress.c \
    $$PWD/src/decompress.c \
    $$PWD/src/blocksort.c \
    $$PWD/src/randtable.c \
    $$PWD/src/huffman.c

TARGETLIB_PATH = $$PWD

DESTDIR=$${TARGETLIB_PATH}/libs
