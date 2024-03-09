/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "xmach.h"

XMACH::XMACH(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XBinary(pDevice, bIsImage, nModuleAddress)
{
}

XMACH::~XMACH()
{
}

bool XMACH::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    quint32 nMagic = read_uint32(0);

    if ((nMagic == XMACH_DEF::S_MH_MAGIC) || (nMagic == XMACH_DEF::S_MH_CIGAM) || (nMagic == XMACH_DEF::S_MH_MAGIC_64) || (nMagic == XMACH_DEF::S_MH_CIGAM_64)) {
        bResult = true;
    }

    return bResult;
}

bool XMACH::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XMACH xmach(pDevice, bIsImage, nModuleAddress);

    return xmach.isValid();
}

XBinary::MODE XMACH::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XMACH xmach(pDevice, bIsImage, nModuleAddress);

    return xmach.getMode();
}

XBinary::ENDIAN XMACH::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint32 nData = read_uint32(0);

    if ((nData == XMACH_DEF::S_MH_MAGIC) || (nData == XMACH_DEF::S_MH_MAGIC_64)) {
        result = ENDIAN_LITTLE;
    } else if ((nData == XMACH_DEF::S_MH_CIGAM) || (nData == XMACH_DEF::S_MH_CIGAM_64)) {
        result = ENDIAN_BIG;
    }

    return result;
}

qint64 XMACH::getHeaderOffset()
{
    return 0;
}

qint64 XMACH::getHeader32Size()
{
    return sizeof(XMACH_DEF::mach_header);
}

qint64 XMACH::getHeader64Size()
{
    return sizeof(XMACH_DEF::mach_header_64);
}

quint32 XMACH::getHeader_magic()
{
    return read_uint32(offsetof(XMACH_DEF::mach_header, magic));
}

qint32 XMACH::getHeader_cputype()
{
    return read_int32(offsetof(XMACH_DEF::mach_header, cputype), isBigEndian());
}

qint32 XMACH::getHeader_cpusubtype()
{
    return read_int32(offsetof(XMACH_DEF::mach_header, cpusubtype), isBigEndian());
}

quint32 XMACH::getHeader_filetype()
{
    return read_uint32(offsetof(XMACH_DEF::mach_header, filetype), isBigEndian());
}

quint32 XMACH::getHeader_ncmds()
{
    return read_uint32(offsetof(XMACH_DEF::mach_header, ncmds), isBigEndian());
}

quint32 XMACH::getHeader_sizeofcmds()
{
    return read_uint32(offsetof(XMACH_DEF::mach_header, sizeofcmds), isBigEndian());
}

quint32 XMACH::getHeader_flags()
{
    return read_uint32(offsetof(XMACH_DEF::mach_header, flags), isBigEndian());
}

quint32 XMACH::getHeader_reserved()
{
    return read_uint32(offsetof(XMACH_DEF::mach_header_64, reserved), isBigEndian());
}

void XMACH::setHeader_magic(quint32 nValue)
{
    write_uint32(offsetof(XMACH_DEF::mach_header, magic), nValue);
}

void XMACH::setHeader_cputype(qint32 nValue)
{
    write_int32(offsetof(XMACH_DEF::mach_header, cputype), nValue, isBigEndian());
}

void XMACH::setHeader_cpusubtype(qint32 nValue)
{
    write_int32(offsetof(XMACH_DEF::mach_header, cpusubtype), nValue, isBigEndian());
}

void XMACH::setHeader_filetype(quint32 nValue)
{
    write_uint32(offsetof(XMACH_DEF::mach_header, filetype), nValue, isBigEndian());
}

void XMACH::setHeader_ncmds(quint32 nValue)
{
    write_uint32(offsetof(XMACH_DEF::mach_header, ncmds), nValue, isBigEndian());
}

void XMACH::setHeader_sizeofcmds(quint32 nValue)
{
    write_uint32(offsetof(XMACH_DEF::mach_header, sizeofcmds), nValue, isBigEndian());
}

void XMACH::setHeader_flags(quint32 nValue)
{
    write_uint32(offsetof(XMACH_DEF::mach_header, flags), nValue, isBigEndian());
}

void XMACH::setHeader_reserved(quint32 nValue)
{
    write_uint32(offsetof(XMACH_DEF::mach_header_64, reserved), nValue, isBigEndian());
}

qint64 XMACH::getHeaderSize()
{
    qint64 nResult = 0;

    if (is64()) {
        nResult = sizeof(XMACH_DEF::mach_header_64);
    } else {
        nResult = sizeof(XMACH_DEF::mach_header);
    }

    return nResult;
}

QMap<quint64, QString> XMACH::getHeaderMagics()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0xfeedface, "MH_MAGIC");
    mapResult.insert(0xcefaedfe, "MH_CIGAM");
    mapResult.insert(0xfeedfacf, "MH_MAGIC_64");
    mapResult.insert(0xcffaedfe, "MH_CIGAM_64");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderMagicsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0xfeedface, "MAGIC");
    mapResult.insert(0xcefaedfe, "CIGAM");
    mapResult.insert(0xfeedfacf, "MAGIC_64");
    mapResult.insert(0xcffaedfe, "CIGAM_64");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderCpuTypes()
{
    QMap<quint64, QString> mapResult;
    // https://opensource.apple.com/source/cctools/cctools-836/include/mach/machine.h
    mapResult.insert(1, "CPU_TYPE_VAX");
    mapResult.insert(2, "CPU_TYPE_ROMP");
    mapResult.insert(4, "CPU_TYPE_NS32032");
    mapResult.insert(5, "CPU_TYPE_NS32332");
    mapResult.insert(6, "CPU_TYPE_MC680x0");
    mapResult.insert(7, "CPU_TYPE_I386");
    mapResult.insert(0x1000007, "CPU_TYPE_X86_64");
    mapResult.insert(8, "CPU_TYPE_MIPS");
    mapResult.insert(9, "CPU_TYPE_NS32532");
    mapResult.insert(0xB, "CPU_TYPE_HPPA");
    mapResult.insert(0xC, "CPU_TYPE_ARM");
    mapResult.insert(0x100000C, "CPU_TYPE_ARM64");
    mapResult.insert(0xD, "CPU_TYPE_MC88000");
    mapResult.insert(0xE, "CPU_TYPE_SPARC");
    mapResult.insert(0xF, "CPU_TYPE_I860");
    mapResult.insert(0x10, "CPU_TYPE_I860_LITTLE");
    mapResult.insert(0x11, "CPU_TYPE_RS6000");
    mapResult.insert(0x12, "CPU_TYPE_POWERPC");
    mapResult.insert(0x1000012, "CPU_TYPE_POWERPC64");
    mapResult.insert(255, "CPU_TYPE_VEO");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderCpuTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(1, "VAX");
    mapResult.insert(2, "ROMP");
    mapResult.insert(4, "NS32032");
    mapResult.insert(5, "NS32332");
    mapResult.insert(6, "MC680x0");
    mapResult.insert(7, "I386");
    mapResult.insert(0x1000007, "X86_64");
    mapResult.insert(8, "MIPS");
    mapResult.insert(9, "NS32532");
    mapResult.insert(0xB, "HPPA");
    mapResult.insert(0xC, "ARM");
    mapResult.insert(0x100000C, "ARM64");
    mapResult.insert(0xD, "MC88000");
    mapResult.insert(0xE, "SPARC");
    mapResult.insert(0xF, "I860");
    mapResult.insert(0x10, "I860_LITTLE");
    mapResult.insert(0x11, "RS6000");
    mapResult.insert(0x12, "POWERPC");
    mapResult.insert(0x1000012, "POWERPC64");
    mapResult.insert(255, "VEO");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderCpuSubTypes(quint32 nCpuType)
{
    QMap<quint64, QString> mapResult;

    if (nCpuType == XMACH_DEF::S_CPU_TYPE_VAX) {
        mapResult.insert(0, "CPU_SUBTYPE_VAX_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_VAX780");
        mapResult.insert(2, "CPU_SUBTYPE_VAX785");
        mapResult.insert(3, "CPU_SUBTYPE_VAX750");
        mapResult.insert(4, "CPU_SUBTYPE_VAX730");
        mapResult.insert(5, "CPU_SUBTYPE_UVAXI");
        mapResult.insert(6, "CPU_SUBTYPE_UVAXII");
        mapResult.insert(7, "CPU_SUBTYPE_VAX8200");
        mapResult.insert(8, "CPU_SUBTYPE_VAX8500");
        mapResult.insert(9, "CPU_SUBTYPE_VAX8600");
        mapResult.insert(10, "CPU_SUBTYPE_VAX8650");
        mapResult.insert(11, "CPU_SUBTYPE_VAX8800");
        mapResult.insert(12, "CPU_SUBTYPE_UVAXIII");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_ROMP) {
        mapResult.insert(0, "CPU_SUBTYPE_RT_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_RT_PC");
        mapResult.insert(2, "CPU_SUBTYPE_RT_APC");
        mapResult.insert(3, "CPU_SUBTYPE_RT_135");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_NS32032) || (nCpuType == XMACH_DEF::S_CPU_TYPE_NS32332) || (nCpuType == XMACH_DEF::S_CPU_TYPE_NS32532)) {
        mapResult.insert(0, "CPU_SUBTYPE_MMAX_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_MMAX_DPC");
        mapResult.insert(2, "CPU_SUBTYPE_SQT");
        mapResult.insert(3, "CPU_SUBTYPE_MMAX_APC_FPU");
        mapResult.insert(4, "CPU_SUBTYPE_MMAX_APC_FPA");
        mapResult.insert(5, "CPU_SUBTYPE_MMAX_XPC");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_I386) || (nCpuType == XMACH_DEF::S_CPU_TYPE_X86_64)) {
        mapResult.insert(3, "CPU_SUBTYPE_386_ALL");
        mapResult.insert(0x80000003, "CPU_SUBTYPE_X86_64_ALL");
        // TODO
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MIPS) {
        mapResult.insert(0, "CPU_SUBTYPE_MIPS_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_MIPS_R2300");
        mapResult.insert(2, "CPU_SUBTYPE_MIPS_R2600");
        mapResult.insert(3, "CPU_SUBTYPE_MIPS_R2800");
        mapResult.insert(4, "CPU_SUBTYPE_MIPS_R2000a");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC680x0) {
        mapResult.insert(1, "CPU_SUBTYPE_MC68030");
        mapResult.insert(2, "CPU_SUBTYPE_MC68040");
        mapResult.insert(3, "CPU_SUBTYPE_MC68030_ONLY");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_HPPA) {
        mapResult.insert(0, "CPU_SUBTYPE_HPPA_7100");
        mapResult.insert(1, "CPU_SUBTYPE_HPPA_7100LC");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_ARM) || (nCpuType == XMACH_DEF::S_CPU_TYPE_ARM64)) {
        mapResult.insert(0, "CPU_SUBTYPE_ARM_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_ARM_A500_ARCH");
        mapResult.insert(2, "CPU_SUBTYPE_ARM_A500");
        mapResult.insert(3, "CPU_SUBTYPE_ARM_A440");
        mapResult.insert(4, "CPU_SUBTYPE_ARM_M4");
        mapResult.insert(5, "CPU_SUBTYPE_ARM_V4T");
        mapResult.insert(6, "CPU_SUBTYPE_ARM_V6");
        mapResult.insert(7, "CPU_SUBTYPE_ARM_V5TEJ");
        mapResult.insert(8, "CPU_SUBTYPE_ARM_XSCALE");
        mapResult.insert(9, "CPU_SUBTYPE_ARM_V7");
        mapResult.insert(10, "CPU_SUBTYPE_ARM_V7F");
        mapResult.insert(11, "CPU_SUBTYPE_ARM_V7S");
        mapResult.insert(12, "CPU_SUBTYPE_ARM_V7K");
        mapResult.insert(14, "CPU_SUBTYPE_ARM_V6M");
        mapResult.insert(15, "CPU_SUBTYPE_ARM_V7M");
        mapResult.insert(16, "CPU_SUBTYPE_ARM_V7EM");
        mapResult.insert(0x80000002, "CPU_SUBTYPE_ARM64E");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC88000) {
        mapResult.insert(0, "CPU_SUBTYPE_MC88000_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_MC88100");
        mapResult.insert(2, "CPU_SUBTYPE_MC88110");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC98000) {
        mapResult.insert(0, "CPU_SUBTYPE_MC98000_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_MC98601");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_I860) {
        mapResult.insert(0, "CPU_SUBTYPE_I860_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_I860_860");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_RS6000) {
        mapResult.insert(0, "CPU_SUBTYPE_RS6000_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_RS6000");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_SPARC) {
        mapResult.insert(0, "CPU_SUBTYPE_SPARC_ALL");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_POWERPC) || (nCpuType == XMACH_DEF::S_CPU_TYPE_POWERPC64)) {
        mapResult.insert(0, "CPU_SUBTYPE_POWERPC_ALL");
        mapResult.insert(1, "CPU_SUBTYPE_POWERPC_601");
        mapResult.insert(2, "CPU_SUBTYPE_POWERPC_602");
        mapResult.insert(3, "CPU_SUBTYPE_POWERPC_603");
        mapResult.insert(4, "CPU_SUBTYPE_POWERPC_603e");
        mapResult.insert(5, "CPU_SUBTYPE_POWERPC_603ev");
        mapResult.insert(6, "CPU_SUBTYPE_POWERPC_604");
        mapResult.insert(7, "CPU_SUBTYPE_POWERPC_604e");
        mapResult.insert(8, "CPU_SUBTYPE_POWERPC_620");
        mapResult.insert(9, "CPU_SUBTYPE_POWERPC_750");
        mapResult.insert(10, "CPU_SUBTYPE_POWERPC_7400");
        mapResult.insert(11, "CPU_SUBTYPE_POWERPC_7450");
        mapResult.insert(100, "CPU_SUBTYPE_POWERPC_970");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_VEO) {
        mapResult.insert(1, "CPU_SUBTYPE_VEO_1");
        mapResult.insert(2, "CPU_SUBTYPE_VEO_2");
        mapResult.insert(3, "CPU_SUBTYPE_VEO_3");
        mapResult.insert(4, "CPU_SUBTYPE_VEO_4");
    }

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderCpuSubTypesS(quint32 nCpuType)
{
    QMap<quint64, QString> mapResult;

    if (nCpuType == XMACH_DEF::S_CPU_TYPE_VAX) {
        mapResult.insert(0, "VAX_ALL");
        mapResult.insert(1, "VAX780");
        mapResult.insert(2, "VAX785");
        mapResult.insert(3, "VAX750");
        mapResult.insert(4, "VAX730");
        mapResult.insert(5, "UVAXI");
        mapResult.insert(6, "UVAXII");
        mapResult.insert(7, "VAX8200");
        mapResult.insert(8, "VAX8500");
        mapResult.insert(9, "VAX8600");
        mapResult.insert(10, "VAX8650");
        mapResult.insert(11, "VAX8800");
        mapResult.insert(12, "UVAXIII");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_ROMP) {
        mapResult.insert(0, "RT_ALL");
        mapResult.insert(1, "RT_PC");
        mapResult.insert(2, "RT_APC");
        mapResult.insert(3, "RT_135");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_NS32032) || (nCpuType == XMACH_DEF::S_CPU_TYPE_NS32332) || (nCpuType == XMACH_DEF::S_CPU_TYPE_NS32532)) {
        mapResult.insert(0, "MMAX_ALL");
        mapResult.insert(1, "MMAX_DPC");
        mapResult.insert(2, "SQT");
        mapResult.insert(3, "MMAX_APC_FPU");
        mapResult.insert(4, "MMAX_APC_FPA");
        mapResult.insert(5, "MMAX_XPC");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_I386) || (nCpuType == XMACH_DEF::S_CPU_TYPE_X86_64)) {
        mapResult.insert(3, "386_ALL");
        mapResult.insert(0x80000003, "X86_64_ALL");
        // TODO
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MIPS) {
        mapResult.insert(0, "MIPS_ALL");
        mapResult.insert(1, "MIPS_R2300");
        mapResult.insert(2, "MIPS_R2600");
        mapResult.insert(3, "MIPS_R2800");
        mapResult.insert(4, "MIPS_R2000a");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC680x0) {
        mapResult.insert(1, "MC68030");
        mapResult.insert(2, "MC68040");
        mapResult.insert(3, "MC68030_ONLY");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_HPPA) {
        mapResult.insert(0, "HPPA_7100");
        mapResult.insert(1, "HPPA_7100LC");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_ARM) || (nCpuType == XMACH_DEF::S_CPU_TYPE_ARM64)) {
        mapResult.insert(0, "ARM_ALL");
        mapResult.insert(1, "ARM_A500_ARCH");
        mapResult.insert(2, "ARM_A500");
        mapResult.insert(3, "ARM_A440");
        mapResult.insert(4, "ARM_M4");
        mapResult.insert(5, "ARM_V4T");
        mapResult.insert(6, "ARM_V6");
        mapResult.insert(7, "ARM_V5TEJ");
        mapResult.insert(8, "ARM_XSCALE");
        mapResult.insert(9, "ARM_V7");
        mapResult.insert(10, "ARM_V7F");
        mapResult.insert(11, "ARM_V7S");
        mapResult.insert(12, "ARM_V7K");
        mapResult.insert(14, "ARM_V6M");
        mapResult.insert(15, "ARM_V7M");
        mapResult.insert(16, "ARM_V7EM");
        mapResult.insert(0x80000002, "ARM64E");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC88000) {
        mapResult.insert(0, "MC88000_ALL");
        mapResult.insert(1, "MC88100");
        mapResult.insert(2, "MC88110");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC98000) {
        mapResult.insert(0, "MC98000_ALL");
        mapResult.insert(1, "MC98601");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_I860) {
        mapResult.insert(0, "I860_ALL");
        mapResult.insert(1, "I860_860");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_RS6000) {
        mapResult.insert(0, "RS6000_ALL");
        mapResult.insert(1, "RS6000");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_SPARC) {
        mapResult.insert(0, "SPARC_ALL");
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_POWERPC) || (nCpuType == XMACH_DEF::S_CPU_TYPE_POWERPC64)) {
        mapResult.insert(0, "POWERPC_ALL");
        mapResult.insert(1, "POWERPC_601");
        mapResult.insert(2, "POWERPC_602");
        mapResult.insert(3, "POWERPC_603");
        mapResult.insert(4, "POWERPC_603e");
        mapResult.insert(5, "POWERPC_603ev");
        mapResult.insert(6, "POWERPC_604");
        mapResult.insert(7, "POWERPC_604e");
        mapResult.insert(8, "POWERPC_620");
        mapResult.insert(9, "POWERPC_750");
        mapResult.insert(10, "POWERPC_7400");
        mapResult.insert(11, "POWERPC_7450");
        mapResult.insert(100, "POWERPC_970");
    } else if (nCpuType == XMACH_DEF::S_CPU_TYPE_VEO) {
        mapResult.insert(1, "VEO_1");
        mapResult.insert(2, "VEO_2");
        mapResult.insert(3, "VEO_3");
        mapResult.insert(4, "VEO_4");
    }

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderFileTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "MH_OBJECT");
    mapResult.insert(0x2, "MH_EXECUTE");
    mapResult.insert(0x3, "MH_FVMLIB");
    mapResult.insert(0x4, "MH_CORE");
    mapResult.insert(0x5, "MH_PRELOAD");
    mapResult.insert(0x6, "MH_DYLIB");
    mapResult.insert(0x7, "MH_DYLINKER");
    mapResult.insert(0x8, "MH_BUNDLE");
    mapResult.insert(0x9, "MH_DYLIB_STUB");
    mapResult.insert(0xa, "MH_DSYM");
    mapResult.insert(0xb, "MH_KEXT_BUNDLE");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderFileTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "OBJECT");
    mapResult.insert(0x2, "EXECUTE");
    mapResult.insert(0x3, "FVMLIB");
    mapResult.insert(0x4, "CORE");
    mapResult.insert(0x5, "PRELOAD");
    mapResult.insert(0x6, "DYLIB");
    mapResult.insert(0x7, "DYLINKER");
    mapResult.insert(0x8, "BUNDLE");
    mapResult.insert(0x9, "DYLIB_STUB");
    mapResult.insert(0xa, "DSYM");
    mapResult.insert(0xb, "KEXT_BUNDLE");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderFlags()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "MH_NOUNDEFS");
    mapResult.insert(0x2, "MH_INCRLINK");
    mapResult.insert(0x4, "MH_DYLDLINK");
    mapResult.insert(0x8, "MH_BINDATLOAD");
    mapResult.insert(0x10, "MH_PREBOUND");
    mapResult.insert(0x20, "MH_SPLIT_SEGS");
    mapResult.insert(0x40, "MH_LAZY_INIT");
    mapResult.insert(0x80, "MH_TWOLEVEL");
    mapResult.insert(0x100, "MH_FORCE_FLAT");
    mapResult.insert(0x200, "MH_NOMULTIDEFS");
    mapResult.insert(0x400, "MH_NOFIXPREBINDING");
    mapResult.insert(0x800, "MH_PREBINDABLE");
    mapResult.insert(0x1000, "MH_ALLMODSBOUND");
    mapResult.insert(0x2000, "MH_SUBSECTIONS_VIA_SYMBOLS");
    mapResult.insert(0x4000, "MH_CANONICAL");
    mapResult.insert(0x8000, "MH_WEAK_DEFINES");
    mapResult.insert(0x10000, "MH_BINDS_TO_WEAK");
    mapResult.insert(0x20000, "MH_ALLOW_STACK_EXECUTION");
    mapResult.insert(0x40000, "MH_ROOT_SAFE");
    mapResult.insert(0x80000, "MH_SETUID_SAFE");
    mapResult.insert(0x100000, "MH_NO_REEXPORTED_DYLIBS");
    mapResult.insert(0x200000, "MH_PIE");
    mapResult.insert(0x400000, "MH_DEAD_STRIPPABLE_DYLIB");
    mapResult.insert(0x800000, "MH_HAS_TLV_DESCRIPTORS");
    mapResult.insert(0x1000000, "MH_NO_HEAP_EXECUTION");
    mapResult.insert(0x02000000, "MH_APP_EXTENSION_SAFE");
    mapResult.insert(0x04000000, "MH_NLIST_OUTOFSYNC_WITH_DYLDINFO");
    mapResult.insert(0x08000000, "MH_SIM_SUPPORT");

    return mapResult;
}

