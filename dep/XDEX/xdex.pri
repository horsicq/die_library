INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xandroidbinary.h \
    $$PWD/xandroidbinary_def.h \
    $$PWD/xdex.h \
    $$PWD/xdex_def.h

SOURCES += \
    $$PWD/xandroidbinary.cpp \
    $$PWD/xdex.cpp

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/../Formats/xbinary.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xdex.cmake
