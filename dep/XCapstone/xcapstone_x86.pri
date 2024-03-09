INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xcapstone.h

SOURCES += \
    $$PWD/xcapstone.cpp

!contains(XCONFIG, Capstone) {
    XCONFIG += Capstone
    include($$PWD/3rdparty/Capstone/Capstone_x86.pri)
}

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/../Formats/xbinary.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xcapstone.cmake
