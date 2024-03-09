INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

win32-g++ {
    contains(QT_ARCH, i386) {
        LIBS += $$PWD/libs/win32-g++/liblzfse.a
    } else {
        LIBS += $$PWD/libs/win64-g++/liblzfse.a
    }
}
win32-msvc* {
    contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += $$PWD/libs/win64-msvc/lzfse.lib
    } else {
        LIBS += $$PWD/libs/win32-msvc/lzfse.lib
    }
}
unix:!macx {
    BITSIZE = $$system(getconf LONG_BIT)
    if (contains(BITSIZE, 64)) {
        LIBS +=  $$PWD/libs/lin64/liblzfse.a
    }
    if (contains(BITSIZE, 32)) {
        LIBS +=  $$PWD/libs/lin32/liblzfse.a
    }
}
unix:macx {
    LIBS +=  $$PWD/libs/mac/liblzfse.a
}