QMap<quint64, QString> XMACH::getHeaderFlagsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "NOUNDEFS");
    mapResult.insert(0x2, "INCRLINK");
    mapResult.insert(0x4, "DYLDLINK");
    mapResult.insert(0x8, "BINDATLOAD");
    mapResult.insert(0x10, "PREBOUND");
    mapResult.insert(0x20, "SPLIT_SEGS");
    mapResult.insert(0x40, "LAZY_INIT");
    mapResult.insert(0x80, "TWOLEVEL");
    mapResult.insert(0x100, "FORCE_FLAT");
    mapResult.insert(0x200, "NOMULTIDEFS");
    mapResult.insert(0x400, "NOFIXPREBINDING");
    mapResult.insert(0x800, "PREBINDABLE");
    mapResult.insert(0x1000, "ALLMODSBOUND");
    mapResult.insert(0x2000, "SUBSECTIONS_VIA_SYMBOLS");
    mapResult.insert(0x4000, "CANONICAL");
    mapResult.insert(0x8000, "WEAK_DEFINES");
    mapResult.insert(0x10000, "BINDS_TO_WEAK");
    mapResult.insert(0x20000, "ALLOW_STACK_EXECUTION");
    mapResult.insert(0x40000, "ROOT_SAFE");
    mapResult.insert(0x80000, "SETUID_SAFE");
    mapResult.insert(0x100000, "NO_REEXPORTED_DYLIBS");
    mapResult.insert(0x200000, "PIE");
    mapResult.insert(0x400000, "DEAD_STRIPPABLE_DYLIB");
    mapResult.insert(0x800000, "HAS_TLV_DESCRIPTORS");
    mapResult.insert(0x1000000, "NO_HEAP_EXECUTION");
    mapResult.insert(0x02000000, "APP_EXTENSION_SAFE");
    mapResult.insert(0x04000000, "NLIST_OUTOFSYNC_WITH_DYLDINFO");
    mapResult.insert(0x08000000, "SIM_SUPPORT");

    return mapResult;
}

QMap<quint64, QString> XMACH::getLoadCommandTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "LC_SEGMENT");
    mapResult.insert(0x2, "LC_SYMTAB");
    mapResult.insert(0x3, "LC_SYMSEG");
    mapResult.insert(0x4, "LC_THREAD");
    mapResult.insert(0x5, "LC_UNIXTHREAD");
    mapResult.insert(0x6, "LC_LOADFVMLIB");
    mapResult.insert(0x7, "LC_IDFVMLIB");
    mapResult.insert(0x8, "LC_IDENT");
    mapResult.insert(0x9, "LC_FVMFILE");
    mapResult.insert(0xa, "LC_PREPAGE");
    mapResult.insert(0xb, "LC_DYSYMTAB");
    mapResult.insert(0xc, "LC_LOAD_DYLIB");
    mapResult.insert(0xd, "LC_ID_DYLIB");
    mapResult.insert(0xe, "LC_LOAD_DYLINKER");
    mapResult.insert(0xf, "LC_ID_DYLINKER");
    mapResult.insert(0x10, "LC_PREBOUND_DYLIB");
    mapResult.insert(0x11, "LC_ROUTINES");
    mapResult.insert(0x12, "LC_SUB_FRAMEWORK");
    mapResult.insert(0x13, "LC_SUB_UMBRELLA");
    mapResult.insert(0x14, "LC_SUB_CLIENT");
    mapResult.insert(0x15, "LC_SUB_LIBRARY");
    mapResult.insert(0x16, "LC_TWOLEVEL_HINTS");
    mapResult.insert(0x17, "LC_PREBIND_CKSUM");
    mapResult.insert(0x18 | 0x80000000, "LC_LOAD_WEAK_DYLIB");
    mapResult.insert(0x19, "LC_SEGMENT_64");
    mapResult.insert(0x1a, "LC_ROUTINES_64");
    mapResult.insert(0x1b, "LC_UUID");
    mapResult.insert(0x1c | 0x80000000, "LC_RPATH");
    mapResult.insert(0x1d, "LC_CODE_SIGNATURE");
    mapResult.insert(0x1e, "LC_SEGMENT_SPLIT_INFO");
    mapResult.insert(0x1f | 0x80000000, "LC_REEXPORT_DYLIB");
    mapResult.insert(0x20, "LC_LAZY_LOAD_DYLIB");
    mapResult.insert(0x21, "LC_ENCRYPTION_INFO");
    mapResult.insert(0x22, "LC_DYLD_INFO");
    mapResult.insert(0x22 | 0x80000000, "LC_DYLD_INFO_ONLY");
    mapResult.insert(0x23 | 0x80000000, "LC_LOAD_UPWARD_DYLIB");
    mapResult.insert(0x24, "LC_VERSION_MIN_MACOSX");
    mapResult.insert(0x25, "LC_VERSION_MIN_IPHONEOS");
    mapResult.insert(0x26, "LC_FUNCTION_STARTS");
    mapResult.insert(0x27, "LC_DYLD_ENVIRONMENT");
    mapResult.insert(0x28 | 0x80000000, "LC_MAIN");
    mapResult.insert(0x29, "LC_DATA_IN_CODE");
    mapResult.insert(0x2A, "LC_SOURCE_VERSION");
    mapResult.insert(0x2B, "LC_DYLIB_CODE_SIGN_DRS");
    mapResult.insert(0x2C, "LC_ENCRYPTION_INFO_64");
    mapResult.insert(0x2D, "LC_LINKER_OPTION");
    mapResult.insert(0x2E, "LC_LINKER_OPTIMIZATION_HINT");
    mapResult.insert(0x2F, "LC_VERSION_MIN_TVOS");
    mapResult.insert(0x30, "LC_VERSION_MIN_WATCHOS");
    mapResult.insert(0x31, "LC_NOTE");
    mapResult.insert(0x32, "LC_BUILD_VERSION");
    mapResult.insert(0x33 | 0x80000000, "LC_DYLD_EXPORTS_TRIE");
    mapResult.insert(0x34 | 0x80000000, "LC_DYLD_CHAINED_FIXUPS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getLoadCommandTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "SEGMENT");
    mapResult.insert(0x2, "SYMTAB");
    mapResult.insert(0x3, "SYMSEG");
    mapResult.insert(0x4, "THREAD");
    mapResult.insert(0x5, "UNIXTHREAD");
    mapResult.insert(0x6, "LOADFVMLIB");
    mapResult.insert(0x7, "IDFVMLIB");
    mapResult.insert(0x8, "IDENT");
    mapResult.insert(0x9, "FVMFILE");
    mapResult.insert(0xa, "PREPAGE");
    mapResult.insert(0xb, "DYSYMTAB");
    mapResult.insert(0xc, "LOAD_DYLIB");
    mapResult.insert(0xd, "ID_DYLIB");
    mapResult.insert(0xe, "LOAD_DYLINKER");
    mapResult.insert(0xf, "ID_DYLINKER");
    mapResult.insert(0x10, "PREBOUND_DYLIB");
    mapResult.insert(0x11, "ROUTINES");
    mapResult.insert(0x12, "SUB_FRAMEWORK");
    mapResult.insert(0x13, "SUB_UMBRELLA");
    mapResult.insert(0x14, "SUB_CLIENT");
    mapResult.insert(0x15, "SUB_LIBRARY");
    mapResult.insert(0x16, "TWOLEVEL_HINTS");
    mapResult.insert(0x17, "PREBIND_CKSUM");
    mapResult.insert(0x18 | 0x80000000, "LOAD_WEAK_DYLIB");
    mapResult.insert(0x19, "SEGMENT_64");
    mapResult.insert(0x1a, "ROUTINES_64");
    mapResult.insert(0x1b, "UUID");
    mapResult.insert(0x1c | 0x80000000, "RPATH");
    mapResult.insert(0x1d, "CODE_SIGNATURE");
    mapResult.insert(0x1e, "SEGMENT_SPLIT_INFO");
    mapResult.insert(0x1f | 0x80000000, "REEXPORT_DYLIB");
    mapResult.insert(0x20, "LAZY_LOAD_DYLIB");
    mapResult.insert(0x21, "ENCRYPTION_INFO");
    mapResult.insert(0x22, "DYLD_INFO");
    mapResult.insert(0x22 | 0x80000000, "DYLD_INFO_ONLY");
    mapResult.insert(0x23 | 0x80000000, "LOAD_UPWARD_DYLIB");
    mapResult.insert(0x24, "VERSION_MIN_MACOSX");
    mapResult.insert(0x25, "VERSION_MIN_IPHONEOS");
    mapResult.insert(0x26, "FUNCTION_STARTS");
    mapResult.insert(0x27, "DYLD_ENVIRONMENT");
    mapResult.insert(0x28 | 0x80000000, "MAIN");
    mapResult.insert(0x29, "DATA_IN_CODE");
    mapResult.insert(0x2A, "SOURCE_VERSION");
    mapResult.insert(0x2B, "DYLIB_CODE_SIGN_DRS");
    mapResult.insert(0x2C, "ENCRYPTION_INFO_64");
    mapResult.insert(0x2D, "LINKER_OPTION");
    mapResult.insert(0x2E, "LINKER_OPTIMIZATION_HINT");
    mapResult.insert(0x2F, "VERSION_MIN_TVOS");
    mapResult.insert(0x30, "VERSION_MIN_WATCHOS");
    mapResult.insert(0x31, "NOTE");
    mapResult.insert(0x32, "BUILD_VERSION");
    mapResult.insert(0x33 | 0x80000000, "DYLD_EXPORTS_TRIE");
    mapResult.insert(0x34 | 0x80000000, "DYLD_CHAINED_FIXUPS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getVMProtections()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "VM_PROT_READ");
    mapResult.insert(0x2, "VM_PROT_WRITE");
    mapResult.insert(0x4, "VM_PROT_EXECUTE");

    return mapResult;
}

QMap<quint64, QString> XMACH::getVMProtectionsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x1, "READ");
    mapResult.insert(0x2, "WRITE");
    mapResult.insert(0x4, "EXECUTE");

    return mapResult;
}

QMap<quint64, QString> XMACH::getSectionFlagsTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00, "S_REGULAR");
    mapResult.insert(0x01, "S_ZEROFILL");
    mapResult.insert(0x02, "S_CSTRING_LITERALS");
    mapResult.insert(0x03, "S_4BYTE_LITERALS");
    mapResult.insert(0x04, "S_8BYTE_LITERALS");
    mapResult.insert(0x05, "S_LITERAL_POINTERS");
    mapResult.insert(0x06, "S_NON_LAZY_SYMBOL_POINTERS");
    mapResult.insert(0x07, "S_LAZY_SYMBOL_POINTERS");
    mapResult.insert(0x08, "S_SYMBOL_STUBS");
    mapResult.insert(0x09, "S_MOD_INIT_FUNC_POINTERS");
    mapResult.insert(0x0A, "S_MOD_TERM_FUNC_POINTERS");
    mapResult.insert(0x0B, "S_COALESCED");
    mapResult.insert(0x0C, "S_GB_ZEROFILL");
    mapResult.insert(0x0D, "S_INTERPOSING");
    mapResult.insert(0x0E, "S_16BYTE_LITERALS");
    mapResult.insert(0x0F, "S_DTRACE_DOF");
    mapResult.insert(0x10, "S_LAZY_DYLIB_SYMBOL_POINTERS");
    mapResult.insert(0x11, "S_THREAD_LOCAL_REGULAR");
    mapResult.insert(0x12, "S_THREAD_LOCAL_ZEROFILL");
    mapResult.insert(0x13, "S_THREAD_LOCAL_VARIABLES");
    mapResult.insert(0x14, "S_THREAD_LOCAL_VARIABLE_POINTERS");
    mapResult.insert(0x15, "S_THREAD_LOCAL_INIT_FUNCTION_POINTERS");
    mapResult.insert(0x16, "S_INIT_FUNC_OFFSETS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getSectionFlagsTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00, "REGULAR");
    mapResult.insert(0x01, "ZEROFILL");
    mapResult.insert(0x02, "CSTRING_LITERALS");
    mapResult.insert(0x03, "4BYTE_LITERALS");
    mapResult.insert(0x04, "8BYTE_LITERALS");
    mapResult.insert(0x05, "LITERAL_POINTERS");
    mapResult.insert(0x06, "NON_LAZY_SYMBOL_POINTERS");
    mapResult.insert(0x07, "LAZY_SYMBOL_POINTERS");
    mapResult.insert(0x08, "SYMBOL_STUBS");
    mapResult.insert(0x09, "MOD_INIT_FUNC_POINTERS");
    mapResult.insert(0x0A, "MOD_TERM_FUNC_POINTERS");
    mapResult.insert(0x0B, "COALESCED");
    mapResult.insert(0x0C, "GB_ZEROFILL");
    mapResult.insert(0x0D, "INTERPOSING");
    mapResult.insert(0x0E, "16BYTE_LITERALS");
    mapResult.insert(0x0F, "DTRACE_DOF");
    mapResult.insert(0x10, "LAZY_DYLIB_SYMBOL_POINTERS");
    mapResult.insert(0x11, "THREAD_LOCAL_REGULAR");
    mapResult.insert(0x12, "THREAD_LOCAL_ZEROFILL");
    mapResult.insert(0x13, "THREAD_LOCAL_VARIABLES");
    mapResult.insert(0x14, "THREAD_LOCAL_VARIABLE_POINTERS");
    mapResult.insert(0x15, "THREAD_LOCAL_INIT_FUNCTION_POINTERS");
    mapResult.insert(0x16, "INIT_FUNC_OFFSETS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getSectionAttributesUsr()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x02000000, "S_ATTR_DEBUG");
    mapResult.insert(0x04000000, "S_ATTR_SELF_MODIFYING_CODE");
    mapResult.insert(0x08000000, "S_ATTR_LIVE_SUPPORT");
    mapResult.insert(0x10000000, "S_ATTR_NO_DEAD_STRIP");
    mapResult.insert(0x20000000, "S_ATTR_STRIP_STATIC_SYMS");
    mapResult.insert(0x40000000, "S_ATTR_NO_TOC");
    mapResult.insert(0x80000000, "S_ATTR_PURE_INSTRUCTIONS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getSectionAttributesUsrS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x02000000, "DEBUG");
    mapResult.insert(0x04000000, "SELF_MODIFYING_CODE");
    mapResult.insert(0x08000000, "LIVE_SUPPORT");
    mapResult.insert(0x10000000, "NO_DEAD_STRIP");
    mapResult.insert(0x20000000, "STRIP_STATIC_SYMS");
    mapResult.insert(0x40000000, "NO_TOC");
    mapResult.insert(0x80000000, "PURE_INSTRUCTIONS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getSectionAttributesSys()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000100, "S_ATTR_LOC_RELOC");
    mapResult.insert(0x00000200, "S_ATTR_EXT_RELOC");
    mapResult.insert(0x00000400, "S_ATTR_SOME_INSTRUCTIONS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getSectionAttributesSysS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000100, "LOC_RELOC");
    mapResult.insert(0x00000200, "EXT_RELOC");
    mapResult.insert(0x00000400, "SOME_INSTRUCTIONS");

    return mapResult;
}

QMap<quint64, QString> XMACH::getDICEKinds()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000001, "DICE_KIND_DATA");
    mapResult.insert(0x00000002, "DICE_KIND_JUMP_TABLE8");
    mapResult.insert(0x00000003, "DICE_KIND_JUMP_TABLE16");
    mapResult.insert(0x00000004, "DICE_KIND_JUMP_TABLE32");
    mapResult.insert(0x00000005, "DICE_KIND_ABS_JUMP_TABLE32");

    return mapResult;
}

QMap<quint64, QString> XMACH::getDICEKindsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000001, "DATA");
    mapResult.insert(0x00000002, "JUMP_TABLE8");
    mapResult.insert(0x00000003, "JUMP_TABLE16");
    mapResult.insert(0x00000004, "JUMP_TABLE32");
    mapResult.insert(0x00000005, "ABS_JUMP_TABLE32");

    return mapResult;
}

XMACH::COMMAND_RECORD XMACH::_readLoadCommand(qint64 nOffset, bool bIsBigEndian)
{
    COMMAND_RECORD result = {};

    result.nStructOffset = nOffset;
    result.nType = read_uint32(nOffset + offsetof(XMACH_DEF::load_command, cmd), bIsBigEndian);
    result.nSize = read_uint32(nOffset + offsetof(XMACH_DEF::load_command, cmdsize), bIsBigEndian);

    return result;
}

