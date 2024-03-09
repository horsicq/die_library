INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

win32-g++ {
    LIBS += $$PWD/libs/libbzip2-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/libs/bzip2-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/libs/libbzip2-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/libs/libbzip2-macos-$${QT_ARCH}.a
}
