INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xarchives.h

SOURCES += \
    $$PWD/xarchives.cpp

!contains(XCONFIG, xarchive) {
    XCONFIG += xarchive
    include(xarchive.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xarchives.cmake