void XMACH::_setCommand_cmd(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::load_command, cmd), nValue, isBigEndian());
}

void XMACH::_setCommand_cmdsize(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::load_command, cmdsize), nValue, isBigEndian());
}

QList<XMACH::COMMAND_RECORD> XMACH::getCommandRecords(quint32 nCommandID)
{
    QList<COMMAND_RECORD> listResult;

    quint32 nNumberOfCommands = getHeader_ncmds();

    if (nNumberOfCommands & 0xFFFF0000) {
        nNumberOfCommands = 0;
    }

    quint32 nSizeOfCommands = getHeader_sizeofcmds();

    qint64 nOffset = getHeaderSize();
    bool bIsBigEndian = isBigEndian();
    bool bIs64 = is64();

    qint64 nSize = 0;

    for (quint32 i = 0; i < nNumberOfCommands; i++) {
        COMMAND_RECORD record = _readLoadCommand(nOffset, bIsBigEndian);

        if ((nCommandID == 0) || (record.nType == nCommandID)) {
            listResult.append(record);
        }

        qint64 _nSize = record.nSize;

        if (bIs64) {
            _nSize = S_ALIGN_UP(_nSize, 8);
        } else {
            _nSize = S_ALIGN_UP(_nSize, 4);
        }

        nSize += _nSize;
        nOffset += _nSize;

        if (nSize > nSizeOfCommands) {
            break;
        }
    }

    return listResult;
}

QList<XMACH::COMMAND_RECORD> XMACH::getCommandRecords(quint32 nCommandID, QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    QList<COMMAND_RECORD> listResult;

    qint32 nNumberOfCommands = pListCommandRecords->count();

    for (qint32 i = 0; i < nNumberOfCommands; i++) {
        if (pListCommandRecords->at(i).nType == nCommandID) {
            listResult.append(pListCommandRecords->at(i));
        }
    }

    return listResult;
}

bool XMACH::isCommandPresent(quint32 nCommandID, qint32 nIndex)
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(nCommandID);

    return isCommandPresent(nCommandID, nIndex, &listCommandRecords);
}

bool XMACH::isCommandPresent(quint32 nCommandID, qint32 nIndex, QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    bool bResult = false;

    qint32 nNumberOfCommands = pListCommandRecords->count();

    qint32 nCurrentIndex = 0;

    for (qint32 i = 0; i < nNumberOfCommands; i++) {
        if (pListCommandRecords->at(i).nType == nCommandID) {
            if (nCurrentIndex == nIndex) {
                bResult = true;

                break;
            }

            nCurrentIndex++;
        }
    }

    return bResult;
}

bool XMACH::isCommandPresent(quint32 nCommandID, QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    return isCommandPresent(nCommandID, 0, pListCommandRecords);
}

QByteArray XMACH::getCommandData(quint32 nCommandID, qint32 nIndex)
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(nCommandID);

    return getCommandData(nCommandID, nIndex, &listCommandRecords);
}

bool XMACH::setCommandData(quint32 nCommandID, QByteArray baData, qint32 nIndex)
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(nCommandID);

    return setCommandData(nCommandID, baData, nIndex, &listCommandRecords);
}

QByteArray XMACH::getCommandData(quint32 nCommandID, qint32 nIndex, QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    QByteArray baResult;

    qint32 nNumberOfCommands = pListCommandRecords->count();

    qint32 nCurrentIndex = 0;

    for (qint32 i = 0; i < nNumberOfCommands; i++) {
        if (pListCommandRecords->at(i).nType == nCommandID) {
            if (nCurrentIndex == nIndex) {
                baResult = read_array(pListCommandRecords->at(i).nStructOffset, pListCommandRecords->at(i).nSize);

                break;
            }

            nCurrentIndex++;
        }
    }

    return baResult;
}

bool XMACH::setCommandData(quint32 nCommandID, QByteArray baData, qint32 nIndex, QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    bool bResult = false;

    qint32 nNumberOfCommands = pListCommandRecords->count();

    qint32 nCurrentIndex = 0;

    for (qint32 i = 0; i < nNumberOfCommands; i++) {
        if (pListCommandRecords->at(i).nType == nCommandID) {
            qint32 nSize = baData.size();

            if (nCurrentIndex == nIndex) {
                if (nSize == pListCommandRecords->at(i).nSize) {
                    bResult = (write_array(pListCommandRecords->at(i).nStructOffset, baData.data(), pListCommandRecords->at(i).nSize) == nSize);
                }

                break;
            }

            nCurrentIndex++;
        }
    }

    return bResult;
}

qint64 XMACH::getCommandRecordOffset(quint32 nCommandID, qint32 nIndex)
{
    qint64 nResult = -1;

    QList<COMMAND_RECORD> listCR = getCommandRecords(nCommandID);

    if (nIndex < listCR.count()) {
        nResult = listCR.at(nIndex).nStructOffset;
    }

    return nResult;
}

qint64 XMACH::getCommandRecordOffset(quint32 nCommandID, qint32 nIndex, QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    qint64 nResult = -1;

    QList<COMMAND_RECORD> listCR = getCommandRecords(nCommandID, pListCommandRecords);

    if (nIndex < listCR.count()) {
        nResult = listCR.at(nIndex).nStructOffset;
    }

    return nResult;
}

qint64 XMACH::getCommandHeaderSize()
{
    return sizeof(XMACH_DEF::load_command);
}

qint64 XMACH::getAddressOfEntryPoint(XBinary::_MEMORY_MAP *pMemoryMap)
{
    qint64 nResult = -1;

    bool bIsBigEndian = isBigEndian();

    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords();

    if (isCommandPresent(XMACH_DEF::S_LC_MAIN, &listCommandRecords)) {
        qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_MAIN, 0, &listCommandRecords);

        qint64 nEntryPointOffset = read_uint64(nOffset + offsetof(XMACH_DEF::entry_point_command, entryoff), bIsBigEndian);

        nResult = offsetToAddress(pMemoryMap, nEntryPointOffset);
        //        nResult=nEntryPointOffset+getMo; // TODO Check
    } else if (isCommandPresent(XMACH_DEF::S_LC_UNIXTHREAD,
                                &listCommandRecords))  // TODO Check LC_THREAD
    {
        qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_UNIXTHREAD, 0, &listCommandRecords);

        quint32 nMachine = getHeader_cputype();

        nOffset += sizeof(XMACH_DEF::load_command);

        //        XMACH_DEF::state_hdr_t _state_hdr=_read_state_hdr_t(nOffset);

        nOffset += sizeof(XMACH_DEF::state_hdr_t);

        if (nMachine == XMACH_DEF::S_CPU_TYPE_I386) {
            nResult = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eip), bIsBigEndian);
        } else if (nMachine == XMACH_DEF::S_CPU_TYPE_X86_64) {
            nResult = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rip), bIsBigEndian);
        } else if (nMachine == XMACH_DEF::S_CPU_TYPE_ARM) {
            nResult = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, pc), bIsBigEndian);
        } else if (nMachine == XMACH_DEF::S_CPU_TYPE_ARM64) {
            nResult = read_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, pc), bIsBigEndian);
        }
        //        else if(nMachine==XMACH_DEF::S_CPU_TYPE_POWERPC)
        //        {
        //            nResult=read_uint32(nOffset+offsetof(XMACH_DEF::ppc_thread_state32_t,pc),bIsBigEndian);
        //        }
        else if (nMachine == XMACH_DEF::S_CPU_TYPE_MC680x0) {
            nResult = read_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, pc), bIsBigEndian);
        }
    }

    return nResult;
}

QList<XBinary::MAPMODE> XMACH::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_SEGMENTS);
    listResult.append(MAPMODE_SECTIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XMACH::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP result = {};

    qint32 nIndex = 0;

    result.mode = getMode();

    if (result.mode == MODE_64) {
        result.fileType = FT_MACHO64;
    } else {
        result.fileType = FT_MACHO32;
    }

    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = getTypeAsString();
    result.nBinarySize = getSize();
    result.nModuleAddress = getModuleAddress();

    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords();

    qint64 nMaxOffset = 0;
    XADDR nMinAddress = 0;
    XADDR nMaxAddress = 0;

    bool bIs64 = is64();

    if ((mapMode == MAPMODE_UNKNOWN) || (mapMode == MAPMODE_SEGMENTS)) {
        QList<SEGMENT_RECORD> listSegmentRecords = getSegmentRecords(&listCommandRecords);
        qint32 nNumberOfSegments = listSegmentRecords.count();

        if (nNumberOfSegments) {
            if (bIs64) {
                nMinAddress = listSegmentRecords.at(0).s.segment64.vmaddr;
            } else {
                nMinAddress = listSegmentRecords.at(0).s.segment32.vmaddr;
            }

            for (qint32 i = 0; i < nNumberOfSegments; i++) {
                QString sSegmentName;  // TODO Limit
                // TODO Align
                // TODO File size
                qint64 nFileOffset = 0;
                XADDR nVirtualAddress = 0;
                qint64 nFileSize = 0;
                qint64 nVirtualSize = 0;

                if (bIs64) {
                    QString _sSegmentName = QString(listSegmentRecords.at(i).s.segment64.segname);  // TODO Limit

                    sSegmentName = QString("%1(%2)['%3']").arg(tr("Segment"), QString::number(i), _sSegmentName);
                    nFileOffset = listSegmentRecords.at(i).s.segment64.fileoff;
                    nVirtualAddress = listSegmentRecords.at(i).s.segment64.vmaddr;
                    nFileSize = listSegmentRecords.at(i).s.segment64.filesize;
                    nVirtualSize = listSegmentRecords.at(i).s.segment64.vmsize;
                } else {
                    QString _sSegmentName = QString(listSegmentRecords.at(i).s.segment32.segname);  // TODO Limit

                    sSegmentName = QString("%1(%2)['%3']").arg(tr("Segment"), QString::number(i), _sSegmentName);
                    nFileOffset = listSegmentRecords.at(i).s.segment32.fileoff;
                    nVirtualAddress = listSegmentRecords.at(i).s.segment32.vmaddr;
                    nFileSize = listSegmentRecords.at(i).s.segment32.filesize;
                    nVirtualSize = listSegmentRecords.at(i).s.segment32.vmsize;
                }

                if (nFileSize) {
                    XBinary::_MEMORY_RECORD record = {};

                    record.type = MMT_LOADSEGMENT;

                    record.sName = sSegmentName;
                    record.nAddress = nVirtualAddress;
                    record.nSize = nFileSize;
                    record.nOffset = nFileOffset;
                    record.nIndex = nIndex++;

                    result.listRecords.append(record);
                }

                if (nVirtualSize > nFileSize) {
                    XBinary::_MEMORY_RECORD record = {};

                    record.type = MMT_LOADSEGMENT;
                    record.bIsVirtual = true;

                    record.sName = sSegmentName;
                    record.nAddress = nVirtualAddress + nFileSize;
                    record.nSize = nVirtualSize - nFileSize;
                    record.nOffset = -1;
                    record.nIndex = nIndex++;

                    result.listRecords.append(record);
                }

                nMaxOffset = qMax(nMaxOffset, nFileOffset + nFileSize);
                nMinAddress = qMin(nVirtualAddress, nMinAddress);
                nMaxAddress = qMax(nVirtualAddress + nVirtualSize, nMaxAddress);
            }
        }
    } else if (mapMode == MAPMODE_SECTIONS) {
        QList<SECTION_RECORD> listSections = getSectionRecords(&listCommandRecords);
        qint32 nNumberOfSections = listSections.count();

        if (nNumberOfSections) {
            if (bIs64) {
                nMinAddress = listSections.at(0).s.section64.addr;
            } else {
                nMinAddress = listSections.at(0).s.section32.addr;
            }

            for (qint32 i = 0; i < nNumberOfSections; i++) {
                QString sSectionName;
                qint64 nFileOffset = 0;
                XADDR nVirtualAddress = 0;
                qint64 nFileSize = 0;
                qint64 nVirtualSize = 0;

                if (bIs64) {
                    QString _sSectiontName = QString(listSections.at(i).s.section64.sectname);  // TODO Limit

                    sSectionName = QString("%1(%2)['%3']").arg(tr("Section"), QString::number(i), _sSectiontName);
                    nFileOffset = listSections.at(i).s.section64.offset;
                    nVirtualAddress = listSections.at(i).s.section64.addr;
                    nFileSize = listSections.at(i).s.section64.size;
                    nVirtualSize = listSections.at(i).s.section64.size;
                } else {
                    QString _sSectiontName = QString(listSections.at(i).s.section32.sectname);  // TODO Limit

                    sSectionName = QString("%1(%2)['%3']").arg(tr("Section"), QString::number(i), _sSectiontName);
                    nFileOffset = listSections.at(i).s.section32.offset;
                    nVirtualAddress = listSections.at(i).s.section32.addr;
                    nFileSize = listSections.at(i).s.section32.size;
                    nVirtualSize = listSections.at(i).s.section32.size;
                }

                XBinary::_MEMORY_RECORD record = {};

                record.type = MMT_LOADSEGMENT;

                record.sName = sSectionName;
                record.nAddress = nVirtualAddress;
                record.nSize = nFileSize;
                record.nOffset = nFileOffset;
                record.nIndex = nIndex++;

                result.listRecords.append(record);

                nMaxOffset = qMax(nMaxOffset, nFileOffset + nFileSize);
                nMinAddress = qMin(nVirtualAddress, nMinAddress);
                nMaxAddress = qMax(nVirtualAddress + nVirtualSize, nMaxAddress);
            }
        }
    }

    result.nImageSize = nMaxAddress - nMinAddress;
    result.nEntryPointAddress = getAddressOfEntryPoint(&result);

    return result;
}

QList<XMACH::LIBRARY_RECORD> XMACH::getLibraryRecords(qint32 nType)
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(nType);

    return getLibraryRecords(&listCommandRecords, nType);
}

QList<XMACH::LIBRARY_RECORD> XMACH::getLibraryRecords(QList<XMACH::COMMAND_RECORD> *pListCommandRecords, qint32 nType)
{
    QList<LIBRARY_RECORD> listResult;

    bool bIsBigEndian = isBigEndian();

    QList<COMMAND_RECORD> listLibraryCommandRecords = getCommandRecords(nType, pListCommandRecords);

    qint32 nNumberOfCommands = listLibraryCommandRecords.count();

    for (qint32 i = 0; i < nNumberOfCommands; i++) {
        LIBRARY_RECORD record = _readLibraryRecord(listLibraryCommandRecords.at(i).nStructOffset, bIsBigEndian);

        listResult.append(record);
    }

    return listResult;
}

XMACH::LIBRARY_RECORD XMACH::getLibraryRecordByName(const QString &sName, QList<XMACH::LIBRARY_RECORD> *pListLibraryRecords)
{
    LIBRARY_RECORD result = {};

    qint32 nNumberOfLibraries = pListLibraryRecords->count();

    for (qint32 i = 0; i < nNumberOfLibraries; i++) {
        if (pListLibraryRecords->at(i).sName == sName) {
            result = pListLibraryRecords->at(i);

            break;
        }
    }

    return result;
}

bool XMACH::isLibraryRecordNamePresent(const QString &sName)
{
    QList<LIBRARY_RECORD> listLibraryRecords = getLibraryRecords();

    return isLibraryRecordNamePresent(sName, &listLibraryRecords);
}

bool XMACH::isLibraryRecordNamePresent(const QString &sName, QList<XMACH::LIBRARY_RECORD> *pListLibraryRecords)
{
    bool bResult = false;

    qint32 nNumberOfLibraries = pListLibraryRecords->count();

    for (qint32 i = 0; i < nNumberOfLibraries; i++) {
        if (pListLibraryRecords->at(i).sName == sName) {
            bResult = true;

            break;
        }
    }

    return bResult;
}

XMACH::LIBRARY_RECORD XMACH::_readLibraryRecord(qint64 nOffset, bool bIsBigEndian)
{
    LIBRARY_RECORD result = {};

    result.name = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, name), bIsBigEndian);

    result.nStructOffset = nOffset;
    result.nStructSize = read_uint32(nOffset + offsetof(XMACH_DEF::load_command, cmdsize), bIsBigEndian);
    result.sFullName = read_ansiString(nOffset + result.name);
    result.sName = result.sFullName.section("/", -1, -1);
    result.timestamp = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, timestamp), bIsBigEndian);
    result.current_version = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, current_version), bIsBigEndian);
    result.compatibility_version = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, compatibility_version), bIsBigEndian);
    result.nMaxStringSize = result.nStructSize - sizeof(XMACH_DEF::dylib_command) - 2;

    if (result.nMaxStringSize < result.sFullName.size()) {
        result.nMaxStringSize = 0;
    }

    return result;
}

QList<XMACH::FVM_LIBRARY_RECORD> XMACH::getFvmLibraryRecords(qint32 nType)
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(nType);

    return getFvmLibraryRecords(&listCommandRecords, nType);
}

QList<XMACH::FVM_LIBRARY_RECORD> XMACH::getFvmLibraryRecords(QList<XMACH::COMMAND_RECORD> *pListCommandRecords, qint32 nType)
{
    QList<FVM_LIBRARY_RECORD> listResult;

    bool bIsBigEndian = isBigEndian();

    QList<COMMAND_RECORD> listLibraryCommandRecords = getCommandRecords(nType, pListCommandRecords);

    qint32 nNumberOfCommands = listLibraryCommandRecords.count();

    for (qint32 i = 0; i < nNumberOfCommands; i++) {
        FVM_LIBRARY_RECORD record = _readFvmLibraryRecord(listLibraryCommandRecords.at(i).nStructOffset, bIsBigEndian);

        listResult.append(record);
    }

    return listResult;
}

XMACH::FVM_LIBRARY_RECORD XMACH::getFvmLibraryRecordByName(const QString &sName, QList<XMACH::FVM_LIBRARY_RECORD> *pListLibraryRecords)
{
    FVM_LIBRARY_RECORD result = {};

    qint32 nNumberOfLibraries = pListLibraryRecords->count();

    for (qint32 i = 0; i < nNumberOfLibraries; i++) {
        if (pListLibraryRecords->at(i).sName == sName) {
            result = pListLibraryRecords->at(i);

            break;
        }
    }

    return result;
}

bool XMACH::isFvmLibraryRecordNamePresent(const QString &sName)
{
    QList<FVM_LIBRARY_RECORD> listLibraryRecords = getFvmLibraryRecords();

    return isFvmLibraryRecordNamePresent(sName, &listLibraryRecords);
}

bool XMACH::isFvmLibraryRecordNamePresent(const QString &sName, QList<XMACH::FVM_LIBRARY_RECORD> *pListLibraryRecords)
{
    bool bResult = false;

    qint32 nNumberOfLibraries = pListLibraryRecords->count();

    for (qint32 i = 0; i < nNumberOfLibraries; i++) {
        if (pListLibraryRecords->at(i).sName == sName) {
            bResult = true;

            break;
        }
    }

    return bResult;
}

