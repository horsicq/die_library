INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/scanitem.h \
    $$PWD/scanitemmodel.h

SOURCES += \
    $$PWD/scanitem.cpp \
    $$PWD/scanitemmodel.cpp

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/xbinary.pri)
}

DISTFILES += \
    $$PWD/scanitem.cmake
