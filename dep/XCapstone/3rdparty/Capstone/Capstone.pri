INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/include
DEPENDPATH += $$PWD/src/include

win32-g++ {
    LIBS += $$PWD/libs/libcapstone-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/libs/capstone-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/libs/libcapstone-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/libs/libcapstone-macos-$${QT_ARCH}.a
}