XMACH::FVM_LIBRARY_RECORD XMACH::_readFvmLibraryRecord(qint64 nOffset, bool bIsBigEndian)
{
    FVM_LIBRARY_RECORD result = {};

    result.name = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::fvmlib, name), bIsBigEndian);

    result.nStructOffset = nOffset;
    result.nStructSize = read_uint32(nOffset + offsetof(XMACH_DEF::load_command, cmdsize), bIsBigEndian);
    result.sFullName = read_ansiString(nOffset + result.name);
    result.sName = result.sFullName.section("/", -1, -1);
    result.minor_version = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::fvmlib, minor_version), bIsBigEndian);
    result.header_addr = read_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::fvmlib, header_addr), bIsBigEndian);
    result.nMaxStringSize = result.nStructSize - sizeof(XMACH_DEF::fvmlib_command) - 2;

    if (result.nMaxStringSize < result.sFullName.size()) {
        result.nMaxStringSize = 0;
    }

    return result;
}

void XMACH::_setLibraryRecord_timestamp(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, timestamp), nValue, isBigEndian());
}

void XMACH::_setLibraryRecord_current_version(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, current_version), nValue, isBigEndian());
}

void XMACH::_setLibraryRecord_compatibility_version(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::dylib, compatibility_version), nValue, isBigEndian());
}

void XMACH::_setLibraryRecord_name(qint64 nOffset, const QString &sValue)
{
    bool bIsBigEndian = isBigEndian();
    LIBRARY_RECORD libraryRecord = _readLibraryRecord(nOffset, bIsBigEndian);

    if (libraryRecord.name == sizeof(XMACH_DEF::dylib_command)) {
        write_ansiStringFix(nOffset + sizeof(XMACH_DEF::dylib_command), libraryRecord.nStructSize - libraryRecord.name - 1, sValue);
    }
}

void XMACH::_setFvmLibraryRecord_minor_version(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::fvmlib, minor_version), nValue, isBigEndian());
}

void XMACH::_setFvmLibraryRecord_header_addr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + sizeof(XMACH_DEF::load_command) + offsetof(XMACH_DEF::fvmlib, header_addr), nValue, isBigEndian());
}

void XMACH::_setFvmLibraryRecord_name(qint64 nOffset, const QString &sValue)
{
    bool bIsBigEndian = isBigEndian();
    FVM_LIBRARY_RECORD libraryRecord = _readFvmLibraryRecord(nOffset, bIsBigEndian);

    if (libraryRecord.name == sizeof(XMACH_DEF::dylib_command)) {
        write_ansiStringFix(nOffset + sizeof(XMACH_DEF::dylib_command), libraryRecord.nStructSize - libraryRecord.name - 1, sValue);
    }
}

QList<XMACH::SEGMENT_RECORD> XMACH::getSegmentRecords()
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords();

    return getSegmentRecords(&listCommandRecords);
}

QList<XMACH::SEGMENT_RECORD> XMACH::getSegmentRecords(QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    QList<SEGMENT_RECORD> listResult;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        QList<COMMAND_RECORD> listLCSegments = getCommandRecords(XMACH_DEF::S_LC_SEGMENT_64, pListCommandRecords);

        qint32 nNumberOfSegments = listLCSegments.count();

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            qint64 nOffset = listLCSegments.at(i).nStructOffset;

            SEGMENT_RECORD record = {};

            record.bIs64 = bIs64;
            record.nStructOffset = nOffset;
            record.s.segment64 = _read_segment_command_64(nOffset, bIsBigEndian);

            listResult.append(record);
        }
    } else {
        QList<COMMAND_RECORD> listLCSegments = getCommandRecords(XMACH_DEF::S_LC_SEGMENT, pListCommandRecords);

        qint32 nNumberOfSegments = listLCSegments.count();

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            qint64 nOffset = listLCSegments.at(i).nStructOffset;

            SEGMENT_RECORD record = {};

            record.bIs64 = bIs64;
            record.nStructOffset = nOffset;
            record.s.segment32 = _read_segment_command(nOffset, bIsBigEndian);

            listResult.append(record);
        }
    }

    return listResult;
}

XMACH_DEF::segment_command XMACH::_read_segment_command(qint64 nOffset, bool bIsBigEndian)
{
    XMACH_DEF::segment_command result = {};

    read_array(nOffset + offsetof(XMACH_DEF::segment_command, segname), result.segname, sizeof(result.segname));
    result.vmaddr = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, vmaddr), bIsBigEndian);
    result.vmsize = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, vmsize), bIsBigEndian);
    result.fileoff = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, fileoff), bIsBigEndian);
    result.filesize = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, filesize), bIsBigEndian);
    result.maxprot = read_int32(nOffset + offsetof(XMACH_DEF::segment_command, maxprot), bIsBigEndian);
    result.initprot = read_int32(nOffset + offsetof(XMACH_DEF::segment_command, initprot), bIsBigEndian);
    result.nsects = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, nsects), bIsBigEndian);
    result.flags = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, flags), bIsBigEndian);

    return result;
}

XMACH_DEF::segment_command_64 XMACH::_read_segment_command_64(qint64 nOffset, bool bIsBigEndian)
{
    XMACH_DEF::segment_command_64 result = {};

    read_array(nOffset + offsetof(XMACH_DEF::segment_command_64, segname), result.segname, sizeof(result.segname));
    result.vmaddr = read_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, vmaddr), bIsBigEndian);
    result.vmsize = read_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, vmsize), bIsBigEndian);
    result.fileoff = read_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, fileoff), bIsBigEndian);
    result.filesize = read_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, filesize), bIsBigEndian);
    result.maxprot = read_int32(nOffset + offsetof(XMACH_DEF::segment_command_64, maxprot), bIsBigEndian);
    result.initprot = read_int32(nOffset + offsetof(XMACH_DEF::segment_command_64, initprot), bIsBigEndian);
    result.nsects = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command_64, nsects), bIsBigEndian);
    result.flags = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command_64, flags), bIsBigEndian);

    return result;
}

void XMACH::_setSegment32_segname(qint64 nOffset, const QString &sValue)
{
    write_ansiStringFix(nOffset + offsetof(XMACH_DEF::segment_command, segname), 16, sValue);
}

void XMACH::_setSegment32_vmaddr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command, vmaddr), nValue, isBigEndian());
}

void XMACH::_setSegment32_vmsize(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command, vmsize), nValue, isBigEndian());
}

void XMACH::_setSegment32_fileoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command, fileoff), nValue, isBigEndian());
}

void XMACH::_setSegment32_filesize(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command, filesize), nValue, isBigEndian());
}

void XMACH::_setSegment32_maxprot(qint64 nOffset, qint32 nValue)
{
    write_int32(nOffset + offsetof(XMACH_DEF::segment_command, maxprot), nValue, isBigEndian());
}

void XMACH::_setSegment32_initprot(qint64 nOffset, qint32 nValue)
{
    write_int32(nOffset + offsetof(XMACH_DEF::segment_command, initprot), nValue, isBigEndian());
}

void XMACH::_setSegment32_nsects(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command, nsects), nValue, isBigEndian());
}

void XMACH::_setSegment32_flags(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command, flags), nValue, isBigEndian());
}

void XMACH::_setSegment64_segname(qint64 nOffset, const QString &sValue)
{
    write_ansiStringFix(nOffset + offsetof(XMACH_DEF::segment_command_64, segname), 16, sValue);
}

void XMACH::_setSegment64_vmaddr(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, vmaddr), nValue, isBigEndian());
}

void XMACH::_setSegment64_vmsize(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, vmsize), nValue, isBigEndian());
}

void XMACH::_setSegment64_fileoff(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, fileoff), nValue, isBigEndian());
}

void XMACH::_setSegment64_filesize(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::segment_command_64, filesize), nValue, isBigEndian());
}

void XMACH::_setSegment64_maxprot(qint64 nOffset, qint32 nValue)
{
    write_int32(nOffset + offsetof(XMACH_DEF::segment_command_64, maxprot), nValue, isBigEndian());
}

void XMACH::_setSegment64_initprot(qint64 nOffset, qint32 nValue)
{
    write_int32(nOffset + offsetof(XMACH_DEF::segment_command_64, initprot), nValue, isBigEndian());
}

void XMACH::_setSegment64_nsects(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command_64, nsects), nValue, isBigEndian());
}

void XMACH::_setSegment64_flags(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::segment_command_64, flags), nValue, isBigEndian());
}

QList<XMACH::SECTION_RECORD> XMACH::getSectionRecords()
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords();

    return getSectionRecords(&listCommandRecords);
}

QList<XMACH::SECTION_RECORD> XMACH::getSectionRecords(QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    QList<SECTION_RECORD> listResult;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        QList<COMMAND_RECORD> listLCSegments = getCommandRecords(XMACH_DEF::S_LC_SEGMENT_64, pListCommandRecords);

        qint32 nNumberOfSegments = listLCSegments.count();

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            qint64 nOffset = listLCSegments.at(i).nStructOffset;
            qint32 nNumberOfSections = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command_64, nsects), bIsBigEndian);

            if (nNumberOfSections & 0xFFFFFF00) {
                nNumberOfSections = 0;
            }

            nOffset += sizeof(XMACH_DEF::segment_command_64);

            for (qint32 j = 0; j < nNumberOfSections; j++) {
                SECTION_RECORD record = {};

                record.bIs64 = bIs64;
                record.nStructOffset = nOffset;
                record.s.section64 = _read_section_64(nOffset, bIsBigEndian);

                listResult.append(record);

                nOffset += sizeof(XMACH_DEF::section_64);
            }
        }
    } else {
        QList<COMMAND_RECORD> listLCSegments = getCommandRecords(XMACH_DEF::S_LC_SEGMENT, pListCommandRecords);

        qint32 nNumberOfSegments = listLCSegments.count();

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            qint64 nOffset = listLCSegments.at(i).nStructOffset;
            qint32 nNumberOfSections = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, nsects), bIsBigEndian);

            if (nNumberOfSections & 0xFFFFFF00) {
                nNumberOfSections = 0;
            }

            nOffset += sizeof(XMACH_DEF::segment_command);

            for (qint32 j = 0; j < nNumberOfSections; j++) {
                SECTION_RECORD record = {};

                record.bIs64 = bIs64;
                record.nStructOffset = nOffset;
                record.s.section32 = _read_section(nOffset, bIsBigEndian);

                listResult.append(record);

                nOffset += sizeof(XMACH_DEF::section);
            }
        }
    }

    return listResult;
}

XMACH_DEF::section XMACH::_read_section(qint64 nOffset, bool bIsBigEndian)
{
    XMACH_DEF::section result = {};

    read_array(nOffset + offsetof(XMACH_DEF::section, sectname), result.sectname, sizeof(result.sectname));
    read_array(nOffset + offsetof(XMACH_DEF::section, segname), result.segname, sizeof(result.segname));
    result.addr = read_uint32(nOffset + offsetof(XMACH_DEF::section, addr), bIsBigEndian);
    result.size = read_uint32(nOffset + offsetof(XMACH_DEF::section, size), bIsBigEndian);
    result.offset = read_uint32(nOffset + offsetof(XMACH_DEF::section, offset), bIsBigEndian);
    result.align = read_uint32(nOffset + offsetof(XMACH_DEF::section, align), bIsBigEndian);
    result.reloff = read_uint32(nOffset + offsetof(XMACH_DEF::section, reloff), bIsBigEndian);
    result.nreloc = read_uint32(nOffset + offsetof(XMACH_DEF::section, nreloc), bIsBigEndian);
    result.flags = read_uint32(nOffset + offsetof(XMACH_DEF::section, flags), bIsBigEndian);
    result.reserved1 = read_uint32(nOffset + offsetof(XMACH_DEF::section, reserved1), bIsBigEndian);
    result.reserved2 = read_uint32(nOffset + offsetof(XMACH_DEF::section, reserved2), bIsBigEndian);

    return result;
}

XMACH_DEF::section_64 XMACH::_read_section_64(qint64 nOffset, bool bIsBigEndian)
{
    XMACH_DEF::section_64 result = {};

    read_array(nOffset + offsetof(XMACH_DEF::section_64, sectname), result.sectname, sizeof(result.sectname));
    read_array(nOffset + offsetof(XMACH_DEF::section_64, segname), result.segname, sizeof(result.segname));
    result.addr = read_uint64(nOffset + offsetof(XMACH_DEF::section_64, addr), bIsBigEndian);
    result.size = read_uint64(nOffset + offsetof(XMACH_DEF::section_64, size), bIsBigEndian);
    result.offset = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, offset), bIsBigEndian);
    result.align = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, align), bIsBigEndian);
    result.reloff = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, reloff), bIsBigEndian);
    result.nreloc = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, nreloc), bIsBigEndian);
    result.flags = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, flags), bIsBigEndian);
    result.reserved1 = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, reserved1), bIsBigEndian);
    result.reserved2 = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, reserved2), bIsBigEndian);
    result.reserved3 = read_uint32(nOffset + offsetof(XMACH_DEF::section_64, reserved3), bIsBigEndian);

    return result;
}

void XMACH::_setSection32_sectname(qint64 nOffset, const QString &sValue)
{
    write_ansiStringFix(nOffset + offsetof(XMACH_DEF::section, sectname), 16, sValue);
}

void XMACH::_setSection32_segname(qint64 nOffset, const QString &sValue)
{
    write_ansiStringFix(nOffset + offsetof(XMACH_DEF::section, segname), 16, sValue);
}

void XMACH::_setSection32_addr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, addr), nValue, isBigEndian());
}

void XMACH::_setSection32_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, size), nValue, isBigEndian());
}

void XMACH::_setSection32_offset(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, offset), nValue, isBigEndian());
}

void XMACH::_setSection32_align(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, align), nValue, isBigEndian());
}

void XMACH::_setSection32_reloff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, reloff), nValue, isBigEndian());
}

void XMACH::_setSection32_nreloc(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, nreloc), nValue, isBigEndian());
}

void XMACH::_setSection32_flags(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, flags), nValue, isBigEndian());
}

void XMACH::_setSection32_reserved1(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, reserved1), nValue, isBigEndian());
}

void XMACH::_setSection32_reserved2(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section, reserved2), nValue, isBigEndian());
}

void XMACH::_setSection64_sectname(qint64 nOffset, const QString &sValue)
{
    write_ansiStringFix(nOffset + offsetof(XMACH_DEF::section_64, sectname), 16, sValue);
}

void XMACH::_setSection64_segname(qint64 nOffset, const QString &sValue)
{
    write_ansiStringFix(nOffset + offsetof(XMACH_DEF::section_64, segname), 16, sValue);
}

void XMACH::_setSection64_addr(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::section_64, addr), nValue, isBigEndian());
}

void XMACH::_setSection64_size(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::section_64, size), nValue, isBigEndian());
}

void XMACH::_setSection64_offset(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, offset), nValue, isBigEndian());
}

void XMACH::_setSection64_align(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, align), nValue, isBigEndian());
}

void XMACH::_setSection64_reloff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, reloff), nValue, isBigEndian());
}

void XMACH::_setSection64_nreloc(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, nreloc), nValue, isBigEndian());
}

void XMACH::_setSection64_flags(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, flags), nValue, isBigEndian());
}

void XMACH::_setSection64_reserved1(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, reserved1), nValue, isBigEndian());
}

void XMACH::_setSection64_reserved2(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, reserved2), nValue, isBigEndian());
}

void XMACH::_setSection64_reserved3(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::section_64, reserved3), nValue, isBigEndian());
}

qint64 XMACH::getSegmentHeaderSize()
{
    qint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XMACH_DEF::segment_command_64);
    } else {
        nResult = sizeof(XMACH_DEF::segment_command);
    }

    return nResult;
}

quint32 XMACH::getNumberOfSections()
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords();

    return getNumberOfSections(&listCommandRecords);
}

quint32 XMACH::getNumberOfSections(QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    quint32 nResult = 0;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        QList<COMMAND_RECORD> listLCSegments = getCommandRecords(XMACH_DEF::S_LC_SEGMENT_64, pListCommandRecords);

        qint32 nNumberOfSegments = listLCSegments.count();

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            qint64 nOffset = listLCSegments.at(i).nStructOffset;
            qint32 nNumberOfSections = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command_64, nsects), bIsBigEndian);

            nResult += nNumberOfSections;
        }
    } else {
        QList<COMMAND_RECORD> listLCSegments = getCommandRecords(XMACH_DEF::S_LC_SEGMENT, pListCommandRecords);

        qint32 nNumberOfSegments = listLCSegments.count();

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            qint64 nOffset = listLCSegments.at(i).nStructOffset;
            qint32 nNumberOfSections = read_uint32(nOffset + offsetof(XMACH_DEF::segment_command, nsects), bIsBigEndian);

            nResult += nNumberOfSections;
        }
    }

    return nResult;
}

bool XMACH::isSectionNamePresent(const QString &sName)
{
    QList<SECTION_RECORD> listSections = getSectionRecords();

    return isSectionNamePresent(sName, &listSections);
}

bool XMACH::isSectionNamePresent(const QString &sName, QList<XMACH::SECTION_RECORD> *pListSectionRecords)
{
    return (getSectionNumber(sName, pListSectionRecords) != -1);
}

qint32 XMACH::getSectionNumber(const QString &sName, QList<XMACH::SECTION_RECORD> *pListSectionRecords)
{
    qint32 nResult = -1;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        QString _sName = QString(pListSectionRecords->at(i).s.section32.sectname);  // TODO Check 64
        if (_sName.size() > 16) {
            _sName.resize(16);
        }
        if (_sName == sName) {
            nResult = i;

            break;
        }
    }

    return nResult;
}

qint32 XMACH::getSectionNumber(const QString &sName)
{
    QList<SECTION_RECORD> listSectionRecords = getSectionRecords();

    return getSectionNumber(sName, &listSectionRecords);
}

qint64 XMACH::getSectionHeaderSize()
{
    qint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XMACH_DEF::section_64);
    } else {
        nResult = sizeof(XMACH_DEF::section);
    }

    return nResult;
}

quint32 XMACH::getSectionFileOffset(quint32 nIndex, QList<SECTION_RECORD> *pListSectionRecords)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionRecords->count()) {
        if (pListSectionRecords->at(nIndex).bIs64) {
            nResult = pListSectionRecords->at(nIndex).s.section64.offset;
        } else {
            nResult = pListSectionRecords->at(nIndex).s.section32.offset;
        }
    }

    return nResult;
}

quint32 XMACH::getSectionFileSize(quint32 nIndex, QList<SECTION_RECORD> *pListSectionRecords)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionRecords->count()) {
        if (pListSectionRecords->at(nIndex).bIs64) {
            nResult = pListSectionRecords->at(nIndex).s.section64.size;
        } else {
            nResult = pListSectionRecords->at(nIndex).s.section32.size;
        }
    }

    return nResult;
}

QList<QString> XMACH::getSectionNames(QList<SECTION_RECORD> *pListSectionRecords)
{
    QList<QString> listResult;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        QString sString;

        if (pListSectionRecords->at(i).bIs64) {
            sString = pListSectionRecords->at(i).s.section64.sectname;
        } else {
            sString = pListSectionRecords->at(i).s.section32.sectname;
        }

        listResult.append(sString);
    }

    return listResult;
}

bool XMACH::isSegmentNamePresent(const QString &sName)
{
    QList<SEGMENT_RECORD> listSegmentRecords = getSegmentRecords();

    return isSegmentNamePresent(sName, &listSegmentRecords);
}

bool XMACH::isSegmentNamePresent(const QString &sName, QList<XMACH::SEGMENT_RECORD> *pListSegmentRecords)
{
    return (getSegmentNumber(sName, pListSegmentRecords) != -1);
}

