INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src
INCLUDEPATH += $$PWD/src/include
DEPENDPATH += $$PWD/src/include

win32-g++ {
    LIBS += $$PWD/libs/libcapstone_x86-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/libs/capstone_x86-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/libs/libcapstone_x86-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/libs/libcapstone_x86-macos-$${QT_ARCH}.a
}
