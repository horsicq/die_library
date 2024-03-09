cmake_minimum_required(VERSION 3.14)

project(capstone LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(${PROJECT_SOURCE_DIR})

if(WIN32)
  add_definitions(-DWIN32)
endif()

add_definitions(
    -DCAPSTONE_HAS_ARM
    -DCAPSTONE_HAS_ARM64
    -DCAPSTONE_HAS_BPF
    -DCAPSTONE_HAS_EVM
    -DCAPSTONE_HAS_M68K
    -DCAPSTONE_HAS_M680X
    -DCAPSTONE_HAS_MIPS
    -DCAPSTONE_HAS_MOS65XX
    -DCAPSTONE_HAS_POWERPC
    -DCAPSTONE_HAS_RISCV
    -DCAPSTONE_HAS_SPARC
    -DCAPSTONE_HAS_SYSZ
    -DCAPSTONE_HAS_TMS320C64X
    -DCAPSTONE_HAS_WASM
    -DCAPSTONE_HAS_X86
    -DCAPSTONE_HAS_XCORE
    -DCAPSTONE_X86_ATT_DISABLE_NO
    -DCAPSTONE_DIET_NO
    -DCAPSTONE_X86_REDUCE_NO
    -DCAPSTONE_USE_SYS_DYN_MEM
    -D_LIB
    )
    
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/include)

add_library(capstone STATIC
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/MCInst.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/MCInstrDesc.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/MCRegisterInfo.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/SStream.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/AArch64/AArch64BaseInfo.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/AArch64/AArch64Disassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/AArch64/AArch64InstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/AArch64/AArch64Mapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/AArch64/AArch64Module.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/ARM/ARMDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/ARM/ARMInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/ARM/ARMMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/ARM/ARMModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/BPF/BPFDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/BPF/BPFInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/BPF/BPFMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/BPF/BPFModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/EVM/EVMDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/EVM/EVMInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/EVM/EVMMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/EVM/EVMModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/M680X/M680XDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/M680X/M680XInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/M680X/M680XModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/M68K/M68KDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/M68K/M68KInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/M68K/M68KModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/MOS65XX/MOS65XXDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/MOS65XX/MOS65XXModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Mips/MipsDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Mips/MipsInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Mips/MipsMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Mips/MipsModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/PowerPC/PPCDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/PowerPC/PPCInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/PowerPC/PPCMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/PowerPC/PPCModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/RISCV/RISCVDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/RISCV/RISCVInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/RISCV/RISCVMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/RISCV/RISCVModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Sparc/SparcDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Sparc/SparcInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Sparc/SparcMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/Sparc/SparcModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/SystemZ/SystemZDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/SystemZ/SystemZInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/SystemZ/SystemZMCTargetDesc.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/SystemZ/SystemZMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/SystemZ/SystemZModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/TMS320C64x/TMS320C64xDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/TMS320C64x/TMS320C64xInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/TMS320C64x/TMS320C64xMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/TMS320C64x/TMS320C64xModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/WASM/WASMDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/WASM/WASMInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/WASM/WASMMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/WASM/WASMModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86ATTInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86Disassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86DisassemblerDecoder.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86InstPrinterCommon.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86IntelInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86Mapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/X86/X86Module.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/XCore/XCoreDisassembler.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/XCore/XCoreInstPrinter.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/XCore/XCoreMapping.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/arch/XCore/XCoreModule.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/cs.c
    ${PROJECT_SOURCE_DIR}/3rdparty/Capstone/src/utils.c
)

set_target_properties(capstone PROPERTIES LINKER_LANGUAGE C)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