qint32 XMACH::getSegmentNumber(const QString &sName, QList<XMACH::SEGMENT_RECORD> *pListSegmentRecords)
{
    qint32 nResult = -1;

    qint32 nNumberOfSegments = pListSegmentRecords->count();

    for (qint32 i = 0; i < nNumberOfSegments; i++) {
        QString _sName = QString(pListSegmentRecords->at(i).s.segment32.segname);  // TODO Check 64
        if (_sName.size() > 16) {
            _sName.resize(16);
        }
        if (_sName == sName) {
            nResult = i;

            break;
        }
    }

    return nResult;
}

qint32 XMACH::getSegmentNumber(const QString &sName)
{
    QList<SEGMENT_RECORD> listSegmentRecords = getSegmentRecords();

    return getSegmentNumber(sName, &listSegmentRecords);
}

quint32 XMACH::getLibraryCurrentVersion(const QString &sName, QList<XMACH::LIBRARY_RECORD> *pListLibraryRecords)
{
    return getLibraryRecordByName(sName, pListLibraryRecords).current_version;
}

XMACH_DEF::dyld_info_command XMACH::get_dyld_info_command()
{
    XMACH_DEF::dyld_info_command result = {};

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_DYLD_INFO_ONLY, 0);

    if (nOffset != -1) {
        result = _read_dyld_info_command(nOffset);
    }

    return result;
}

XMACH_DEF::dyld_info_command XMACH::_read_dyld_info_command(qint64 nOffset)
{
    XMACH_DEF::dyld_info_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, cmdsize), bIsBigEndian);
    result.rebase_off = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, rebase_off), bIsBigEndian);
    result.rebase_size = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, rebase_size), bIsBigEndian);
    result.bind_off = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, bind_off), bIsBigEndian);
    result.bind_size = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, bind_size), bIsBigEndian);
    result.weak_bind_off = read_int32(nOffset + offsetof(XMACH_DEF::dyld_info_command, weak_bind_off), bIsBigEndian);
    result.weak_bind_size = read_int32(nOffset + offsetof(XMACH_DEF::dyld_info_command, weak_bind_size), bIsBigEndian);
    result.lazy_bind_off = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, lazy_bind_off), bIsBigEndian);
    result.lazy_bind_size = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, lazy_bind_size), bIsBigEndian);
    result.export_off = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, export_off), bIsBigEndian);
    result.export_size = read_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, export_size), bIsBigEndian);

    return result;
}

qint64 XMACH::get_dyld_info_command_size()
{
    return sizeof(XMACH_DEF::dyld_info_command);
}

void XMACH::_set_dyld_info_command_rebase_off(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, rebase_off), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_rebase_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, rebase_size), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_bind_off(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, bind_off), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_bind_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, bind_size), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_weak_bind_off(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, weak_bind_off), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_weak_bind_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, weak_bind_size), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_lazy_bind_off(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, lazy_bind_off), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_lazy_bind_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, lazy_bind_size), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_export_off(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, export_off), nValue, isBigEndian());
}

void XMACH::_set_dyld_info_command_export_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dyld_info_command, export_size), nValue, isBigEndian());
}

QString XMACH::getModuleUUID()
{
    QString sResult;

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_UUID, 0);

    if (nOffset != -1) {
        sResult = read_UUID_bytes(nOffset + offsetof(XMACH_DEF::uuid_command, uuid));
    }

    return sResult;
}

void XMACH::setModuleUUID(const QString &sValue)
{
    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_UUID, 0);

    if (nOffset != -1) {
        write_UUID_bytes(nOffset + offsetof(XMACH_DEF::uuid_command, uuid), sValue);
    }
}

QString XMACH::getLoadDylinker()
{
    QString sResult;

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_LOAD_DYLINKER, 0);

    if (nOffset != -1) {
        qint64 nStringOffset = read_uint32(nOffset + offsetof(XMACH_DEF::dylinker_command, name));

        sResult = read_ansiString(nOffset + nStringOffset);
    }

    return sResult;
}

void XMACH::setLoadDylinker(const QString &sValue)
{
    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_LOAD_DYLINKER, 0);

    if (nOffset != -1) {
        XMACH_DEF::dylinker_command dylinker = _read_dylinker_command(nOffset);

        if (dylinker.name == sizeof(XMACH_DEF::dylinker_command)) {
            write_ansiStringFix(nOffset + dylinker.name, dylinker.cmdsize - dylinker.name - 1, sValue);
        }
    }
}

QString XMACH::getRPath()
{
    QString sResult;

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_RPATH, 0);

    if (nOffset != -1) {
        qint64 nStringOffset = read_uint32(nOffset + offsetof(XMACH_DEF::rpath_command, path));

        sResult = read_ansiString(nOffset + nStringOffset);
    }

    return sResult;
}

void XMACH::setRPath(const QString &sValue)
{
    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_RPATH, 0);

    if (nOffset != -1) {
        XMACH_DEF::rpath_command rpath = _read_rpath_command(nOffset);

        if (rpath.path == sizeof(XMACH_DEF::rpath_command)) {
            write_ansiStringFix(nOffset + rpath.path, rpath.cmdsize - rpath.path - 1, sValue);
        }
    }
}

XMACH_DEF::symtab_command XMACH::get_symtab_command()
{
    XMACH_DEF::symtab_command result = {};

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_SYMTAB, 0);

    if (nOffset != -1) {
        result = _read_symtab_command(nOffset);
    }

    return result;
}

XMACH_DEF::symtab_command XMACH::_read_symtab_command(qint64 nOffset)
{
    XMACH_DEF::symtab_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, cmdsize), bIsBigEndian);
    result.symoff = read_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, symoff), bIsBigEndian);
    result.nsyms = read_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, nsyms), bIsBigEndian);
    result.stroff = read_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, stroff), bIsBigEndian);
    result.strsize = read_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, strsize), bIsBigEndian);

    return result;
}

qint64 XMACH::get_symtab_command_size()
{
    return sizeof(XMACH_DEF::symtab_command);
}

void XMACH::_set_symtab_command_symoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, symoff), nValue, isBigEndian());
}

void XMACH::_set_symtab_command_nsyms(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, nsyms), nValue, isBigEndian());
}

void XMACH::_set_symtab_command_stroff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, stroff), nValue, isBigEndian());
}

void XMACH::_set_symtab_command_strsize(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::symtab_command, strsize), nValue, isBigEndian());
}

XMACH_DEF::dysymtab_command XMACH::get_dysymtab_command()
{
    XMACH_DEF::dysymtab_command result = {};

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_DYSYMTAB, 0);

    if (nOffset != -1) {
        result = _read_dysymtab_command(nOffset);
    }

    return result;
}

XMACH_DEF::dysymtab_command XMACH::_read_dysymtab_command(qint64 nOffset)
{
    XMACH_DEF::dysymtab_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, cmdsize), bIsBigEndian);
    result.ilocalsym = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, ilocalsym), bIsBigEndian);
    result.nlocalsym = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nlocalsym), bIsBigEndian);
    result.iextdefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, iextdefsym), bIsBigEndian);
    result.nextdefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nextdefsym), bIsBigEndian);
    result.iundefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, iundefsym), bIsBigEndian);
    result.nundefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nundefsym), bIsBigEndian);
    result.tocoff = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, tocoff), bIsBigEndian);
    result.ntoc = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, ntoc), bIsBigEndian);
    result.modtaboff = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, modtaboff), bIsBigEndian);
    result.nmodtab = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nmodtab), bIsBigEndian);
    result.extrefsymoff = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, extrefsymoff), bIsBigEndian);
    result.nextrefsyms = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nextrefsyms), bIsBigEndian);
    result.indirectsymoff = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, indirectsymoff), bIsBigEndian);
    result.nindirectsyms = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nindirectsyms), bIsBigEndian);
    result.extreloff = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, extreloff), bIsBigEndian);
    result.nextrel = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nextrel), bIsBigEndian);
    result.locreloff = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, locreloff), bIsBigEndian);
    result.nlocrel = read_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nlocrel), bIsBigEndian);

    return result;
}

qint64 XMACH::get_dysymtab_command_size()
{
    return sizeof(XMACH_DEF::dysymtab_command);
}

void XMACH::_set_dysymtab_command_ilocalsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, ilocalsym), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nlocalsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nlocalsym), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_iextdefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, iextdefsym), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nextdefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nextdefsym), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_iundefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, iundefsym), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nundefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nundefsym), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_tocoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, tocoff), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_ntoc(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, ntoc), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_modtaboff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, modtaboff), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nmodtab(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nmodtab), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_extrefsymoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, extrefsymoff), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nextrefsyms(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nextrefsyms), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_indirectsymoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, indirectsymoff), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nindirectsyms(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nindirectsyms), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_extreloff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, extreloff), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nextrel(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nextrel), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_locreloff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, locreloff), nValue, isBigEndian());
}

void XMACH::_set_dysymtab_command_nlocrel(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dysymtab_command, nlocrel), nValue, isBigEndian());
}

XMACH_DEF::version_min_command XMACH::get_version_min_command()  // TODO Iphone!!!
{
    XMACH_DEF::version_min_command result = {};

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_VERSION_MIN_MACOSX, 0);

    if (nOffset != -1) {
        result = _read_version_min_command(nOffset);
    }

    return result;
}

XMACH_DEF::version_min_command XMACH::_read_version_min_command(qint64 nOffset)
{
    XMACH_DEF::version_min_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::version_min_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::version_min_command, cmdsize), bIsBigEndian);
    result.version = read_uint32(nOffset + offsetof(XMACH_DEF::version_min_command, version), bIsBigEndian);
    result.sdk = read_uint32(nOffset + offsetof(XMACH_DEF::version_min_command, sdk), bIsBigEndian);

    return result;
}

XMACH_DEF::build_version_command XMACH::_read_build_version_command(qint64 nOffset)
{
    XMACH_DEF::build_version_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, cmdsize), bIsBigEndian);
    result.platform = read_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, platform), bIsBigEndian);
    result.minos = read_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, minos), bIsBigEndian);
    result.sdk = read_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, sdk), bIsBigEndian);
    result.ntools = read_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, ntools), bIsBigEndian);

    return result;
}

qint64 XMACH::get_version_min_command_size()
{
    return sizeof(XMACH_DEF::version_min_command);
}

void XMACH::_set_version_min_command_version(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::version_min_command, version), nValue, isBigEndian());
}

void XMACH::_set_version_min_command_sdk(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::version_min_command, sdk), nValue, isBigEndian());
}

qint64 XMACH::get_build_version_command_size()
{
    return sizeof(XMACH_DEF::build_version_command);
}

void XMACH::_set_build_version_command_platform(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, platform), nValue, isBigEndian());
}

void XMACH::_set_build_version_command_minos(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, minos), nValue, isBigEndian());
}

void XMACH::_set_build_version_command_sdk(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, sdk), nValue, isBigEndian());
}

void XMACH::_set_build_version_command_ntools(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::build_version_command, ntools), nValue, isBigEndian());
}

quint64 XMACH::getSourceVersion()
{
    quint64 nResult = 0;

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_SOURCE_VERSION, 0);

    if (nOffset != -1) {
        nResult = read_uint64(nOffset + offsetof(XMACH_DEF::source_version_command, version), isBigEndian());
    }

    return nResult;
}

void XMACH::setSourceVersion(quint64 nValue)
{
    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_SOURCE_VERSION, 0);

    if (nOffset != -1) {
        write_uint64(nOffset + offsetof(XMACH_DEF::source_version_command, version), nValue, isBigEndian());
    }
}

qint64 XMACH::get_source_version_command_size()
{
    return sizeof(XMACH_DEF::source_version_command);
}

void XMACH::_set_encryption_info_command_64_cryptoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cryptoff), nValue, isBigEndian());
}

void XMACH::_set_encryption_info_command_64_cryptsize(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cryptsize), nValue, isBigEndian());
}

void XMACH::_set_encryption_info_command_64_cryptid(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cryptid), nValue, isBigEndian());
}

void XMACH::_set_encryption_info_command_64_pad(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, pad), nValue, isBigEndian());
}

qint64 XMACH::get_encryption_info_command_size()
{
    return sizeof(XMACH_DEF::encryption_info_command);
}

qint64 XMACH::get_encryption_info_command_64_size()
{
    return sizeof(XMACH_DEF::encryption_info_command_64);
}

void XMACH::_set_linkedit_data_command_dataoff(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::linkedit_data_command, dataoff), nValue, isBigEndian());
}

void XMACH::_set_linkedit_data_command_datasize(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::linkedit_data_command, datasize), nValue, isBigEndian());
}

qint64 XMACH::get_linkedit_data_command_size()
{
    return sizeof(XMACH_DEF::linkedit_data_command);
}

void XMACH::_set_entry_point_command_entryoff(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::entry_point_command, entryoff), nValue, isBigEndian());
}

void XMACH::_set_entry_point_command_stacksize(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::entry_point_command, stacksize), nValue, isBigEndian());
}

qint64 XMACH::get_entry_point_command_size()
{
    return sizeof(XMACH_DEF::entry_point_command);
}

void XMACH::_set_unix_thread_command_flavor(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::unix_thread_command, flavor), nValue, isBigEndian());
}

void XMACH::_set_unix_thread_command_count(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::unix_thread_command, count), nValue, isBigEndian());
}

qint64 XMACH::get_unix_thread_command_size()
{
    return sizeof(XMACH_DEF::unix_thread_command);
}

void XMACH::_set_x86_thread_state32_t_eax(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eax), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_ebx(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ebx), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_ecx(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ecx), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_edx(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, edx), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_edi(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, edi), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_esi(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, esi), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_ebp(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ebp), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_esp(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, esp), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_ss(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ss), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_eflags(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eflags), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_eip(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eip), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_cs(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, cs), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_ds(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ds), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_es(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, es), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_fs(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, fs), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state32_t_gs(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, gs), nValue, isBigEndian());
}

qint64 XMACH::get_x86_thread_state32_t_size()
{
    return sizeof(XMACH_DEF::x86_thread_state32_t);
}

void XMACH::_set_x86_thread_state64_t_rax(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rax), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rbx(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rbx), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rcx(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rcx), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rdx(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rdx), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rdi(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rdi), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rsi(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rsi), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rbp(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rbp), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rsp(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rsp), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r8(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r8), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r9(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r9), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r10(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r10), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r11(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r11), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r12(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r12), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r13(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r13), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r14(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r14), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_r15(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r15), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rip(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rip), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_rflags(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rflags), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_cs(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, cs), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_fs(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, fs), nValue, isBigEndian());
}

void XMACH::_set_x86_thread_state64_t_gs(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, gs), nValue, isBigEndian());
}

qint64 XMACH::get_x86_thread_state64_t_size()
{
    return sizeof(XMACH_DEF::x86_thread_state64_t);
}

void XMACH::_set_ppc_thread_state32_t_srr0(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, srr0), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_srr1(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, srr1), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_r(qint64 nOffset, quint32 nValue, qint32 nIndex)
{
    if (nIndex < 32) {
        write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, r) + sizeof(quint32) * nIndex, nValue, isBigEndian());
    }
}

void XMACH::_set_ppc_thread_state32_t_ct(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, ct), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_xer(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, xer), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_lr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, lr), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_ctr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, ctr), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_mq(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, mq), nValue, isBigEndian());
}

void XMACH::_set_ppc_thread_state32_t_vrsave(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, vrsave), nValue, isBigEndian());
}

qint64 XMACH::get_ppc_thread_state32_t_size()
{
    return sizeof(XMACH_DEF::ppc_thread_state32_t);
}

void XMACH::_set_arm_thread_state32_t_r(qint64 nOffset, quint32 nValue, qint32 nIndex)
{
    if (nIndex < 13) {
        write_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, r) + sizeof(quint32) * nIndex, nValue, isBigEndian());
    }
}

void XMACH::_set_arm_thread_state32_t_sp(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, sp), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state32_t_lr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, lr), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state32_t_pc(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, pc), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state32_t_cpsr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, cpsr), nValue, isBigEndian());
}

qint64 XMACH::get_arm_thread_state32_t_size()
{
    return sizeof(XMACH_DEF::arm_thread_state32_t);
}

void XMACH::_set_m68k_thread_state32_t_dreg(qint64 nOffset, quint32 nValue, qint32 nIndex)
{
    if (nIndex < 8) {
        write_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, dreg) + sizeof(quint32) * nIndex, nValue, isBigEndian());
    }
}

void XMACH::_set_m68k_thread_state32_t_areg(qint64 nOffset, quint32 nValue, qint32 nIndex)
{
    if (nIndex < 8) {
        write_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, areg) + sizeof(quint32) * nIndex, nValue, isBigEndian());
    }
}

void XMACH::_set_m68k_thread_state32_t_pad0(qint64 nOffset, quint16 nValue)
{
    write_uint16(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, pad0), nValue, isBigEndian());
}

void XMACH::_set_m68k_thread_state32_t_sr(qint64 nOffset, quint16 nValue)
{
    write_uint16(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, sr), nValue, isBigEndian());
}

void XMACH::_set_m68k_thread_state32_t_pc(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, pc), nValue, isBigEndian());
}

qint64 XMACH::get_m68k_thread_state32_t_size()
{
    return sizeof(XMACH_DEF::m68k_thread_state32_t);
}

void XMACH::_set_arm_thread_state64_t_x(qint64 nOffset, quint64 nValue, qint32 nIndex)
{
    if (nIndex < 29) {
        write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, x) + sizeof(quint64) * nIndex, nValue, isBigEndian());
    }
}

void XMACH::_set_arm_thread_state64_t_fp(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, fp), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state64_t_lr(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, lr), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state64_t_sp(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, sp), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state64_t_pc(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, pc), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state64_t_cpsr(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, cpsr), nValue, isBigEndian());
}

void XMACH::_set_arm_thread_state64_t_pad(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, pad), nValue, isBigEndian());
}

qint64 XMACH::get_arm_thread_state64_t_size()
{
    return sizeof(XMACH_DEF::arm_thread_state64_t);
}

void XMACH::_set_nlist_n_strx(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::nlist, n_strx), nValue, isBigEndian());
}

void XMACH::_set_nlist_n_type(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XMACH_DEF::nlist, n_type), nValue);
}

void XMACH::_set_nlist_n_sect(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XMACH_DEF::nlist, n_sect), nValue);
}

void XMACH::_set_nlist_n_desc(qint64 nOffset, quint16 nValue)
{
    write_uint16(nOffset + offsetof(XMACH_DEF::nlist, n_desc), nValue, isBigEndian());
}

void XMACH::_set_nlist_n_value(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::nlist, n_value), nValue, isBigEndian());
}

qint64 XMACH::get_nlist_size()
{
    return sizeof(XMACH_DEF::nlist);
}

void XMACH::_set_nlist_64_n_strx(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::nlist_64, n_strx), nValue, isBigEndian());
}

void XMACH::_set_nlist_64_n_type(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XMACH_DEF::nlist_64, n_type), nValue);
}

void XMACH::_set_nlist_64_n_sect(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XMACH_DEF::nlist_64, n_sect), nValue);
}

void XMACH::_set_nlist_64_n_desc(qint64 nOffset, quint16 nValue)
{
    write_uint16(nOffset + offsetof(XMACH_DEF::nlist_64, n_desc), nValue, isBigEndian());
}

void XMACH::_set_nlist_64_n_value(qint64 nOffset, quint64 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::nlist_64, n_value), nValue, isBigEndian());
}

