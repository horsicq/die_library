QT       -= core gui

TARGET = lzfse
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/lzfse
DEPENDPATH += $$PWD/lzfse

include(../../build.pri)

CONFIG(debug, debug|release) {
    TARGET = lzfsed
} else {
    TARGET = lzfse
}

TARGETLIB_PATH = $$PWD

win32-g++ {
    contains(QT_ARCH, i386) {
        DESTDIR=$${TARGETLIB_PATH}/libs/win32-g++
    } else {
        DESTDIR=$${TARGETLIB_PATH}/libs/win64-g++
    }
}
win32-msvc* {
    contains(QMAKE_TARGET.arch, x86_64) {
        DESTDIR=$${TARGETLIB_PATH}/libs/win64-msvc
    } else {
        DESTDIR=$${TARGETLIB_PATH}/libs/win32-msvc
    }
}
unix:!macx {
    BITSIZE = $$system(getconf LONG_BIT)
    if (contains(BITSIZE, 64)) {
        DESTDIR=$${TARGETLIB_PATH}/libs/lin64
    }
    if (contains(BITSIZE, 32)) {
        DESTDIR=$${TARGETLIB_PATH}/libs/lin32
    }
}
unix:macx {
    DESTDIR=$${TARGETLIB_PATH}/libs/mac
}

SOURCES += \
    $$PWD/src/lzfse_decode.c \
    $$PWD/src/lzfse_decode_base.c \
    $$PWD/src/lzfse_encode.c \
    $$PWD/src/lzfse_encode_base.c \
    $$PWD/src/lzfse_fse.c \
    $$PWD/src/lzvn_decode_base.c \
    $$PWD/src/lzvn_encode_base.c 