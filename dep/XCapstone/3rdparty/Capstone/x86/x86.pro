#-------------------------------------------------
#
# Project created by QtCreator 2019-02-01T10:50:10
#
#-------------------------------------------------

QT       -= core gui

TARGET = capstone
TEMPLATE = lib
CONFIG += staticlib

include(../../../build.pri)

CONFIG(debug, debug|release) {
    TARGET = capstone_x86d
} else {
    TARGET = capstone_x86
}

win32{
    TARGET = capstone_x86-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = capstone_x86-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = capstone_x86-macos-$${QT_ARCH}
}

win32 {
    DEFINES += "WIN32"
}

CONFIG += c++11

DEFINES += "CAPSTONE_HAS_X86"

DEFINES += "CAPSTONE_X86_ATT_DISABLE"
DEFINES += "CAPSTONE_DIET_NO"
DEFINES += "CAPSTONE_X86_REDUCE"
DEFINES += "CAPSTONE_USE_SYS_DYN_MEM"
DEFINES += "_LIB"

INCLUDEPATH += $$PWD/../src/
DEPENDPATH += $$PWD/../src/

SOURCES += \
    ../src/MCInst.c \
    ../src/MCInstrDesc.c \
    ../src/MCRegisterInfo.c \
    ../src/SStream.c \
    ../src/arch/X86/X86ATTInstPrinter.c \
    ../src/arch/X86/X86Disassembler.c \
    ../src/arch/X86/X86DisassemblerDecoder.c \
    ../src/arch/X86/X86InstPrinterCommon.c \
    ../src/arch/X86/X86IntelInstPrinter.c \
    ../src/arch/X86/X86Mapping.c \
    ../src/arch/X86/X86Module.c \
    ../src/cs.c \
    ../src/utils.c

TARGETLIB_PATH = $$PWD/../

DESTDIR=$${TARGETLIB_PATH}/libs