qint64 XMACH::get_nlist_64_size()
{
    return sizeof(XMACH_DEF::nlist_64);
}

qint64 XMACH::get_nlist_MODE_size()
{
    qint64 nResult = 0;

    if (is64()) {
        nResult = get_nlist_64_size();
    } else {
        nResult = get_nlist_size();
    }

    return nResult;
}

void XMACH::_set_data_in_code_entry_offset(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::data_in_code_entry, offset), nValue, isBigEndian());
}

void XMACH::_set_data_in_code_entry_length(qint64 nOffset, quint16 nValue)
{
    write_uint16(nOffset + offsetof(XMACH_DEF::data_in_code_entry, length), nValue, isBigEndian());
}

void XMACH::_set_data_in_code_entry_kind(qint64 nOffset, quint16 nValue)
{
    write_uint16(nOffset + offsetof(XMACH_DEF::data_in_code_entry, kind), nValue, isBigEndian());
}

qint64 XMACH::get_data_in_code_entry_size()
{
    return sizeof(XMACH_DEF::data_in_code_entry);
}

void XMACH::_set_dylib_module_module_name(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, module_name), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_iextdefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, iextdefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_nextdefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nextdefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_irefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, irefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_nrefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nrefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_ilocalsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, ilocalsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_nlocalsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nlocalsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_iextrel(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, iextrel), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_nextrel(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nextrel), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_iinit_iterm(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, iinit_iterm), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_ninit_nterm(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, ninit_nterm), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_objc_module_info_addr(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, objc_module_info_addr), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_objc_module_info_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, objc_module_info_size), nValue, isBigEndian());
}

qint64 XMACH::get_dylib_module_size()
{
    return sizeof(XMACH_DEF::dylib_module);
}

void XMACH::_set_dylib_module_64_module_name(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, module_name), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_iextdefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, iextdefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_nextdefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nextdefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_irefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, irefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_nrefsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nrefsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_ilocalsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, ilocalsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_nlocalsym(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nlocalsym), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_iextrel(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, iextrel), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_nextrel(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nextrel), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_iinit_iterm(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, iinit_iterm), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_ninit_nterm(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, ninit_nterm), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_objc_module_info_size(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, objc_module_info_size), nValue, isBigEndian());
}

void XMACH::_set_dylib_module_64_objc_module_info_addr(qint64 nOffset, quint32 nValue)
{
    write_uint64(nOffset + offsetof(XMACH_DEF::dylib_module_64, objc_module_info_addr), nValue, isBigEndian());
}

qint64 XMACH::get_dylib_module_64_size()
{
    return sizeof(XMACH_DEF::dylib_module_64);
}

void XMACH::_set_dylib_table_of_contents_symbol_index(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_table_of_contents, symbol_index), nValue, isBigEndian());
}

void XMACH::_set_dylib_table_of_contents_module_index(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::dylib_table_of_contents, module_index), nValue, isBigEndian());
}

qint64 XMACH::get_dylib_table_of_contents_size()
{
    return sizeof(XMACH_DEF::dylib_table_of_contents);
}

void XMACH::_set_relocation_info_r_address(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::relocation_info, r_address), nValue, isBigEndian());
}

void XMACH::_set_relocation_info_value(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::relocation_info, s), nValue, isBigEndian());
}

qint64 XMACH::get_relocation_info_size()
{
    return sizeof(XMACH_DEF::relocation_info);
}

void XMACH::_set_value32_value(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset, nValue, isBigEndian());
}

qint64 XMACH::get_value32_size()
{
    return sizeof(quint32);
}

qint64 XMACH::get_dylib_reference_size()
{
    return sizeof(XMACH_DEF::dylib_reference);
}

void XMACH::_set_SC_SuperBlob_magic(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::__SC_SuperBlob, magic), nValue, true);
}

void XMACH::_set_SC_SuperBlob_length(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::__SC_SuperBlob, length), nValue, true);
}

void XMACH::_set_SC_SuperBlob_count(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::__SC_SuperBlob, count), nValue, true);
}

qint64 XMACH::get_SC_SuperBlob_size()
{
    return sizeof(XMACH_DEF::__SC_SuperBlob);
}

void XMACH::_set_BlobIndex_type(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::__BlobIndex, type), nValue, true);
}

void XMACH::_set_BlobIndex_offset(qint64 nOffset, quint32 nValue)
{
    write_uint32(nOffset + offsetof(XMACH_DEF::__BlobIndex, offset), nValue, true);
}

qint64 XMACH::get_BlobIndex_size()
{
    return sizeof(XMACH_DEF::__BlobIndex);
}

XMACH_DEF::dylinker_command XMACH::_read_dylinker_command(qint64 nOffset)
{
    XMACH_DEF::dylinker_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::dylinker_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::dylinker_command, cmdsize), bIsBigEndian);
    result.name = read_uint32(nOffset + offsetof(XMACH_DEF::dylinker_command, name), bIsBigEndian);

    return result;
}

XMACH_DEF::rpath_command XMACH::_read_rpath_command(qint64 nOffset)
{
    XMACH_DEF::rpath_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::rpath_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::rpath_command, cmdsize), bIsBigEndian);
    result.path = read_uint32(nOffset + offsetof(XMACH_DEF::rpath_command, path), bIsBigEndian);

    return result;
}

XMACH_DEF::source_version_command XMACH::_read_source_version_command(qint64 nOffset)
{
    XMACH_DEF::source_version_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::source_version_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::source_version_command, cmdsize), bIsBigEndian);
    result.version = read_uint64(nOffset + offsetof(XMACH_DEF::source_version_command, version), bIsBigEndian);

    return result;
}

XMACH_DEF::encryption_info_command XMACH::_read_encryption_info_command(qint64 nOffset)
{
    XMACH_DEF::encryption_info_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command, cmdsize), bIsBigEndian);
    result.cryptoff = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command, cryptoff), bIsBigEndian);
    result.cryptsize = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command, cryptsize), bIsBigEndian);
    result.cryptid = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command, cryptid), bIsBigEndian);

    return result;
}

XMACH_DEF::encryption_info_command_64 XMACH::_read_encryption_info_command_64(qint64 nOffset)
{
    XMACH_DEF::encryption_info_command_64 result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cmdsize), bIsBigEndian);
    result.cryptoff = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cryptoff), bIsBigEndian);
    result.cryptsize = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cryptsize), bIsBigEndian);
    result.cryptid = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, cryptid), bIsBigEndian);
    result.pad = read_uint32(nOffset + offsetof(XMACH_DEF::encryption_info_command_64, pad), bIsBigEndian);

    return result;
}

XMACH_DEF::entry_point_command XMACH::_read_entry_point_command(qint64 nOffset)
{
    XMACH_DEF::entry_point_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::entry_point_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::entry_point_command, cmdsize), bIsBigEndian);
    result.entryoff = read_uint32(nOffset + offsetof(XMACH_DEF::entry_point_command, entryoff), bIsBigEndian);
    result.stacksize = read_uint32(nOffset + offsetof(XMACH_DEF::entry_point_command, stacksize), bIsBigEndian);

    return result;
}

XMACH_DEF::x86_thread_state32_t XMACH::_read_x86_thread_state32_t(qint64 nOffset)
{
    XMACH_DEF::x86_thread_state32_t result = {};

    bool bIsBigEndian = isBigEndian();

    result.eax = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eax), bIsBigEndian);
    result.ebx = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ebx), bIsBigEndian);
    result.ecx = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ecx), bIsBigEndian);
    result.edx = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, edx), bIsBigEndian);
    result.edi = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, edi), bIsBigEndian);
    result.esi = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, esi), bIsBigEndian);
    result.ebp = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ebp), bIsBigEndian);
    result.esp = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, esp), bIsBigEndian);
    result.ss = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ss), bIsBigEndian);
    result.eflags = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eflags), bIsBigEndian);
    result.eip = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, eip), bIsBigEndian);
    result.cs = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, cs), bIsBigEndian);
    result.ds = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, ds), bIsBigEndian);
    result.es = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, es), bIsBigEndian);
    result.fs = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, fs), bIsBigEndian);
    result.gs = read_uint32(nOffset + offsetof(XMACH_DEF::x86_thread_state32_t, gs), bIsBigEndian);

    return result;
}

XMACH_DEF::x86_thread_state64_t XMACH::_read_x86_thread_state64_t(qint64 nOffset)
{
    XMACH_DEF::x86_thread_state64_t result = {};

    bool bIsBigEndian = isBigEndian();

    result.rax = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rax), bIsBigEndian);
    result.rbx = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rbx), bIsBigEndian);
    result.rcx = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rcx), bIsBigEndian);
    result.rdx = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rdx), bIsBigEndian);
    result.rdi = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rdi), bIsBigEndian);
    result.rsi = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rsi), bIsBigEndian);
    result.rbp = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rbp), bIsBigEndian);
    result.rsp = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rsp), bIsBigEndian);
    result.r8 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r8), bIsBigEndian);
    result.r9 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r9), bIsBigEndian);
    result.r10 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r10), bIsBigEndian);
    result.r11 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r11), bIsBigEndian);
    result.r12 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r12), bIsBigEndian);
    result.r13 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r13), bIsBigEndian);
    result.r14 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r14), bIsBigEndian);
    result.r15 = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, r15), bIsBigEndian);
    result.rip = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rip), bIsBigEndian);
    result.rflags = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, rflags), bIsBigEndian);
    result.cs = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, cs), bIsBigEndian);
    result.fs = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, fs), bIsBigEndian);
    result.gs = read_uint64(nOffset + offsetof(XMACH_DEF::x86_thread_state64_t, gs), bIsBigEndian);

    return result;
}

XMACH_DEF::ppc_thread_state32_t XMACH::_read_ppc_thread_state32_t(qint64 nOffset)
{
    XMACH_DEF::ppc_thread_state32_t result = {};

    bool bIsBigEndian = isBigEndian();

    result.srr0 = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, srr0), bIsBigEndian);
    result.srr1 = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, srr1), bIsBigEndian);

    for (qint32 i = 0; i < 32; i++) {
        result.r[i] = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, r) + sizeof(quint32) * i, bIsBigEndian);
    }

    result.ct = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, ct), bIsBigEndian);
    result.xer = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, xer), bIsBigEndian);
    result.lr = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, lr), bIsBigEndian);
    result.ctr = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, ctr), bIsBigEndian);
    result.mq = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, mq), bIsBigEndian);
    result.vrsave = read_uint32(nOffset + offsetof(XMACH_DEF::ppc_thread_state32_t, vrsave), bIsBigEndian);

    return result;
}

XMACH_DEF::arm_thread_state32_t XMACH::_read_arm_thread_state32_t(qint64 nOffset)
{
    XMACH_DEF::arm_thread_state32_t result = {};

    bool bIsBigEndian = isBigEndian();

    for (qint32 i = 0; i < 13; i++) {
        result.r[i] = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, r) + sizeof(quint32) * i, bIsBigEndian);
    }
    result.sp = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, sp), bIsBigEndian);
    result.lr = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, lr), bIsBigEndian);
    result.pc = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, pc), bIsBigEndian);
    result.cpsr = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state32_t, cpsr), bIsBigEndian);

    return result;
}

XMACH_DEF::arm_thread_state64_t XMACH::_read_arm_thread_state64_t(qint64 nOffset)
{
    XMACH_DEF::arm_thread_state64_t result = {};

    bool bIsBigEndian = isBigEndian();

    for (qint32 i = 0; i < 29; i++) {
        result.x[i] = read_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, x) + sizeof(quint64) * i, bIsBigEndian);
    }
    result.sp = read_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, sp), bIsBigEndian);
    result.lr = read_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, lr), bIsBigEndian);
    result.sp = read_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, sp), bIsBigEndian);
    result.pc = read_uint64(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, pc), bIsBigEndian);
    result.cpsr = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, cpsr), bIsBigEndian);
    result.pad = read_uint32(nOffset + offsetof(XMACH_DEF::arm_thread_state64_t, pad), bIsBigEndian);

    return result;
}

XMACH_DEF::m68k_thread_state32_t XMACH::_read_m68k_thread_state32_t(qint64 nOffset)
{
    XMACH_DEF::m68k_thread_state32_t result = {};

    bool bIsBigEndian = isBigEndian();

    for (qint32 i = 0; i < 8; i++) {
        result.dreg[i] = read_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, dreg) + sizeof(quint32) * i, bIsBigEndian);
    }

    for (qint32 i = 0; i < 8; i++) {
        result.areg[i] = read_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, areg) + sizeof(quint32) * i, bIsBigEndian);
    }

    result.pad0 = read_uint16(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, pad0), bIsBigEndian);
    result.sr = read_uint16(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, sr), bIsBigEndian);
    result.pc = read_uint32(nOffset + offsetof(XMACH_DEF::m68k_thread_state32_t, pc), bIsBigEndian);

    return result;
}

XMACH_DEF::state_hdr_t XMACH::_read_state_hdr_t(qint64 nOffset)
{
    XMACH_DEF::state_hdr_t result = {};

    bool bIsBigEndian = isBigEndian();

    result.flavor = read_uint32(nOffset + offsetof(XMACH_DEF::state_hdr_t, flavor), bIsBigEndian);
    result.count = read_uint32(nOffset + offsetof(XMACH_DEF::state_hdr_t, count), bIsBigEndian);

    return result;
}

XMACH_DEF::linkedit_data_command XMACH::_read_linkedit_data_command(qint64 nOffset)
{
    XMACH_DEF::linkedit_data_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::linkedit_data_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::linkedit_data_command, cmdsize), bIsBigEndian);
    result.dataoff = read_uint32(nOffset + offsetof(XMACH_DEF::linkedit_data_command, dataoff), bIsBigEndian);
    result.datasize = read_uint32(nOffset + offsetof(XMACH_DEF::linkedit_data_command, datasize), bIsBigEndian);

    return result;
}

XMACH_DEF::unix_thread_command XMACH::_read_unix_thread_command(qint64 nOffset)
{
    XMACH_DEF::unix_thread_command result = {};

    bool bIsBigEndian = isBigEndian();

    result.cmd = read_uint32(nOffset + offsetof(XMACH_DEF::unix_thread_command, cmd), bIsBigEndian);
    result.cmdsize = read_uint32(nOffset + offsetof(XMACH_DEF::unix_thread_command, cmdsize), bIsBigEndian);
    result.flavor = read_uint32(nOffset + offsetof(XMACH_DEF::unix_thread_command, flavor), bIsBigEndian);
    result.count = read_uint32(nOffset + offsetof(XMACH_DEF::unix_thread_command, count), bIsBigEndian);

    return result;
}

XMACH_DEF::nlist XMACH::_read_nlist(qint64 nOffset)
{
    XMACH_DEF::nlist result = {};

    bool bIsBigEndian = isBigEndian();

    result.n_strx = read_uint32(nOffset + offsetof(XMACH_DEF::nlist, n_strx), bIsBigEndian);
    result.n_type = read_uint8(nOffset + offsetof(XMACH_DEF::nlist, n_type));
    result.n_sect = read_uint8(nOffset + offsetof(XMACH_DEF::nlist, n_sect));
    result.n_desc = read_uint16(nOffset + offsetof(XMACH_DEF::nlist, n_desc), bIsBigEndian);
    result.n_value = read_uint32(nOffset + offsetof(XMACH_DEF::nlist, n_value), bIsBigEndian);

    return result;
}

XMACH_DEF::nlist_64 XMACH::_read_nlist_64(qint64 nOffset)
{
    XMACH_DEF::nlist_64 result = {};

    bool bIsBigEndian = isBigEndian();

    result.n_strx = read_uint32(nOffset + offsetof(XMACH_DEF::nlist_64, n_strx), bIsBigEndian);
    result.n_type = read_uint8(nOffset + offsetof(XMACH_DEF::nlist_64, n_type));
    result.n_sect = read_uint8(nOffset + offsetof(XMACH_DEF::nlist_64, n_sect));
    result.n_desc = read_uint16(nOffset + offsetof(XMACH_DEF::nlist_64, n_desc), bIsBigEndian);
    result.n_value = read_uint64(nOffset + offsetof(XMACH_DEF::nlist_64, n_value), bIsBigEndian);

    return result;
}

XMACH_DEF::data_in_code_entry XMACH::_read_data_in_code_entry(qint64 nOffset)
{
    XMACH_DEF::data_in_code_entry result = {};

    bool bIsBigEndian = isBigEndian();

    result.offset = read_uint32(nOffset + offsetof(XMACH_DEF::data_in_code_entry, offset), bIsBigEndian);
    result.length = read_uint16(nOffset + offsetof(XMACH_DEF::data_in_code_entry, length), bIsBigEndian);
    result.kind = read_uint16(nOffset + offsetof(XMACH_DEF::data_in_code_entry, kind), bIsBigEndian);

    return result;
}

struct dylib_module_64 {
    quint32 module_name;
    quint32 iextdefsym;
    quint32 nextdefsym;
    quint32 irefsym;
    quint32 nrefsym;
    quint32 ilocalsym;
    quint32 nlocalsym;
    quint32 iextrel;
    quint32 nextrel;
    quint32 iinit_iterm;
    quint32 ninit_nterm;
    quint32 objc_module_info_size;
    quint64 objc_module_info_addr;
};

XMACH_DEF::dylib_module XMACH::_read_dylib_module(qint64 nOffset)
{
    XMACH_DEF::dylib_module result = {};

    bool bIsBigEndian = isBigEndian();

    result.module_name = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, module_name), bIsBigEndian);
    result.iextdefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, iextdefsym), bIsBigEndian);
    result.nextdefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nextdefsym), bIsBigEndian);
    result.irefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, irefsym), bIsBigEndian);
    result.nrefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nrefsym), bIsBigEndian);
    result.ilocalsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, ilocalsym), bIsBigEndian);
    result.nlocalsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nlocalsym), bIsBigEndian);
    result.iextrel = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, iextrel), bIsBigEndian);
    result.nextrel = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, nextrel), bIsBigEndian);
    result.iinit_iterm = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, iinit_iterm), bIsBigEndian);
    result.ninit_nterm = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, ninit_nterm), bIsBigEndian);
    result.objc_module_info_addr = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, objc_module_info_addr), bIsBigEndian);
    result.objc_module_info_size = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module, objc_module_info_size), bIsBigEndian);

    return result;
}

XMACH_DEF::dylib_module_64 XMACH::_read_dylib_module_64(qint64 nOffset)
{
    XMACH_DEF::dylib_module_64 result = {};

    bool bIsBigEndian = isBigEndian();

    result.module_name = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, module_name), bIsBigEndian);
    result.iextdefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, iextdefsym), bIsBigEndian);
    result.nextdefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nextdefsym), bIsBigEndian);
    result.irefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, irefsym), bIsBigEndian);
    result.nrefsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nrefsym), bIsBigEndian);
    result.ilocalsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, ilocalsym), bIsBigEndian);
    result.nlocalsym = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nlocalsym), bIsBigEndian);
    result.iextrel = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, iextrel), bIsBigEndian);
    result.nextrel = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, nextrel), bIsBigEndian);
    result.iinit_iterm = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, iinit_iterm), bIsBigEndian);
    result.ninit_nterm = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, ninit_nterm), bIsBigEndian);
    result.objc_module_info_size = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_module_64, objc_module_info_size), bIsBigEndian);
    result.objc_module_info_addr = read_uint64(nOffset + offsetof(XMACH_DEF::dylib_module_64, objc_module_info_addr), bIsBigEndian);

    return result;
}

