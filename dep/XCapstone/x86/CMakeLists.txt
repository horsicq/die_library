cmake_minimum_required(VERSION 3.14)

project(capstone_x86 LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(${PROJECT_SOURCE_DIR})

if(WIN32)
  add_definitions(-DWIN32)
endif()

add_definitions(
    -DCAPSTONE_HAS_X86
    -DCAPSTONE_X86_ATT_DISABLE
    -DCAPSTONE_X86_REDUCE
    -DCAPSTONE_DIET_NO
    -DCAPSTONE_USE_SYS_DYN_MEM
    -D_LIB
    )
    
include_directories(${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/include)

add_library(capstone_x86 STATIC
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/MCInst.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/MCInstrDesc.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/MCRegisterInfo.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/SStream.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/arch/X86/X86Disassembler.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/arch/X86/X86DisassemblerDecoder.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/arch/X86/X86IntelInstPrinter.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/arch/X86/X86Mapping.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/arch/X86/X86Module.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/cs.c
    ${PROJECT_SOURCE_DIR}/../3rdparty/Capstone/src/utils.c
)

set_target_properties(capstone_x86 PROPERTIES LINKER_LANGUAGE C)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
