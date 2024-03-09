INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

win32-g++ {
    LIBS += $$PWD/libs/liblzma-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/libs/lzma-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/libs/liblzma-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/libs/liblzma-macos-$${QT_ARCH}.a
}