XMACH_DEF::dylib_table_of_contents XMACH::_read_dylib_table_of_contents(qint64 nOffset)
{
    bool bIsBigEndian = isBigEndian();

    return _read_dylib_table_of_contents(nOffset, bIsBigEndian);
}

XMACH_DEF::dylib_table_of_contents XMACH::_read_dylib_table_of_contents(qint64 nOffset, bool bIsBigEndian)
{
    XMACH_DEF::dylib_table_of_contents result = {};

    result.symbol_index = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_table_of_contents, symbol_index), bIsBigEndian);
    result.module_index = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_table_of_contents, module_index), bIsBigEndian);

    return result;
}

XMACH_DEF::relocation_info XMACH::_read_relocation_info(qint64 nOffset)
{
    XMACH_DEF::relocation_info result = {};

    bool bIsBigEndian = isBigEndian();

    result.r_address = read_uint32(nOffset + offsetof(XMACH_DEF::relocation_info, r_address), bIsBigEndian);
    result.s.value = read_uint32(nOffset + offsetof(XMACH_DEF::relocation_info, s), bIsBigEndian);

    return result;
}

XMACH_DEF::dylib_reference XMACH::_read_dylib_reference(qint64 nOffset)
{
    XMACH_DEF::dylib_reference result = {};

    bool bIsBigEndian = isBigEndian();

    result.s.value = read_uint32(nOffset + offsetof(XMACH_DEF::dylib_reference, s), bIsBigEndian);

    return result;
}

XMACH_DEF::__SC_SuperBlob XMACH::_read_SC_SuperBlob(qint64 nOffset)
{
    XMACH_DEF::__SC_SuperBlob result = {};

    bool bIsBigEndian = true;

    result.magic = read_uint32(nOffset + offsetof(XMACH_DEF::__SC_SuperBlob, magic), bIsBigEndian);
    result.length = read_uint32(nOffset + offsetof(XMACH_DEF::__SC_SuperBlob, length), bIsBigEndian);
    result.count = read_uint32(nOffset + offsetof(XMACH_DEF::__SC_SuperBlob, count), bIsBigEndian);

    return result;
}

XMACH_DEF::__BlobIndex XMACH::_read_BlobIndex(qint64 nOffset)
{
    XMACH_DEF::__BlobIndex result = {};

    bool bIsBigEndian = true;

    result.type = read_uint32(nOffset + offsetof(XMACH_DEF::__BlobIndex, type), bIsBigEndian);
    result.offset = read_uint32(nOffset + offsetof(XMACH_DEF::__BlobIndex, offset), bIsBigEndian);

    return result;
}

QList<XMACH::NLIST_RECORD> XMACH::getNlistRecords()
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_SYMTAB);

    return getNlistRecords(&listCommandRecords);
}

QList<XMACH::NLIST_RECORD> XMACH::getNlistRecords(QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    QList<NLIST_RECORD> listResult;

    bool bIs64 = is64();

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_SYMTAB, 0, pListCommandRecords);

    if (nOffset != -1) {
        XMACH_DEF::symtab_command symtab = _read_symtab_command(nOffset);

        nOffset = symtab.symoff;

        for (qint32 i = 0; i < (int)(symtab.nsyms); i++) {
            NLIST_RECORD record = {};

            record.nStructOffset = nOffset;
            record.bIs64 = bIs64;

            if (bIs64) {
                record.s.nlist64 = _read_nlist_64(nOffset);

                nOffset += get_nlist_64_size();

                if (record.s.nlist64.n_strx == 0) {
                    break;
                }
            } else {
                record.s.nlist32 = _read_nlist(nOffset);

                nOffset += get_nlist_size();

                if (record.s.nlist32.n_strx == 0) {
                    break;
                }
            }

            listResult.append(record);
        }
    }

    return listResult;
}

XMACH::NLIST_RECORD XMACH::searchNlistRecordByValue(QList<XMACH::NLIST_RECORD> *pList, quint64 nValue, bool bValidName)
{
    XMACH::NLIST_RECORD result = {};

    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        bool bSuccess = false;

        if (pList->at(i).bIs64) {
            if (pList->at(i).s.nlist64.n_value == nValue) {
                bSuccess = true;
            }
        } else {
            if (pList->at(i).s.nlist32.n_value == (quint32)nValue) {
                bSuccess = true;
            }
        }

        if (bSuccess) {
            if (bValidName) {
                bSuccess = false;

                if (pList->at(i).bIs64) {
                    if (pList->at(i).s.nlist64.n_strx > 1) {
                        bSuccess = true;
                    }
                } else {
                    if (pList->at(i).s.nlist32.n_strx > 1) {
                        bSuccess = true;
                    }
                }
            }
        }

        if (bSuccess) {
            result = pList->at(i);
            break;
        }
    }

    return result;
}

QList<XMACH::TOC_RECORD> XMACH::get_toc_list()
{
    QList<TOC_RECORD> listResult;

    XMACH_DEF::dysymtab_command dysymtab = get_dysymtab();
    bool bIsBigEndian = isBigEndian();

    qint64 nOffset = dysymtab.tocoff;
    qint32 nNumberOfRecords = dysymtab.ntoc;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        TOC_RECORD record = {};

        record.nStructOffset = nOffset;

        record.toc = _read_dylib_table_of_contents(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += get_dylib_table_of_contents_size();
    }

    return listResult;
}

QList<XMACH::MODTAB_RECORD> XMACH::get_modtab_list()
{
    QList<MODTAB_RECORD> listResult;

    XMACH_DEF::dysymtab_command dysymtab = get_dysymtab();

    bool bIs64 = is64();

    qint64 nOffset = dysymtab.modtaboff;
    qint32 nNumberOfRecords = dysymtab.nmodtab;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        MODTAB_RECORD record = {};

        record.nStructOffset = nOffset;
        record.bIs64 = bIs64;

        if (bIs64) {
            record.s.module64 = _read_dylib_module_64(nOffset);
        } else {
            record.s.module32 = _read_dylib_module(nOffset);
        }

        listResult.append(record);

        if (bIs64) {
            nOffset += get_dylib_module_64_size();
        } else {
            nOffset += get_dylib_module_size();
        }
    }

    return listResult;
}

QList<XMACH::REFERENCE_RECORD> XMACH::get_extrefsyms_list()
{
    QList<REFERENCE_RECORD> listResult;

    XMACH_DEF::dysymtab_command dysymtab = get_dysymtab();
    bool bIsBigEndian = isBigEndian();

    qint64 nOffset = dysymtab.extrefsymoff;
    qint32 nNumberOfRecords = dysymtab.nextrefsyms;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        REFERENCE_RECORD record = {};
        record.nStructOffset = nOffset;
        record.reference.s.value = read_uint32(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += get_dylib_reference_size();
    }

    return listResult;
}

QList<XMACH::VALUE32_RECORD> XMACH::get_indirectsyms_list()
{
    QList<VALUE32_RECORD> listResult;

    XMACH_DEF::dysymtab_command dysymtab = get_dysymtab();

    listResult = getValue32Records(dysymtab.indirectsymoff, dysymtab.nindirectsyms);

    return listResult;
}

QList<XMACH::RELOC_RECORD> XMACH::get_extrel_list()
{
    QList<RELOC_RECORD> listResult;

    XMACH_DEF::dysymtab_command dysymtab = get_dysymtab();

    listResult = getRelocRecords(dysymtab.extreloff, dysymtab.nextrel);

    return listResult;
}

QList<XMACH::RELOC_RECORD> XMACH::get_locrel_list()
{
    QList<RELOC_RECORD> listResult;

    XMACH_DEF::dysymtab_command dysymtab = get_dysymtab();

    listResult = getRelocRecords(dysymtab.locreloff, dysymtab.nlocrel);

    return listResult;
}

QList<XMACH::RELOC_RECORD> XMACH::getRelocRecords(qint64 nOffset, qint32 nNumberOfRecords)
{
    QList<XMACH::RELOC_RECORD> listResult;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        RELOC_RECORD record = {};

        record.nStructOffset = nOffset;

        record.reloc = _read_relocation_info(nOffset);

        listResult.append(record);

        nOffset += get_relocation_info_size();
    }

    return listResult;
}

QList<XMACH::VALUE32_RECORD> XMACH::getValue32Records(qint64 nOffset, qint32 nNumberOfRecords)
{
    QList<VALUE32_RECORD> listResult;
    bool bIsBigEndian = isBigEndian();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        VALUE32_RECORD record = {};
        record.nStructOffset = nOffset;
        record.nValue = read_uint32(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += get_value32_size();
    }

    return listResult;
}

XBinary::OFFSETSIZE XMACH::getStringTableOffsetSize()
{
    QList<COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_SYMTAB);

    return getStringTableOffsetSize(&listCommandRecords);
}

XBinary::OFFSETSIZE XMACH::getStringTableOffsetSize(QList<XMACH::COMMAND_RECORD> *pListCommandRecords)
{
    OFFSETSIZE osResult = {};

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_SYMTAB, 0, pListCommandRecords);

    if (nOffset != -1) {
        XMACH_DEF::symtab_command symtab = _read_symtab_command(nOffset);

        osResult.nOffset = symtab.stroff;
        osResult.nSize = symtab.stroff;
    }

    return osResult;
}

XMACH_DEF::dyld_info_command XMACH::get_dyld_info()
{
    XMACH_DEF::dyld_info_command result = {};

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_DYLD_INFO_ONLY);

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_DYLD_INFO_ONLY, 0, &listCommandRecords);

    if (nOffset != -1) {
        result = _read_dyld_info_command(nOffset);
    }

    return result;
}

XMACH_DEF::symtab_command XMACH::get_symtab()
{
    XMACH_DEF::symtab_command result = {};

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_SYMTAB);

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_SYMTAB, 0, &listCommandRecords);

    if (nOffset != -1) {
        result = _read_symtab_command(nOffset);
    }

    return result;
}

XMACH_DEF::dysymtab_command XMACH::get_dysymtab()
{
    XMACH_DEF::dysymtab_command result = {};

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_DYSYMTAB);

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_DYSYMTAB, 0, &listCommandRecords);

    if (nOffset != -1) {
        result = _read_dysymtab_command(nOffset);
    }

    return result;
}

XMACH_DEF::encryption_info_command XMACH::get_encryption_info()
{
    XMACH_DEF::encryption_info_command result = {};

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_ENCRYPTION_INFO);

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_ENCRYPTION_INFO, 0, &listCommandRecords);

    if (nOffset != -1) {
        result = _read_encryption_info_command(nOffset);
    }

    return result;
}

XMACH_DEF::encryption_info_command_64 XMACH::get_encryption_info_64()
{
    XMACH_DEF::encryption_info_command_64 result = {};

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords(XMACH_DEF::S_LC_ENCRYPTION_INFO_64);

    qint64 nOffset = getCommandRecordOffset(XMACH_DEF::S_LC_ENCRYPTION_INFO_64, 0, &listCommandRecords);

    if (nOffset != -1) {
        result = _read_encryption_info_command_64(nOffset);
    }

    return result;
}

XMACH_DEF::linkedit_data_command XMACH::get_linkedit_data(quint32 nCommandID)
{
    XMACH_DEF::linkedit_data_command result = {};

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords(nCommandID);

    qint64 nOffset = getCommandRecordOffset(nCommandID, 0, &listCommandRecords);

    if (nOffset != -1) {
        result = _read_linkedit_data_command(nOffset);
    }

    return result;
}

QList<XMACH::FUNCTION_RECORD> XMACH::getFunctionRecords(qint64 nOffset, qint64 nSize)
{
    QList<FUNCTION_RECORD> listRecords;

    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    qint32 nRawOffset = 0;

    for (qint64 nCurrentOffset = nOffset; nCurrentOffset < (nOffset + nSize);) {
        PACKED_INT uleb128 = read_uleb128(nCurrentOffset, (nOffset + nSize) - nCurrentOffset);

        if ((uleb128.nValue == 0) && (nCurrentOffset != nOffset)) {
            break;
        }

        nRawOffset += uleb128.nValue;

        FUNCTION_RECORD record = {};
        record.nDataOffset = nCurrentOffset;
        record.nFunctionOffset = nRawOffset;
        record.nFunctionAddress = offsetToAddress(&memoryMap, nRawOffset);

        listRecords.append(record);

        nCurrentOffset += uleb128.nByteSize;
    }

    return listRecords;
}

QList<XMACH::DICE_RECORD> XMACH::getDiceRecords(qint64 nOffset, qint64 nSize)
{
    QList<XMACH::DICE_RECORD> listRecords;

    for (qint64 nCurrentOffset = nOffset; nCurrentOffset < (nOffset + nSize);) {
        XMACH::DICE_RECORD record = {};
        record.nStructOffset = nCurrentOffset;
        record.dice = _read_data_in_code_entry(nCurrentOffset);

        listRecords.append(record);

        nCurrentOffset += sizeof(XMACH_DEF::data_in_code_entry);
    }

    return listRecords;
}

QString XMACH::getIndexSymbolName(quint32 nValue)
{
    QList<XMACH::NLIST_RECORD> listNlist = getNlistRecords();
    XBinary::OFFSETSIZE osStringTable = getStringTableOffsetSize();

    return getIndexSymbolName(nValue, &listNlist, osStringTable.nOffset, osStringTable.nSize);
}

QString XMACH::getIndexSymbolName(quint32 nValue, QList<XMACH::NLIST_RECORD> *pNlistList, qint64 nStringTableOffset, qint64 nStringTableSize)
{
    QString sResult;

    if (nValue == XMACH_DEF::S_INDIRECT_SYMBOL_ABS) {
        sResult = "INDIRECT_SYMBOL_ABS";
    } else if (nValue == XMACH_DEF::S_INDIRECT_SYMBOL_LOCAL) {
        sResult = "INDIRECT_SYMBOL_LOCAL";
    } else if (nValue < (quint32)pNlistList->count()) {
        XMACH::NLIST_RECORD nlist_record = pNlistList->at(nValue);

        if (nlist_record.bIs64) {
            sResult = getStringFromIndex(nStringTableOffset, nStringTableSize, nlist_record.s.nlist64.n_strx);
        } else {
            sResult = getStringFromIndex(nStringTableOffset, nStringTableSize, nlist_record.s.nlist32.n_strx);
        }
    }

    return sResult;
}

XBinary::MODE XMACH::getMode()
{
    MODE result = MODE_32;

    if (getHeader_magic() == XMACH_DEF::S_MH_MAGIC_64) {
        result = MODE_64;
    } else {
        result = MODE_32;
    }

    return result;
}

QString XMACH::getArch()
{
    quint32 nCpuType = getHeader_cputype();
    quint32 nCpuSubType = getHeader_cpusubtype();

    QString sResult = getHeaderCpuTypesS().value(nCpuType, tr("Unknown"));

    if (nCpuType == XMACH_DEF::S_CPU_TYPE_MC680x0) {
        QMap<quint64, QString> mapCpuSubTypesS = getHeaderCpuSubTypesS(nCpuType);

        if (mapCpuSubTypesS.contains(nCpuSubType)) {
            sResult = mapCpuSubTypesS.value(nCpuSubType, tr("Unknown"));
        }
    } else if ((nCpuType == XMACH_DEF::S_CPU_TYPE_ARM) || (nCpuType == XMACH_DEF::S_CPU_TYPE_ARM64)) {
        QMap<quint64, QString> mapCpuSubTypesS = getHeaderCpuSubTypesS(nCpuType);

        if (nCpuSubType && (mapCpuSubTypesS.contains(nCpuSubType))) {
            sResult = mapCpuSubTypesS.value(nCpuSubType, tr("Unknown"));
        }
    }

    return sResult;
}

XBinary::FT XMACH::getFileType()
{
    FT result = FT_MACHO32;

    MODE mode = getMode();

    if (mode == MODE_32) {
        result = FT_MACHO32;
    } else if (mode == MODE_64) {
        result = FT_MACHO64;
    }

    return result;
}

qint32 XMACH::getType()
{
    qint32 nResult = TYPE_UNKNOWN;

    quint32 nFileType = getHeader_filetype();

    if (nFileType == XMACH_DEF::S_MH_OBJECT) nResult = TYPE_OBJECT;
    else if (nFileType == XMACH_DEF::S_MH_EXECUTE) nResult = TYPE_EXECUTE;
    else if (nFileType == XMACH_DEF::S_MH_FVMLIB) nResult = TYPE_FVMLIB;
    else if (nFileType == XMACH_DEF::S_MH_CORE) nResult = TYPE_CORE;
    else if (nFileType == XMACH_DEF::S_MH_PRELOAD) nResult = TYPE_PRELOAD;
    else if (nFileType == XMACH_DEF::S_MH_DYLIB) nResult = TYPE_DYLIB;
    else if (nFileType == XMACH_DEF::S_MH_DYLINKER) nResult = TYPE_DYLINKER;
    else if (nFileType == XMACH_DEF::S_MH_BUNDLE) nResult = TYPE_BUNDLE;
    else if (nFileType == XMACH_DEF::S_MH_DYLIB_STUB) nResult = TYPE_DYLIB_STUB;
    else if (nFileType == XMACH_DEF::S_MH_DSYM) nResult = TYPE_DSYM;
    else if (nFileType == XMACH_DEF::S_MH_KEXT_BUNDLE) nResult = TYPE_KEXT_BUNDLE;

    return nResult;
}

