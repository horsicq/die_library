INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xformats.h

SOURCES += \
    $$PWD/xformats.cpp

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/xbinary.pri)
}

!contains(XCONFIG, xcom) {
    XCONFIG += xcom
    include($$PWD/xcom.pri)
}

!contains(XCONFIG, xmsdos) {
    XCONFIG += xmsdos
    include($$PWD/xmsdos.pri)
}

!contains(XCONFIG, xne) {
    XCONFIG += xne
    include($$PWD/xne.pri)
}

!contains(XCONFIG, xle) {
    XCONFIG += xle
    include($$PWD/xle.pri)
}

!contains(XCONFIG, xpe) {
    XCONFIG += xpe
    include($$PWD/xpe.pri)
}

!contains(XCONFIG, xmach) {
    XCONFIG += xmach
    include($$PWD/xmach.pri)
}

!contains(XCONFIG, xelf) {
    XCONFIG += xelf
    include($$PWD/xelf.pri)
}

contains(XCONFIG, use_dex) {
    DEFINES += USE_DEX
    !contains(XCONFIG, xdex) {
        XCONFIG += xdex
        include($$PWD/../XDEX/xdex.pri)
    }
}

contains(XCONFIG, use_pdf) {
    DEFINES += USE_PDF
    !contains(XCONFIG, xpdf) {
        XCONFIG += xpdf
        include($$PWD/../XPDF/xpdf.pri)
    }
}

contains(XCONFIG, use_archive) {
    DEFINES += USE_ARCHIVE
    !contains(XCONFIG, xarchives) {
        XCONFIG += xarchives
        include($$PWD/../XArchive/xarchives.pri)
    }
}

!contains(XCONFIG, xicon) {
    XCONFIG += xicon
    include($$PWD/images/xicon.pri)
}

!contains(XCONFIG, xgif) {
    XCONFIG += xgif
    include($$PWD/images/xgif.pri)
}

!contains(XCONFIG, xbmp) {
    XCONFIG += xbmp
    include($$PWD/images/xbmp.pri)
}

!contains(XCONFIG, xpng) {
    XCONFIG += xpng
    include($$PWD/images/xpng.pri)
}

!contains(XCONFIG, xjpeg) {
    XCONFIG += xjpeg
    include($$PWD/images/xjpeg.pri)
}

!contains(XCONFIG, xtiff) {
    XCONFIG += xtiff
    include($$PWD/images/xtiff.pri)
}

!contains(XCONFIG, xmp4) {
    XCONFIG += xmp4
    include($$PWD/video/xmp4.pri)
}

!contains(XCONFIG, xriff) {
    XCONFIG += xriff
    include($$PWD/video/xriff.pri)
}

!contains(XCONFIG, xmp3) {
    XCONFIG += xmp3
    include($$PWD/audio/xmp3.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xformats.cmake
