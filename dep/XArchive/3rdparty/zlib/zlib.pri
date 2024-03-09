INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

win32-g++ {
    LIBS += $$PWD/libs/libzlib-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/libs/zlib-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/libs/libzlib-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/libs/libzlib-macos-$${QT_ARCH}.a
}