XBinary::OSINFO XMACH::getOsInfo()
{
    OSINFO result = {};

    result.osName = OSNAME_MAC_OS;

    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    quint32 nCPUType = getHeader_cputype();
    quint32 nCpuSubType = getHeader_cpusubtype();

    if (nCPUType == XMACH_DEF::S_CPU_TYPE_MC680x0) {
        result.osName = OSNAME_MAC_OS;
        result.sOsVersion = "1.0-8.1";
    } else if (nCPUType == XMACH_DEF::S_CPU_TYPE_POWERPC) {
        result.osName = OSNAME_MAC_OS;
        result.sOsVersion = "7.1.2-9.22";
    } else if (nCPUType == XMACH_DEF::S_CPU_TYPE_POWERPC64) {
        result.osName = OSNAME_MAC_OS_X;
        result.sOsVersion = "10.4-10.6";
    } else if (nCPUType == XMACH_DEF::S_CPU_TYPE_I386) {
        result.osName = OSNAME_MAC_OS_X;
        result.sOsVersion = "10.4-10.14";
    } else if (nCPUType == XMACH_DEF::S_CPU_TYPE_X86_64) {
        result.osName = OSNAME_MAC_OS_X;
        result.sOsVersion = "10.4-10.14";
    } else if ((nCPUType == XMACH_DEF::S_CPU_TYPE_ARM) || (nCPUType == XMACH_DEF::S_CPU_TYPE_ARM64)) {
        result.osName = OSNAME_IOS;

        if (nCpuSubType == XMACH_DEF::S_CPU_SUBTYPE_ARM_V6) {
            result.osName = OSNAME_IPHONEOS;
            result.sOsVersion = "1.0-4.2.1";  // TODO Check
        } else if (nCpuSubType == XMACH_DEF::S_CPU_SUBTYPE_ARM_V7) {
            result.osName = OSNAME_IPHONEOS;
            result.sOsVersion = "3.0-10.3.4";  // TODO Check
        } else if (nCPUType == XMACH_DEF::S_CPU_TYPE_ARM64) {
            result.osName = OSNAME_IOS;
            result.sOsVersion = "7.0-15.0";  // TODO Check
        }
    }

    qint64 nVersionMinOffset = -1;
    qint64 nBuildVersionOffset = -1;

    QList<XMACH::COMMAND_RECORD> listCommandRecords = getCommandRecords();

    if (isCommandPresent(XMACH_DEF::S_LC_BUILD_VERSION, &listCommandRecords)) {
        nBuildVersionOffset = getCommandRecordOffset(XMACH_DEF::S_LC_BUILD_VERSION, 0, &listCommandRecords);
    } else if (isCommandPresent(XMACH_DEF::S_LC_VERSION_MIN_IPHONEOS, &listCommandRecords)) {
        nVersionMinOffset = getCommandRecordOffset(XMACH_DEF::S_LC_VERSION_MIN_IPHONEOS, 0, &listCommandRecords);
        result.osName = OSNAME_IOS;
    } else if (isCommandPresent(XMACH_DEF::S_LC_VERSION_MIN_MACOSX, &listCommandRecords)) {
        nVersionMinOffset = getCommandRecordOffset(XMACH_DEF::S_LC_VERSION_MIN_MACOSX, 0, &listCommandRecords);
        result.osName = OSNAME_MACOS;
    } else if (isCommandPresent(XMACH_DEF::S_LC_VERSION_MIN_TVOS, &listCommandRecords)) {
        nVersionMinOffset = getCommandRecordOffset(XMACH_DEF::S_LC_VERSION_MIN_TVOS, 0, &listCommandRecords);
        result.osName = OSNAME_TVOS;
    } else if (isCommandPresent(XMACH_DEF::S_LC_VERSION_MIN_WATCHOS, &listCommandRecords)) {
        nVersionMinOffset = getCommandRecordOffset(XMACH_DEF::S_LC_VERSION_MIN_WATCHOS, 0, &listCommandRecords);
        result.osName = OSNAME_WATCHOS;
    }

    if (nBuildVersionOffset != -1) {
        XMACH_DEF::build_version_command build_version = _read_build_version_command(nBuildVersionOffset);

        if (build_version.platform == XMACH_DEF::S_PLATFORM_MACOS) result.osName = OSNAME_MACOS;
        else if (build_version.platform == XMACH_DEF::S_PLATFORM_BRIDGEOS) result.osName = OSNAME_BRIDGEOS;
        else if (build_version.platform == XMACH_DEF::S_PLATFORM_IOS) result.osName = OSNAME_IOS;  // TODO iPadOS
        else if (build_version.platform == XMACH_DEF::S_PLATFORM_TVOS) result.osName = OSNAME_TVOS;
        else if (build_version.platform == XMACH_DEF::S_PLATFORM_WATCHOS) result.osName = OSNAME_WATCHOS;

        if (build_version.minos) {
            result.sOsVersion = XBinary::get_uint32_full_version(build_version.minos);
        }
    } else if (nVersionMinOffset != -1) {
        XMACH_DEF::version_min_command version_min = _read_version_min_command(nVersionMinOffset);

        result.sOsVersion = XBinary::get_uint32_full_version(version_min.version);
    } else {
        QList<XMACH::LIBRARY_RECORD> listLibraryRecords = getLibraryRecords(XMACH_DEF::S_LC_LOAD_DYLIB);

        if (XMACH::isLibraryRecordNamePresent("Foundation", &listLibraryRecords)) {
            quint32 nVersion = XMACH::getLibraryCurrentVersion("Foundation", &listLibraryRecords);

            if ((result.osName == OSNAME_MAC_OS_X) || (result.osName == OSNAME_OS_X) || (result.osName == OSNAME_MACOS)) {
                if ((nVersion >= S_FULL_VERSION(397, 40, 0)) && (nVersion < S_FULL_VERSION(425, 0, 0))) result.sOsVersion = "10.0.0";
                else if (nVersion < S_FULL_VERSION(567, 0, 0)) result.sOsVersion = "10.3.0";
                else if (nVersion < S_FULL_VERSION(677, 0, 0)) result.sOsVersion = "10.4.0";
                else if (nVersion < S_FULL_VERSION(677, 24, 0)) result.sOsVersion = "10.5.0";
                else if (nVersion < S_FULL_VERSION(751, 0, 0)) result.sOsVersion = "10.5.7";
                else if (nVersion < S_FULL_VERSION(833, 10, 0)) result.sOsVersion = "10.6.0";
                else if (nVersion < S_FULL_VERSION(833, 25, 0)) result.sOsVersion = "10.7.0";
                else if (nVersion < S_FULL_VERSION(945, 18, 0)) result.sOsVersion = "10.7.4";
                else if (nVersion < S_FULL_VERSION(1151, 16, 0)) result.sOsVersion = "10.8.4";
                else if (nVersion < S_FULL_VERSION(1200, 0, 0)) result.sOsVersion = "10.10.0";  // TODO Check

                if (nVersion < S_FULL_VERSION(833, 10, 0)) {
                    result.osName = OSNAME_MAC_OS_X;
                }
                // TODO !
            } else if ((result.osName == OSNAME_IPHONEOS) || (result.osName == OSNAME_IOS) || (result.osName == OSNAME_IPADOS)) {
                if (nVersion < S_FULL_VERSION(678, 24, 0)) result.sOsVersion = "1.0.0";
                else if (nVersion < S_FULL_VERSION(678, 26, 0)) result.sOsVersion = "2.0.0";
                else if (nVersion < S_FULL_VERSION(678, 29, 0)) result.sOsVersion = "2.1.0";
                else if (nVersion < S_FULL_VERSION(678, 47, 0)) result.sOsVersion = "2.2.0";
                else if (nVersion < S_FULL_VERSION(678, 51, 0)) result.sOsVersion = "3.0.0";
                else if (nVersion < S_FULL_VERSION(678, 60, 0)) result.sOsVersion = "3.1.0";
                else if (nVersion < S_FULL_VERSION(751, 32, 0)) result.sOsVersion = "3.2.0";
                else if (nVersion < S_FULL_VERSION(751, 37, 0)) result.sOsVersion = "4.0.0";
                else if (nVersion < S_FULL_VERSION(751, 49, 0)) result.sOsVersion = "4.1.0";
                else if (nVersion < S_FULL_VERSION(881, 0, 0)) result.sOsVersion = "4.2.0";
                else if (nVersion < S_FULL_VERSION(890, 10, 0)) result.sOsVersion = "5.0.0";
                else if (nVersion < S_FULL_VERSION(992, 0, 0)) result.sOsVersion = "5.1.0";
                else if (nVersion < S_FULL_VERSION(993, 0, 0)) result.sOsVersion = "6.0.0";
                else if (nVersion < S_FULL_VERSION(1047, 20, 0)) result.sOsVersion = "6.1.0";
                else if (nVersion < S_FULL_VERSION(1047, 25, 0)) result.sOsVersion = "7.0.0";
                else if (nVersion < S_FULL_VERSION(1140, 11, 0)) result.sOsVersion = "7.1.0";
                else if (nVersion < S_FULL_VERSION(1141, 1, 0)) result.sOsVersion = "8.0.0";
                else if (nVersion < S_FULL_VERSION(1142, 14, 0)) result.sOsVersion = "8.1.0";
                else if (nVersion < S_FULL_VERSION(1144, 17, 0)) result.sOsVersion = "8.2.0";
                else if (nVersion < S_FULL_VERSION(1200, 0, 0)) result.sOsVersion = "8.3.0";  // TODO Check

                if (nVersion < S_FULL_VERSION(751, 32, 0)) {
                    result.osName = OSNAME_IPHONEOS;
                } else {
                    result.osName = OSNAME_IOS;
                }
            }
        }
    }

    return result;
}

QString XMACH::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_OBJECT: sResult = QString("OBJECT"); break;
        case TYPE_EXECUTE: sResult = QString("EXECUTE"); break;
        case TYPE_FVMLIB: sResult = QString("FVMLIB"); break;
        case TYPE_CORE: sResult = QString("CORE"); break;
        case TYPE_PRELOAD: sResult = QString("PRELOAD"); break;
        case TYPE_DYLIB: sResult = QString("DYLIB"); break;
        case TYPE_DYLINKER: sResult = QString("DYLINKER"); break;
        case TYPE_BUNDLE: sResult = QString("BUNDLE"); break;
        case TYPE_DYLIB_STUB: sResult = QString("DYLIB_STUB"); break;
        case TYPE_DSYM: sResult = QString("DSYM"); break;
        case TYPE_KEXT_BUNDLE: sResult = QString("KEXT_BUNDLE"); break;
    }

    return sResult;
}

QString XMACH::getFileFormatString()
{
    QString sResult;

    sResult = QString("MACH-O(%1)").arg(getArch());

    return sResult;
}

QString XMACH::getFileFormatExt()
{
    QString sResult;

    TYPE _type = (TYPE)getType();

    if (_type == TYPE_DYLIB) {
        sResult = "dylib";
    } else if (_type == TYPE_OBJECT) {
        sResult = "o";
    } else if (_type == TYPE_BUNDLE) {
        sResult = "bundle";
    } else {
        sResult = "macho";
    }

    return sResult;
}

qint64 XMACH::getFileFormatSize()
{
    return _calculateRawSize();
}

XADDR XMACH::readOpcodes(quint32 nType, char *pData, XADDR nAddress, qint64 nSize, QList<XBinary::OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus)
{
    XADDR nResult = 0;

    if (nType == OPCODE_TYPE_REBASE) {
        nResult = readOpcodesInterface_rebase(pData, nAddress, nSize, pListOpcodes, pOpcodeStatus);
    } else if (nType == OPCODE_TYPE_BIND) {
        nResult = readOpcodesInterface_bind(pData, nAddress, nSize, pListOpcodes, pOpcodeStatus, true);
    } else if (nType == OPCODE_TYPE_LAZY_BIND) {
        nResult = readOpcodesInterface_bind(pData, nAddress, nSize, pListOpcodes, pOpcodeStatus, false);
    } else if (nType == OPCODE_TYPE_WEAK_BIND) {
        nResult = readOpcodesInterface_bind(pData, nAddress, nSize, pListOpcodes, pOpcodeStatus, true);
    } else if (nType == OPCODE_TYPE_EXPORT) {
        nResult = readOpcodesInterface_export(pData, nAddress, nSize, pListOpcodes, pOpcodeStatus);
    }

    return nResult;
}

XADDR XMACH::readOpcodesInterface_rebase(char *pData, XADDR nAddress, qint64 nSize, QList<XBinary::OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus)
{
    XADDR nResult = 0;

    if (nSize > 0) {
        OPCODE opcodeMain = {};
        OPCODE opcodeUleb1 = {};
        OPCODE opcodeUleb2 = {};

        quint8 nByte = _read_uint8(pData);

        bool bSuccess = true;
        bool bUleb1 = false;
        bool bUleb2 = false;
        bool bImm = false;

        switch (nByte & XMACH_DEF::S_REBASE_OPCODE_MASK) {
            case XMACH_DEF::S_REBASE_OPCODE_SET_TYPE_IMM:
                opcodeMain.sName = QString("REBASE_OPCODE_SET_TYPE_IMM");
                bImm = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
                opcodeMain.sName = QString("REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB");
                bImm = true;
                bUleb1 = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_ADD_ADDR_ULEB:
                opcodeMain.sName = QString("REBASE_OPCODE_ADD_ADDR_ULEB");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_ADD_ADDR_IMM_SCALED:
                opcodeMain.sName = QString("REBASE_OPCODE_ADD_ADDR_IMM_SCALED");
                bImm = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_DO_REBASE_IMM_TIMES:
                opcodeMain.sName = QString("REBASE_OPCODE_DO_REBASE_IMM_TIMES");
                bImm = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_DO_REBASE_ULEB_TIMES:
                opcodeMain.sName = QString("REBASE_OPCODE_DO_REBASE_ULEB_TIMES");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB:
                opcodeMain.sName = QString("REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB:
                opcodeMain.sName = QString("REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB");
                bUleb1 = true;
                bUleb2 = true;
                break;
            default: *pOpcodeStatus = OPCODE_STATUS_END;
        }

        if (nByte == XMACH_DEF::S_REBASE_OPCODE_DONE) {
            opcodeMain.sName = QString("REBASE_OPCODE_DONE");
        }

        opcodeMain.nAddress = nAddress;
        opcodeMain.nSize = 1;

        nSize--;
        pData++;
        nResult++;
        nAddress++;

        if (bImm) {
            opcodeMain.sName += QString("(%1)").arg(nByte & XMACH_DEF::S_REBASE_IMMEDIATE_MASK);
        }

        if (bUleb1 && bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeUleb1, &pData, &nSize, &nAddress, &nResult, "ULEB128");
        }

        if (bUleb2 && bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeUleb2, &pData, &nSize, &nAddress, &nResult, "ULEB128");
        }

        if (bSuccess) {
            pListOpcodes->append(opcodeMain);

            if (bUleb1) pListOpcodes->append(opcodeUleb1);
            if (bUleb2) pListOpcodes->append(opcodeUleb2);
        } else {
            nResult = 0;
        }
    }

    return nResult;
}

XADDR XMACH::readOpcodesInterface_bind(char *pData, XADDR nAddress, qint64 nSize, QList<XBinary::OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus, bool bNullEnd)
{
    XADDR nResult = 0;

    if (nSize > 0) {
        OPCODE opcodeMain = {};
        OPCODE opcodeUleb1 = {};
        OPCODE opcodeUleb2 = {};
        OPCODE opcodeString = {};

        quint8 nByte = _read_uint8(pData);

        bool bSuccess = true;
        bool bUleb1 = false;
        bool bUleb2 = false;
        bool bString = false;
        bool bImm = false;

        switch (nByte & XMACH_DEF::S_BIND_OPCODE_MASK) {
            case XMACH_DEF::S_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
                opcodeMain.sName = QString("BIND_OPCODE_SET_DYLIB_ORDINAL_IMM");
                bImm = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB:
                opcodeMain.sName = QString("BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
                opcodeMain.sName = QString("BIND_OPCODE_SET_DYLIB_SPECIAL_IMM");
                bImm = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM:
                opcodeMain.sName = QString("BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM");
                bImm = true;
                bString = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_SET_TYPE_IMM:
                opcodeMain.sName = QString("BIND_OPCODE_SET_TYPE_IMM");
                bImm = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_SET_ADDEND_SLEB:
                opcodeMain.sName = QString("BIND_OPCODE_SET_ADDEND_SLEB");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB:
                opcodeMain.sName = QString("BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB");
                bImm = true;
                bUleb1 = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_ADD_ADDR_ULEB:
                opcodeMain.sName = QString("BIND_OPCODE_ADD_ADDR_ULEB");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_DO_BIND: opcodeMain.sName = QString("BIND_OPCODE_DO_BIND"); break;
            case XMACH_DEF::S_BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
                opcodeMain.sName = QString("BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB");
                bUleb1 = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED:
                opcodeMain.sName = QString("BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED");
                bImm = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB:
                opcodeMain.sName = QString("BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB");
                bUleb1 = true;
                bUleb2 = true;
                break;
            case XMACH_DEF::S_BIND_OPCODE_THREADED:
                opcodeMain.sName = QString("BIND_OPCODE_THREADED");
                bImm = true;
                break;
            default: *pOpcodeStatus = OPCODE_STATUS_END;
        }

        if (nByte == XMACH_DEF::S_BIND_OPCODE_DONE) {
            opcodeMain.sName = QString("BIND_OPCODE_DONE");

            if (bNullEnd) {
                *pOpcodeStatus = OPCODE_STATUS_END;
            } else {
                *pOpcodeStatus = OPCODE_STATUS_SUCCESS;
            }
        }

        opcodeMain.nAddress = nAddress;
        opcodeMain.nSize = 1;

        nSize--;
        pData++;
        nResult++;
        nAddress++;

        if (bImm) {
            opcodeMain.sName += QString("(%1)").arg(nByte & XMACH_DEF::S_REBASE_IMMEDIATE_MASK);
        }

        if (bString && bSuccess) {
            bSuccess = _read_opcode_ansiString(&opcodeString, &pData, &nSize, &nAddress, &nResult, "String");
        }

        if (bUleb1 && bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeUleb1, &pData, &nSize, &nAddress, &nResult, "ULEB128");
        }

        if (bUleb2 && bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeUleb2, &pData, &nSize, &nAddress, &nResult, "ULEB128");
        }

        if (bSuccess) {
            pListOpcodes->append(opcodeMain);

            if (bString) pListOpcodes->append(opcodeString);
            if (bUleb1) pListOpcodes->append(opcodeUleb1);
            if (bUleb2) pListOpcodes->append(opcodeUleb2);
        } else {
            nResult = 0;
        }
    }

    return nResult;
}

XADDR XMACH::readOpcodesInterface_export(char *pData, XADDR nAddress, qint64 nSize, QList<XBinary::OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus)
{
    Q_UNUSED(pOpcodeStatus)

    XADDR nResult = 0;

    if (nSize > 0) {
        bool bSuccess = false;

        OPCODE opcodeTerminalSize = {};
        OPCODE opcodeFlags = {};
        OPCODE opcodeSymbolOffset = {};
        OPCODE opcodeChildCount = {};
        QList<OPCODE> listChildren;

        bool bFlags = false;
        bool bSymbolOffset = false;

        PACKED_INT uTerminalSize = _read_uleb128(pData, nSize);

        if ((qint64)uTerminalSize.nValue < nSize) {
            bSuccess = _read_opcode_uleb128(&opcodeTerminalSize, &pData, &nSize, &nAddress, &nResult, "Terminal size");

            if (uTerminalSize.nValue > 0) {
                bFlags = true;
                bSymbolOffset = true;
            }
        }

        if (bFlags && bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeFlags, &pData, &nSize, &nAddress, &nResult, "Flags");
        }

        if (bSymbolOffset && bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeSymbolOffset, &pData, &nSize, &nAddress, &nResult, "Symbol offset");
        }

        PACKED_INT uChildCount = _read_uleb128(pData, nSize);

        if (bSuccess) {
            bSuccess = _read_opcode_uleb128(&opcodeChildCount, &pData, &nSize, &nAddress, &nResult, "Child count");
        }

        for (quint64 i = 0; i < uChildCount.nValue; i++) {
            OPCODE opcodeLabel = {};
            OPCODE opcodeOffset = {};

            if (bSuccess) bSuccess = _read_opcode_ansiString(&opcodeLabel, &pData, &nSize, &nAddress, &nResult, "Node label");
            if (bSuccess) bSuccess = _read_opcode_uleb128(&opcodeOffset, &pData, &nSize, &nAddress, &nResult, "Node offset");

            if (bSuccess) {
                listChildren.append(opcodeLabel);
                listChildren.append(opcodeOffset);
            } else {
                break;
            }
        }

        if ((uTerminalSize.nValue == 0) && (uChildCount.nValue == 0)) {
            bSuccess = false;
        }

        if (bSuccess) {
            pListOpcodes->append(opcodeTerminalSize);
            if (bFlags) pListOpcodes->append(opcodeFlags);
            if (bSymbolOffset) pListOpcodes->append(opcodeSymbolOffset);
            pListOpcodes->append(opcodeChildCount);
            pListOpcodes->append(listChildren);
        } else {
            nResult = 0;
        }
    }

    return nResult;
}
