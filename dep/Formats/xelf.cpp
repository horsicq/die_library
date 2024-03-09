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
#include "xelf.h"

XELF::XELF(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XBinary(pDevice, bIsImage, nModuleAddress)
{
}

XELF::~XELF()
{
}

bool XELF::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getIdent_Magic() == XELF_DEF::S_ELFMAG) {
        quint8 nClass = getIdent_class();

        if ((nClass == XELF_DEF::S_ELFCLASS32) || (nClass == XELF_DEF::S_ELFCLASS64)) {
            bResult = true;
        }
    }

    return bResult;
}

bool XELF::isValid(QIODevice *pDevice, bool bIsImage, quint64 nModuleAddress)
{
    XELF xelf(pDevice, bIsImage, nModuleAddress);

    return xelf.isValid();
}

XBinary::MODE XELF::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XELF xelf(pDevice, bIsImage, nModuleAddress);

    return xelf.getMode();
}

XBinary::ENDIAN XELF::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint8 nData = getIdent_data();

    if (nData == XELF_DEF::S_ELFDATA2LSB) {
        result = ENDIAN_LITTLE;
    } else if (nData == XELF_DEF::S_ELFDATA2MSB) {
        result = ENDIAN_BIG;
    }

    return result;
}

qint64 XELF::getEhdrOffset()
{
    return 0;
}

qint64 XELF::getEhdr32Size()
{
    return sizeof(XELF_DEF::Elf32_Ehdr);
}

qint64 XELF::getEhdr64Size()
{
    return sizeof(XELF_DEF::Elf64_Ehdr);
}

quint32 XELF::getIdent_Magic()
{
    return read_uint32(XELF_DEF::S_EI_MAG0);
}

void XELF::setIdent_Magic(quint32 nValue)
{
    write_uint32(XELF_DEF::S_EI_MAG0, nValue);
}

quint8 XELF::getIdent_mag(qint32 nMag)
{
    quint8 nValue = 0;

    if (nMag < 4) {
        nValue = read_uint8(nMag);
    }

    return nValue;
}

void XELF::setIdent_mag(quint8 nValue, qint32 nMag)
{
    if (nMag < 4) {
        write_uint8(nMag, nValue);
    }
}

quint32 XELF::getIdent_mag_LE()
{
    return read_uint32(0, false);
}

void XELF::setIdent_mag_LE(quint32 nValue)
{
    write_uint32(0, nValue, false);
}

quint8 XELF::getIdent_class()
{
    return read_uint8(XELF_DEF::S_EI_CLASS);
}

void XELF::setIdent_class(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_CLASS, nValue);
}

quint8 XELF::getIdent_data()
{
    return read_uint8(XELF_DEF::S_EI_DATA);
}

void XELF::setIdent_data(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_DATA, nValue);
}

quint8 XELF::getIdent_version()
{
    return read_uint8(XELF_DEF::S_EI_VERSION);
}

void XELF::setIdent_version(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_VERSION, nValue);
}

quint8 XELF::getIdent_osabi()
{
    return read_uint8(XELF_DEF::S_EI_OSABI);
}

void XELF::setIdent_osabi(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_OSABI, nValue);
}

quint8 XELF::getIdent_abiversion()
{
    return read_uint8(XELF_DEF::S_EI_ABIVERSION);
}

void XELF::setIdent_abiversion(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_ABIVERSION, nValue);
}

quint8 XELF::getIdent_pad(qint32 nPad)
{
    quint8 nValue = 0;

    if (nPad < 7) {
        nValue = read_uint8(XELF_DEF::S_EI_ABIVERSION + 1 + nPad);
    }

    return nValue;
}

void XELF::setIdent_pad(quint8 nValue, qint32 nPad)
{
    if (nPad < 7) {
        write_uint8(XELF_DEF::S_EI_ABIVERSION + 1 + nPad, nValue);
    }
}

quint16 XELF::getHdr32_type()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_type), isBigEndian());
}

void XELF::setHdr32_type(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_type), nValue, isBigEndian());
}

quint16 XELF::getHdr32_machine()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_machine), isBigEndian());
}

void XELF::setHdr32_machine(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_machine), nValue, isBigEndian());
}

quint32 XELF::getHdr32_version()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_version), isBigEndian());
}

void XELF::setHdr32_version(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_version), nValue, isBigEndian());
}

quint32 XELF::getHdr32_entry()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_entry), isBigEndian());
}

void XELF::setHdr32_entry(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_entry), nValue, isBigEndian());
}

quint32 XELF::getHdr32_phoff()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_phoff), isBigEndian());
}

void XELF::setHdr32_phoff(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_phoff), nValue, isBigEndian());
}

quint32 XELF::getHdr32_shoff()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_shoff), isBigEndian());
}

void XELF::setHdr32_shoff(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_shoff), nValue, isBigEndian());
}

quint32 XELF::getHdr32_flags()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_flags), isBigEndian());
}

void XELF::setHdr32_flags(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_flags), nValue, isBigEndian());
}

quint16 XELF::getHdr32_ehsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_ehsize), isBigEndian());
}

void XELF::setHdr32_ehsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_ehsize), nValue, isBigEndian());
}

quint16 XELF::getHdr32_phentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phentsize), isBigEndian());
}

void XELF::setHdr32_phentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr32_phnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phnum), isBigEndian());
}

void XELF::setHdr32_phnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phnum), nValue, isBigEndian());
}

quint16 XELF::getHdr32_shentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shentsize), isBigEndian());
}

void XELF::setHdr32_shentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr32_shnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shnum), isBigEndian());
}

void XELF::setHdr32_shnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shnum), nValue, isBigEndian());
}

quint16 XELF::getHdr32_shstrndx()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shstrndx), isBigEndian());
}

void XELF::setHdr32_shstrndx(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shstrndx), nValue, isBigEndian());
}

quint16 XELF::getHdr64_type()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_type), isBigEndian());
}

void XELF::setHdr64_type(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_type), nValue, isBigEndian());
}

quint16 XELF::getHdr64_machine()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_machine), isBigEndian());
}

void XELF::setHdr64_machine(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_machine), nValue, isBigEndian());
}

quint32 XELF::getHdr64_version()
{
    return read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_version), isBigEndian());
}

void XELF::setHdr64_version(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_version), nValue, isBigEndian());
}

quint64 XELF::getHdr64_entry()
{
    return read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_entry), isBigEndian());
}

void XELF::setHdr64_entry(quint64 nValue)
{
    write_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_entry), nValue, isBigEndian());
}

quint64 XELF::getHdr64_phoff()
{
    return read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_phoff), isBigEndian());
}

void XELF::setHdr64_phoff(quint64 nValue)
{
    write_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_phoff), nValue, isBigEndian());
}

quint64 XELF::getHdr64_shoff()
{
    return read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_shoff), isBigEndian());
}

void XELF::setHdr64_shoff(quint64 nValue)
{
    write_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_shoff), nValue, isBigEndian());
}

quint32 XELF::getHdr64_flags()
{
    return read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_flags), isBigEndian());
}

void XELF::setHdr64_flags(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_flags), nValue, isBigEndian());
}

quint16 XELF::getHdr64_ehsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_ehsize), isBigEndian());
}

void XELF::setHdr64_ehsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_ehsize), nValue, isBigEndian());
}

quint16 XELF::getHdr64_phentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phentsize), isBigEndian());
}

void XELF::setHdr64_phentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr64_phnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phnum), isBigEndian());
}

void XELF::setHdr64_phnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phnum), nValue, isBigEndian());
}

quint16 XELF::getHdr64_shentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shentsize), isBigEndian());
}

void XELF::setHdr64_shentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr64_shnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shnum), isBigEndian());
}

void XELF::setHdr64_shnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shnum), nValue, isBigEndian());
}

quint16 XELF::getHdr64_shstrndx()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shstrndx), isBigEndian());
}

void XELF::setHdr64_shstrndx(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shstrndx), nValue, isBigEndian());
}

XELF_DEF::Elf_Ehdr XELF::getHdr()
{
    XELF_DEF::Elf_Ehdr result = {};

    bool bIsbigEndian = isBigEndian();

    if (!is64()) {
        result.e_type = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_type), bIsbigEndian);
        result.e_machine = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_machine), bIsbigEndian);
        result.e_version = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_version), bIsbigEndian);
        result.e_entry = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_entry), bIsbigEndian);
        result.e_phoff = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_phoff), bIsbigEndian);
        result.e_shoff = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_shoff), bIsbigEndian);
        result.e_flags = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_flags), bIsbigEndian);
        result.e_ehsize = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_ehsize), bIsbigEndian);
        result.e_phentsize = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phentsize), bIsbigEndian);
        result.e_phnum = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phnum), bIsbigEndian);
        result.e_shentsize = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shentsize), bIsbigEndian);
        result.e_shnum = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shnum), bIsbigEndian);
        result.e_shstrndx = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shstrndx), bIsbigEndian);
    } else {
        result.e_type = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_type), bIsbigEndian);
        result.e_machine = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_machine), bIsbigEndian);
        result.e_version = read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_version), bIsbigEndian);
        result.e_entry = read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_entry), bIsbigEndian);
        result.e_phoff = read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_phoff), bIsbigEndian);
        result.e_shoff = read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_shoff), bIsbigEndian);
        result.e_flags = read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_flags), bIsbigEndian);
        result.e_ehsize = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_ehsize), bIsbigEndian);
        result.e_phentsize = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phentsize), bIsbigEndian);
        result.e_phnum = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phnum), bIsbigEndian);
        result.e_shentsize = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shentsize), bIsbigEndian);
        result.e_shnum = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shnum), bIsbigEndian);
        result.e_shstrndx = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shstrndx), bIsbigEndian);
    }

    return result;
}

QMap<quint64, QString> XELF::getTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "ET_NONE");
    mapResult.insert(1, "ET_REL");
    mapResult.insert(2, "ET_EXEC");
    mapResult.insert(3, "ET_DYN");
    mapResult.insert(4, "ET_CORE");
    mapResult.insert(5, "ET_NUM");
    mapResult.insert(0xff00, "ET_LOPROC");
    mapResult.insert(0xffff, "ET_HIPROC");

    return mapResult;
}

QMap<quint64, QString> XELF::getTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NONE");
    mapResult.insert(1, "REL");
    mapResult.insert(2, "EXEC");
    mapResult.insert(3, "DYN");
    mapResult.insert(4, "CORE");
    mapResult.insert(5, "NUM");
    mapResult.insert(0xff00, "LOPROC");
    mapResult.insert(0xffff, "HIPROC");

    return mapResult;
}

QMap<quint64, QString> XELF::getMachines()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0,
                     "EM_NONE");  // TODO
                                  // http://users.sosdg.org/~qiyong/mxr/source/sys/sys/exec_elf.h
                                  // https://github.com/Mm7/oatrev/blob/master/elfio/elf_types.hpp
    mapResult.insert(1, "EM_M32");
    mapResult.insert(2, "EM_SPARC");
    mapResult.insert(3, "EM_386");
    mapResult.insert(4, "EM_68K");
    mapResult.insert(5, "EM_88K");
    mapResult.insert(6, "EM_486"); /* Perhaps disused */
    mapResult.insert(7, "EM_860");
    mapResult.insert(8, "EM_MIPS");
    mapResult.insert(9, "EM_S370");
    mapResult.insert(10, "EM_MIPS_RS3_LE");
    mapResult.insert(11, "EM_RS6000");
    mapResult.insert(15, "EM_PARISC");
    mapResult.insert(16, "EM_nCUBE");
    mapResult.insert(17, "EM_VPP500");
    mapResult.insert(18, "EM_SPARC32PLUS");
    mapResult.insert(19, "EM_960");
    mapResult.insert(20, "EM_PPC");
    mapResult.insert(21, "EM_PPC64");
    mapResult.insert(22, "EM_S390");
    mapResult.insert(23, "EM_SPU");
    mapResult.insert(36, "EM_V800");
    mapResult.insert(37, "EM_FR20");
    mapResult.insert(38, "EM_RH32");
    mapResult.insert(39, "EM_RCE");
    mapResult.insert(40, "EM_ARM");
    mapResult.insert(41, "EM_ALPHA");
    mapResult.insert(42, "EM_SH");
    mapResult.insert(43, "EM_SPARCV9");
    mapResult.insert(44, "EM_TRICORE");
    mapResult.insert(45, "EM_ARC");
    mapResult.insert(46, "EM_H8_300");
    mapResult.insert(47, "EM_H8_300H");
    mapResult.insert(48, "EM_H8S");
    mapResult.insert(49, "EM_H8_500");
    mapResult.insert(50, "EM_IA_64");
    mapResult.insert(51, "EM_MIPS_X");
    mapResult.insert(52, "EM_COLDFIRE");
    mapResult.insert(53, "EM_68HC12");
    mapResult.insert(54, "EM_MMA");
    mapResult.insert(55, "EM_PCP");
    mapResult.insert(56, "EM_NCPU");
    mapResult.insert(57, "EM_NDR1");
    mapResult.insert(58, "EM_STARCORE");
    mapResult.insert(59, "EM_ME16");
    mapResult.insert(60, "EM_ST100");
    mapResult.insert(61, "EM_TINYJ");
    mapResult.insert(62, "EM_AMD64");  // EM_X86_64
    mapResult.insert(63, "EM_PDSP");
    mapResult.insert(66, "EM_FX66");
    mapResult.insert(67, "EM_ST9PLUS");
    mapResult.insert(68, "EM_ST7");
    mapResult.insert(69, "EM_68HC16");
    mapResult.insert(70, "EM_68HC11");
    mapResult.insert(71, "EM_68HC08");
    mapResult.insert(72, "EM_68HC05");
    mapResult.insert(73, "EM_SVX");
    mapResult.insert(74, "EM_ST19");
    mapResult.insert(75, "EM_VAX");
    mapResult.insert(76, "EM_CRIS");
    mapResult.insert(77, "EM_JAVELIN");
    mapResult.insert(78, "EM_FIREPATH");
    mapResult.insert(79, "EM_ZSP");
    mapResult.insert(80, "EM_MMIX");
    mapResult.insert(81, "EM_HUANY");
    mapResult.insert(82, "EM_PRISM");
    mapResult.insert(83, "EM_AVR");
    mapResult.insert(84, "EM_FR30");
    mapResult.insert(85, "EM_D10V");
    mapResult.insert(86, "EM_D30V");
    mapResult.insert(87, "EM_V850");
    mapResult.insert(88, "EM_M32R");
    mapResult.insert(89, "EM_MN10300");
    mapResult.insert(90, "EM_MN10200");
    mapResult.insert(91, "EM_PJ");
    mapResult.insert(92, "EM_OPENRISC");
    mapResult.insert(93, "EM_ARC_A5");
    mapResult.insert(94, "EM_XTENSA");
    mapResult.insert(95, "EM_VIDEOCORE");
    mapResult.insert(96, "EM_TMM_GPP");
    mapResult.insert(97, "EM_NS32K");
    mapResult.insert(98, "EM_TPC");
    mapResult.insert(99, "EM_SNP1K");
    mapResult.insert(100, "EM_ST200");
    mapResult.insert(101, "EM_IP2K");
    mapResult.insert(102, "EM_MAX");
    mapResult.insert(103, "EM_CR");
    mapResult.insert(104, "EM_F2MC16");
    mapResult.insert(105, "EM_MSP430");
    mapResult.insert(106, "EM_BLACKFIN");
    mapResult.insert(107, "EM_SE_C33");
    mapResult.insert(108, "EM_SEP");
    mapResult.insert(109, "EM_ARCA");
    mapResult.insert(110, "EM_UNICORE");
    mapResult.insert(111, "EM_EXCESS");
    mapResult.insert(112, "EM_DXP");
    mapResult.insert(113, "EM_ALTERA_NIOS2");
    mapResult.insert(114, "EM_CRX");
    mapResult.insert(115, "EM_XGATE");
    mapResult.insert(116, "EM_C166");
    mapResult.insert(117, "EM_M16C");
    mapResult.insert(118, "EM_DSPIC30F");
    mapResult.insert(119, "EM_CE");
    mapResult.insert(120, "EM_M32C");
    mapResult.insert(140, "EM_TI_C6000");
    mapResult.insert(183, "EM_AARCH64");
    mapResult.insert(243, "EM_RISC_V");
    mapResult.insert(0x5441, "EM_FRV");
    mapResult.insert(0x18ad, "EM_AVR32");
    mapResult.insert(0x9026, "EM_ALPHA");
    mapResult.insert(0x9080, "EM_CYGNUS_V850");
    mapResult.insert(0x9041, "EM_CYGNUS_M32R");
    mapResult.insert(0xA390, "EM_S390_OLD");
    mapResult.insert(0xbeef, "EM_CYGNUS_MN10300");

    return mapResult;
}

QMap<quint64, QString> XELF::getMachinesS()
{
    QMap<quint64, QString> mapResult;

    // TODO
    // ftp://ftp.eso.org/pub/solaris/rec_patches/8/8_Recommended/109147-39/SUNWhea/reloc/usr/include/sys/elf.h
    // TODO http://users.sosdg.org/~qiyong/mxr/source/sys/sys/elf.h
    mapResult.insert(0, "NONE");
    mapResult.insert(1, "M32");
    mapResult.insert(2, "SPARC");
    mapResult.insert(3, "386");
    mapResult.insert(4, "68K");
    mapResult.insert(5, "88K");
    mapResult.insert(6, "486"); /* Perhaps disused */
    mapResult.insert(7, "860");
    mapResult.insert(8, "MIPS");
    mapResult.insert(9, "S370");
    mapResult.insert(10, "MIPS_RS3_LE");
    mapResult.insert(11, "RS6000");
    mapResult.insert(15, "PARISC");
    mapResult.insert(16, "nCUBE");
    mapResult.insert(17, "VPP500");
    mapResult.insert(18, "SPARC32PLUS");
    mapResult.insert(19, "960");
    mapResult.insert(20, "PPC");
    mapResult.insert(21, "PPC64");
    mapResult.insert(22, "S390");
    mapResult.insert(23, "SPU");
    mapResult.insert(36, "V800");
    mapResult.insert(37, "FR20");
    mapResult.insert(38, "RH32");
    mapResult.insert(39, "RCE");
    mapResult.insert(40, "ARM");
    mapResult.insert(41, "ALPHA");
    mapResult.insert(42, "SH");
    mapResult.insert(43, "SPARCV9");
    mapResult.insert(44, "TRICORE");
    mapResult.insert(45, "ARC");
    mapResult.insert(46, "H8_300");
    mapResult.insert(47, "H8_300H");
    mapResult.insert(48, "H8S");
    mapResult.insert(49, "H8_500");
    mapResult.insert(50, "IA_64");
    mapResult.insert(51, "MIPS_X");
    mapResult.insert(52, "COLDFIRE");
    mapResult.insert(53, "68HC12");
    mapResult.insert(54, "MMA");
    mapResult.insert(55, "PCP");
    mapResult.insert(56, "NCPU");
    mapResult.insert(57, "NDR1");
    mapResult.insert(58, "STARCORE");
    mapResult.insert(59, "ME16");
    mapResult.insert(60, "ST100");
    mapResult.insert(61, "TINYJ");
    mapResult.insert(62, "AMD64");  // X86_64
    mapResult.insert(63, "PDSP");
    mapResult.insert(66, "FX66");
    mapResult.insert(67, "ST9PLUS");
    mapResult.insert(68, "ST7");
    mapResult.insert(69, "68HC16");
    mapResult.insert(70, "68HC11");
    mapResult.insert(71, "68HC08");
    mapResult.insert(72, "68HC05");
    mapResult.insert(73, "SVX");
    mapResult.insert(74, "ST19");
    mapResult.insert(75, "VAX");
    mapResult.insert(76, "CRIS");
    mapResult.insert(77, "JAVELIN");
    mapResult.insert(78, "FIREPATH");
    mapResult.insert(79, "ZSP");
    mapResult.insert(80, "MMIX");
    mapResult.insert(81, "HUANY");
    mapResult.insert(82, "PRISM");
    mapResult.insert(83, "AVR");
    mapResult.insert(84, "FR30");
    mapResult.insert(85, "D10V");
    mapResult.insert(86, "D30V");
    mapResult.insert(87, "V850");
    mapResult.insert(88, "M32R");
    mapResult.insert(89, "MN10300");
    mapResult.insert(89, "MN10300");
    mapResult.insert(90, "MN10200");
    mapResult.insert(91, "PJ");  // TODO
    mapResult.insert(92, "OPENRISC");
    mapResult.insert(93, "ARC_A5");
    mapResult.insert(94, "XTENSA");
    mapResult.insert(95, "VIDEOCORE");
    mapResult.insert(96, "TMM_GPP");
    mapResult.insert(97, "NS32K");
    mapResult.insert(98, "TPC");
    mapResult.insert(99, "SNP1K");
    mapResult.insert(100, "ST200");
    mapResult.insert(101, "IP2K");
    mapResult.insert(102, "MAX");
    mapResult.insert(103, "CR");
    mapResult.insert(104, "F2MC16");
    mapResult.insert(105, "MSP430");
    mapResult.insert(106, "BLACKFIN");
    mapResult.insert(107, "SE_C33");
    mapResult.insert(108, "SEP");
    mapResult.insert(109, "ARCA");
    mapResult.insert(110, "UNICORE");
    mapResult.insert(111, "EXCESS");
    mapResult.insert(112, "DXP");
    mapResult.insert(113, "ALTERA_NIOS2");
    mapResult.insert(114, "CRX");
    mapResult.insert(115, "XGATE");
    mapResult.insert(116, "C166");
    mapResult.insert(117, "M16C");
    mapResult.insert(118, "DSPIC30F");
    mapResult.insert(119, "CE");
    mapResult.insert(120, "M32C");
    mapResult.insert(140, "TI_C6000");
    mapResult.insert(183, "AARCH64");
    mapResult.insert(243, "RISC_V");
    mapResult.insert(0x5441, "FRV");
    mapResult.insert(0x18ad, "AVR32");
    mapResult.insert(0x9026, "ALPHA");
    mapResult.insert(0x9080, "CYGNUS_V850");
    mapResult.insert(0x9041, "CYGNUS_M32R");
    mapResult.insert(0xA390, "S390_OLD");
    mapResult.insert(0xbeef, "CYGNUS_MN10300");

    return mapResult;
}

QMap<quint64, QString> XELF::getHeaderVersionList()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(1, "EV_CURRENT");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentMag()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x464C457F, "ELFMAG");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentMagS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x464C457F, "ELFMAG");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentClasses()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "ELFCLASSNONE");
    mapResult.insert(1, "ELFCLASS32");
    mapResult.insert(2, "ELFCLASS64");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentClassesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NONE");
    mapResult.insert(1, "32");
    mapResult.insert(2, "64");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentDatas()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "ELFDATANONE");
    mapResult.insert(1, "ELFDATA2LSB");
    mapResult.insert(2, "ELFDATA2MSB");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentDatasS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NONE");
    mapResult.insert(1, "2LSB");
    mapResult.insert(2, "2MSB");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentVersions()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(1, "EV_CURRENT");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentVersionsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(1, "CURRENT");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentOsabis()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "ELFOSABI_SYSV");
    mapResult.insert(1, "ELFOSABI_HPUX");
    mapResult.insert(2, "ELFOSABI_NETBSD");
    mapResult.insert(3, "ELFOSABI_LINUX");
    mapResult.insert(4, "ELFOSABI_HURD");
    mapResult.insert(5, "ELFOSABI_86OPEN");
    mapResult.insert(6, "ELFOSABI_SOLARIS");
    mapResult.insert(7, "ELFOSABI_AIX");
    mapResult.insert(8, "ELFOSABI_IRIX");
    mapResult.insert(9, "ELFOSABI_FREEBSD");
    mapResult.insert(10, "ELFOSABI_TRU64");
    mapResult.insert(11, "ELFOSABI_MODESTO");
    mapResult.insert(12, "ELFOSABI_OPENBSD");
    mapResult.insert(13, "ELFOSABI_OPENVMS");
    mapResult.insert(14, "ELFOSABI_NSK");
    mapResult.insert(15, "ELFOSABI_AROS");
    mapResult.insert(16, "ELFOSABI_FENIXOS");
    mapResult.insert(17, "ELFOSABI_CLOUDABI");
    mapResult.insert(18, "ELFOSABI_OPENVOS");
    mapResult.insert(64, "ELFOSABI_ARM_AEABI");
    mapResult.insert(97, "ELFOSABI_ARM");
    mapResult.insert(255, "ELFOSABI_STANDALONE");

    return mapResult;
}

QMap<quint64, QString> XELF::getIndentOsabisS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "SYSV");
    mapResult.insert(1, "HPUX");
    mapResult.insert(2, "NETBSD");
    mapResult.insert(3, "LINUX");
    mapResult.insert(4, "HURD");
    mapResult.insert(5, "86OPEN");
    mapResult.insert(6, "SOLARIS");
    mapResult.insert(7, "AIX");
    mapResult.insert(8, "IRIX");
    mapResult.insert(9, "FREEBSD");
    mapResult.insert(10, "TRU64");
    mapResult.insert(11, "MODESTO");
    mapResult.insert(12, "OPENBSD");
    mapResult.insert(13, "OPENVMS");
    mapResult.insert(14, "NSK");
    mapResult.insert(15, "AROS");
    mapResult.insert(16, "FENIXOS");
    mapResult.insert(17, "CLOUDABI");
    mapResult.insert(18, "OPENVOS");
    mapResult.insert(64, "ARM_AEABI");
    mapResult.insert(97, "ARM");
    mapResult.insert(255, "STANDALONE");

    return mapResult;
}

QMap<quint64, QString> XELF::getSectionTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "SHT_NULL");
    mapResult.insert(1, "SHT_PROGBITS");
    mapResult.insert(2, "SHT_SYMTAB");
    mapResult.insert(3, "SHT_STRTAB");
    mapResult.insert(4, "SHT_RELA");
    mapResult.insert(5, "SHT_HASH");
    mapResult.insert(6, "SHT_DYNAMIC");
    mapResult.insert(7, "SHT_NOTE");
    mapResult.insert(8, "SHT_NOBITS");
    mapResult.insert(9, "SHT_REL");
    mapResult.insert(10, "SHT_SHLIB");
    mapResult.insert(11, "SHT_DYNSYM");
    mapResult.insert(14, "SHT_INIT_ARRAY");
    mapResult.insert(15, "SHT_FINI_ARRAY");
    mapResult.insert(16, "SHT_PREINIT_ARRAY");
    mapResult.insert(17, "SHT_GROUP");
    mapResult.insert(18, "SHT_SYMTAB_SHNDX");
    mapResult.insert(19, "SHT_NUM");
    mapResult.insert(0x60000000, "SHT_LOOS");
    mapResult.insert(0x6ffffff5, "SHT_GNU_ATTRIBUTES");
    mapResult.insert(0x6ffffff6, "SHT_GNU_HASH");
    mapResult.insert(0x6ffffffa, "SHT_SUNW_move");
    mapResult.insert(0x6ffffffc, "SHT_SUNW_syminfo");
    mapResult.insert(0x6ffffffd, "SHT_GNU_verdef");
    mapResult.insert(0x6ffffffe, "SHT_GNU_verneed");
    mapResult.insert(0x6fffffff, "SHT_GNU_versym");
    mapResult.insert(0x70000000, "SHT_LOPROC");
    mapResult.insert(0x70000001, "SHT_AMD64_UNWIND");
    mapResult.insert(0x70000002, "SHT_ARM_PREEMPTMAP");
    mapResult.insert(0x70000003, "SHT_ARM_ATTRIBUTES");
    mapResult.insert(0x70000004, "SHT_ARM_DEBUGOVERLAY");
    mapResult.insert(0x70000005, "SHT_ARM_OVERLAYSECTION");
    mapResult.insert(0x70000006, "SHT_MIPS_REGINFO");
    mapResult.insert(0x70000007, "SHT_MIPS_PACKAGE");
    mapResult.insert(0x70000008, "SHT_MIPS_PACKSYM");
    mapResult.insert(0x70000009, "SHT_MIPS_RELD");
    mapResult.insert(0x7000000b, "SHT_MIPS_IFACE");
    mapResult.insert(0x7000000c, "SHT_MIPS_CONTENT");
    mapResult.insert(0x7000000d, "SHT_MIPS_OPTIONS");
    mapResult.insert(0x70000010, "SHT_MIPS_SHDR");
    mapResult.insert(0x70000011, "SHT_MIPS_FDESC");
    mapResult.insert(0x70000012, "SHT_MIPS_EXTSYM");
    mapResult.insert(0x70000013, "SHT_MIPS_DENSE");
    mapResult.insert(0x70000014, "SHT_MIPS_PDESC");
    mapResult.insert(0x70000015, "SHT_MIPS_LOCSYM");
    mapResult.insert(0x70000016, "SHT_MIPS_AUXSYM");
    mapResult.insert(0x70000017, "SHT_MIPS_OPTSYM");
    mapResult.insert(0x70000018, "SHT_MIPS_LOCSTR");
    mapResult.insert(0x70000019, "SHT_MIPS_LINE");
    mapResult.insert(0x7000001a, "SHT_MIPS_RFDESC");
    mapResult.insert(0x7000001b, "SHT_MIPS_DELTASYM");
    mapResult.insert(0x7000001c, "SHT_MIPS_DELTAINST");
    mapResult.insert(0x7000001d, "SHT_MIPS_DELTACLASS");
    mapResult.insert(0x7000001e, "SHT_MIPS_DWARF");
    mapResult.insert(0x7000001f, "SHT_MIPS_DELTADECL");
    mapResult.insert(0x70000020, "SHT_MIPS_SYMBOL_LIB");
    mapResult.insert(0x70000021, "SHT_MIPS_EVENTS");
    mapResult.insert(0x70000022, "SHT_MIPS_TRANSLATE");
    mapResult.insert(0x70000023, "SHT_MIPS_PIXIE");
    mapResult.insert(0x70000024, "SHT_MIPS_XLATE");
    mapResult.insert(0x70000025, "SHT_MIPS_XLATE_DEBUG");
    mapResult.insert(0x70000026, "SHT_MIPS_WHIRL");
    mapResult.insert(0x70000027, "SHT_MIPS_EH_REGION");
    mapResult.insert(0x70000028, "SHT_MIPS_XLATE_OLD");
    mapResult.insert(0x70000029, "SHT_MIPS_PDR_EXCEPTION");
    mapResult.insert(0x7000002a, "SHT_MIPS_ABIFLAGS");
    mapResult.insert(0x7fffffff, "SHT_HIPROC");
    mapResult.insert(0x80000000, "SHT_LOUSER");
    mapResult.insert(0xffffffff, "SHT_HIUSER");

    return mapResult;
}

QMap<quint64, QString> XELF::getSectionTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NULL");
    mapResult.insert(1, "PROGBITS");
    mapResult.insert(2, "SYMTAB");
    mapResult.insert(3, "STRTAB");
    mapResult.insert(4, "RELA");
    mapResult.insert(5, "HASH");
    mapResult.insert(6, "DYNAMIC");
    mapResult.insert(7, "NOTE");
    mapResult.insert(8, "NOBITS");
    mapResult.insert(9, "REL");
    mapResult.insert(10, "SHLIB");
    mapResult.insert(11, "DYNSYM");
    mapResult.insert(14, "INIT_ARRAY");
    mapResult.insert(15, "FINI_ARRAY");
    mapResult.insert(16, "PREINIT_ARRAY");
    mapResult.insert(17, "GROUP");
    mapResult.insert(18, "SYMTAB_SHNDX");
    mapResult.insert(19, "NUM");
    mapResult.insert(0x60000000, "LOOS");
    mapResult.insert(0x6ffffff5, "GNU_ATTRIBUTES");
    mapResult.insert(0x6ffffff6, "GNU_HASH");
    mapResult.insert(0x6ffffffa, "SUNW_move");
    mapResult.insert(0x6ffffffc, "SUNW_syminfo");
    mapResult.insert(0x6ffffffd, "GNU_verdef");
    mapResult.insert(0x6ffffffe, "GNU_verneed");
    mapResult.insert(0x6fffffff, "GNU_versym");
    mapResult.insert(0x70000000, "LOPROC");
    mapResult.insert(0x70000001, "AMD64_UNWIND");
    mapResult.insert(0x70000002, "ARM_PREEMPTMAP");
    mapResult.insert(0x70000003, "ARM_ATTRIBUTES");
    mapResult.insert(0x70000004, "ARM_DEBUGOVERLAY");
    mapResult.insert(0x70000005, "ARM_OVERLAYSECTION");
    mapResult.insert(0x70000006, "MIPS_REGINFO");
    mapResult.insert(0x70000007, "MIPS_PACKAGE");
    mapResult.insert(0x70000008, "MIPS_PACKSYM");
    mapResult.insert(0x70000009, "MIPS_RELD");
    mapResult.insert(0x7000000b, "MIPS_IFACE");
    mapResult.insert(0x7000000c, "MIPS_CONTENT");
    mapResult.insert(0x7000000d, "MIPS_OPTIONS");
    mapResult.insert(0x70000010, "MIPS_SHDR");
    mapResult.insert(0x70000011, "MIPS_FDESC");
    mapResult.insert(0x70000012, "MIPS_EXTSYM");
    mapResult.insert(0x70000013, "MIPS_DENSE");
    mapResult.insert(0x70000014, "MIPS_PDESC");
    mapResult.insert(0x70000015, "MIPS_LOCSYM");
    mapResult.insert(0x70000016, "MIPS_AUXSYM");
    mapResult.insert(0x70000017, "MIPS_OPTSYM");
    mapResult.insert(0x70000018, "MIPS_LOCSTR");
    mapResult.insert(0x70000019, "MIPS_LINE");
    mapResult.insert(0x7000001a, "MIPS_RFDESC");
    mapResult.insert(0x7000001b, "MIPS_DELTASYM");
    mapResult.insert(0x7000001c, "MIPS_DELTAINST");
    mapResult.insert(0x7000001d, "MIPS_DELTACLASS");
    mapResult.insert(0x7000001e, "MIPS_DWARF");
    mapResult.insert(0x7000001f, "MIPS_DELTADECL");
    mapResult.insert(0x70000020, "MIPS_SYMBOL_LIB");
    mapResult.insert(0x70000021, "MIPS_EVENTS");
    mapResult.insert(0x70000022, "MIPS_TRANSLATE");
    mapResult.insert(0x70000023, "MIPS_PIXIE");
    mapResult.insert(0x70000024, "MIPS_XLATE");
    mapResult.insert(0x70000025, "MIPS_XLATE_DEBUG");
    mapResult.insert(0x70000026, "MIPS_WHIRL");
    mapResult.insert(0x70000027, "MIPS_EH_REGION");
    mapResult.insert(0x70000028, "MIPS_XLATE_OLD");
    mapResult.insert(0x70000029, "MIPS_PDR_EXCEPTION");
    mapResult.insert(0x7000002a, "MIPS_ABIFLAGS");
    mapResult.insert(0x7fffffff, "HIPROC");
    mapResult.insert(0x80000000, "LOUSER");
    mapResult.insert(0xffffffff, "HIUSER");

    return mapResult;
}

QMap<quint64, QString> XELF::getSectionFlags()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000001, "SHF_WRITE");
    mapResult.insert(0x00000002, "SHF_ALLOC");
    mapResult.insert(0x00000004, "SHF_EXECINSTR");
    mapResult.insert(0x00000010, "SHF_MERGE");
    mapResult.insert(0x00000020, "SHF_STRINGS");
    mapResult.insert(0x00000040, "SHF_INFO_LINK");
    mapResult.insert(0x00000080, "SHF_LINK_ORDER");
    mapResult.insert(0x00000100, "SHF_OS_NONCONFORMING");
    mapResult.insert(0x00000200, "SHF_GROUP");
    mapResult.insert(0x00000400, "SHF_TLS");
    mapResult.insert(0x0ff00000, "SHF_MASKOS");
    mapResult.insert(0xf0000000, "SHF_MASKPROC");
    mapResult.insert(0x40000000, "SHF_ORDERED");
    mapResult.insert(0x80000000, "SHF_EXCLUDE");

    return mapResult;
}

QMap<quint64, QString> XELF::getSectionFlagsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000001, "WRITE");
    mapResult.insert(0x00000002, "ALLOC");
    mapResult.insert(0x00000004, "EXECINSTR");
    mapResult.insert(0x00000010, "MERGE");
    mapResult.insert(0x00000020, "STRINGS");
    mapResult.insert(0x00000040, "INFO_LINK");
    mapResult.insert(0x00000080, "LINK_ORDER");
    mapResult.insert(0x00000100, "OS_NONCONFORMING");
    mapResult.insert(0x00000200, "GROUP");
    mapResult.insert(0x00000400, "TLS");
    mapResult.insert(0x0ff00000, "MASKOS");
    mapResult.insert(0xf0000000, "MASKPROC");
    mapResult.insert(0x40000000, "ORDERED");
    mapResult.insert(0x80000000, "EXCLUDE");

    return mapResult;
}

QMap<quint64, QString> XELF::getProgramTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "PT_NULL");
    mapResult.insert(1, "PT_LOAD");
    mapResult.insert(2, "PT_DYNAMIC");
    mapResult.insert(3, "PT_INTERP");
    mapResult.insert(4, "PT_NOTE");
    mapResult.insert(5, "PT_SHLIB");
    mapResult.insert(6, "PT_PHDR");
    mapResult.insert(7, "PT_TLS");
    mapResult.insert(8, "PT_NUM");
    mapResult.insert(0x60000000, "PT_LOOS");
    mapResult.insert(0x6464e550, "PT_SUNW_UNWIND");
    mapResult.insert(0x6474e550, "PT_GNU_EH_FRAME");  // PT_SUNW_EH_FRAME
    mapResult.insert(0x6474e551, "PT_GNU_STACK");
    mapResult.insert(0x6474e552, "PT_GNU_RELRO");
    mapResult.insert(0x6474e553, "PT_GNU_PROPERTY");
    mapResult.insert(0x65041580, "PT_PAX_FLAGS");
    mapResult.insert(0x6ffffffa, "PT_LOSUNW");
    mapResult.insert(0x6ffffffa, "PT_SUNWBSS");
    mapResult.insert(0x6ffffffb, "PT_SUNWSTACK");
    mapResult.insert(0x6ffffffc, "PT_SUNWDTRACE");
    mapResult.insert(0x6ffffffd, "PT_SUNWCAP");
    mapResult.insert(0x6fffffff, "PT_HIOS");  // PT_HISUNW
    mapResult.insert(0x70000000, "PT_LOPROC");
    mapResult.insert(0x7fffffff, "PT_HIPROC");
    //    mapResult.insert(0x70000000,"PT_MIPXELF_DEF::REGINFO");

    return mapResult;
}

QMap<quint64, QString> XELF::getProgramTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NULL");
    mapResult.insert(1, "LOAD");
    mapResult.insert(2, "DYNAMIC");
    mapResult.insert(3, "INTERP");
    mapResult.insert(4, "NOTE");
    mapResult.insert(5, "SHLIB");
    mapResult.insert(6, "PHDR");
    mapResult.insert(7, "TLS");
    mapResult.insert(8, "NUM");
    mapResult.insert(0x60000000, "LOOS");
    mapResult.insert(0x6464e550, "SUNW_UNWIND");
    mapResult.insert(0x6474e550, "GNU_EH_FRAME");  // SUNW_EH_FRAME
    mapResult.insert(0x6474e551, "GNU_STACK");
    mapResult.insert(0x6474e552, "GNU_RELRO");
    mapResult.insert(0x6474e553, "GNU_PROPERTY");
    mapResult.insert(0x65041580, "PAX_FLAGS");
    mapResult.insert(0x6ffffffa, "LOSUNW");
    mapResult.insert(0x6ffffffa, "SUNWBSS");
    mapResult.insert(0x6ffffffb, "SUNWSTACK");
    mapResult.insert(0x6ffffffc, "SUNWDTRACE");
    mapResult.insert(0x6ffffffd, "SUNWCAP");
    mapResult.insert(0x6fffffff, "HIOS");  // HISUNW
    mapResult.insert(0x70000000, "LOPROC");
    mapResult.insert(0x7fffffff, "HIPROC");
    //    mapResult.insert(0x70000000,"MIPXELF_DEF::REGINFO");

    return mapResult;
}

QMap<quint64, QString> XELF::getProgramFlags()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000001, "PF_X");
    mapResult.insert(0x00000002, "PF_W");
    mapResult.insert(0x00000004, "PF_R");
    mapResult.insert(0x0ff00000, "PF_MASKOS");
    mapResult.insert(0xf0000000, "PF_MASKPROC");

    return mapResult;
}

QMap<quint64, QString> XELF::getProgramFlagsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00000001, "X");
    mapResult.insert(0x00000002, "W");
    mapResult.insert(0x00000004, "R");
    mapResult.insert(0x0ff00000, "MASKOS");
    mapResult.insert(0xf0000000, "MASKPROC");

    return mapResult;
}

quint32 XELF::getSectionStringTable()
{
    return getSectionStringTable(is64());
}

quint32 XELF::getSectionStringTable(bool bIs64)
{
    quint32 nResult = 0;

    if (bIs64) {
        nResult = getHdr64_shstrndx();
    } else {
        nResult = getHdr32_shstrndx();
    }

    if (nResult == 0) {
        quint64 nOffset = 0;

        if (bIs64) {
            nOffset = getHdr64_shoff();
        } else {
            nOffset = getHdr32_shoff();
        }

        if (nOffset) {
            bool bIsBigEndian = isBigEndian();

            if (bIs64) {
                XELF_DEF::Elf64_Shdr record = _readElf64_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_link;
            } else {
                XELF_DEF::Elf32_Shdr record = _readElf32_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_link;
            }
        }
    }

    return nResult;
}

XBinary::OFFSETSIZE XELF::getSectionOffsetSize(quint32 nSection)
{
    OFFSETSIZE osResult = {};

    if (is64()) {
        XELF_DEF::Elf64_Shdr section_header = getElf64_Shdr(nSection);
        osResult.nOffset = section_header.sh_offset;
        osResult.nSize = section_header.sh_size;
    } else {
        XELF_DEF::Elf32_Shdr section_header = getElf32_Shdr(nSection);
        osResult.nOffset = section_header.sh_offset;
        osResult.nSize = section_header.sh_size;
    }

    return osResult;
}

QMap<quint32, QString> XELF::getStringsFromSection(quint32 nSection)
{
    QMap<quint32, QString> mapResult;

    if (nSection != XELF_DEF::S_SHN_UNDEF) {
        QByteArray baSection = getSection(nSection);

        mapResult = getStringsFromSectionData(&baSection);
    }

    return mapResult;
}

QMap<quint32, QString> XELF::getStringsFromSectionData(QByteArray *pbaData)
{
    QMap<quint32, QString> mapResult;

    qint32 nSize = pbaData->size();
    char *pOffset = pbaData->data();
    quint32 nCurrentOffset = 0;

    while (nSize > 0) {
        QString sString(pOffset + nCurrentOffset);

        if (sString.length()) {
            mapResult.insert(nCurrentOffset, sString);
        }

        nCurrentOffset += (quint32)sString.length() + 1;
        nSize -= sString.length() + 1;
    }

    return mapResult;
}

QString XELF::getStringFromSection(quint32 nIndex, quint32 nSection)
{
    QString sResult;

    XBinary::OFFSETSIZE osRegion = getSectionOffsetSize(nSection);

    sResult = getStringFromIndex(osRegion.nOffset, osRegion.nSize, nIndex);

    return sResult;
}

QMap<quint32, QString> XELF::getStringsFromMainSection()
{
    quint32 nSection = getSectionStringTable();

    return getStringsFromSection(nSection);
}

QString XELF::getStringFromMainSection(quint32 nIndex)
{
    quint32 nSection = getSectionStringTable();

    return getStringFromSection(nIndex, nSection);  // TODO optimize
}

QByteArray XELF::getSection(quint32 nIndex)
{
    qint64 nOffset = 0;
    qint64 nSize = 0;

    if (is64()) {
        XELF_DEF::Elf64_Shdr section_header = getElf64_Shdr(nIndex);
        nSize = section_header.sh_size;
        nOffset = (isImage()) ? (section_header.sh_addr) : (section_header.sh_offset);
    } else {
        XELF_DEF::Elf32_Shdr section_header = getElf32_Shdr(nIndex);
        nSize = section_header.sh_size;
        nOffset = (isImage()) ? (section_header.sh_addr) : (section_header.sh_offset);
    }

    return read_array(nOffset, nSize);
}

bool XELF::isSectionValid(quint32 nIndex)
{
    quint32 nNumberOfSections = getNumberOfSections();

    return (nIndex < nNumberOfSections) && (nIndex != (quint32)-1);
}

QList<XELF_DEF::Elf32_Shdr> XELF::getElf32_ShdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf32_Shdr> listResult;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 nOffset = getHdr32_shoff();

    if (nOffset) {
        bool bIsBigEndian = isBigEndian();

        if (nLimit != -1) {
            nNumberOfSections = qMin((quint32)nLimit, nNumberOfSections);
        }

        for (quint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf32_Shdr record = _readElf32_Shdr(nOffset, bIsBigEndian);

            listResult.append(record);

            nOffset += sizeof(XELF_DEF::Elf32_Shdr);
        }
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Shdr> XELF::getElf64_ShdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf64_Shdr> listResult;

    quint32 nNumberOfSections = getNumberOfSections();

    quint64 nOffset = getHdr64_shoff();

    if (nOffset) {
        bool bIsBigEndian = isBigEndian();

        if (nLimit != -1) {
            nNumberOfSections = qMin((quint32)nLimit, nNumberOfSections);
        }

        for (quint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf64_Shdr record = _readElf64_Shdr(nOffset, bIsBigEndian);

            listResult.append(record);

            nOffset += sizeof(XELF_DEF::Elf64_Shdr);
        }
    }

    return listResult;
}

QList<XELF_DEF::Elf_Shdr> XELF::getElf_ShdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf_Shdr> listResult;

    bool bIs64 = is64();

    if (bIs64) {
        QList<XELF_DEF::Elf64_Shdr> listSectionHeaders = getElf64_ShdrList(nLimit);
        qint32 nNumberOfSections = listSectionHeaders.count();

        for (qint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf_Shdr record = {};

            record.sh_name = listSectionHeaders.at(i).sh_name;
            record.sh_type = listSectionHeaders.at(i).sh_type;
            record.sh_flags = listSectionHeaders.at(i).sh_flags;
            record.sh_addr = listSectionHeaders.at(i).sh_addr;
            record.sh_offset = listSectionHeaders.at(i).sh_offset;
            record.sh_size = listSectionHeaders.at(i).sh_size;
            record.sh_link = listSectionHeaders.at(i).sh_link;
            record.sh_info = listSectionHeaders.at(i).sh_info;
            record.sh_addralign = listSectionHeaders.at(i).sh_addralign;
            record.sh_entsize = listSectionHeaders.at(i).sh_entsize;

            listResult.append(record);
        }
    } else {
        QList<XELF_DEF::Elf32_Shdr> listSectionHeaders = getElf32_ShdrList(nLimit);
        qint32 nNumberOfSections = listSectionHeaders.count();

        for (qint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf_Shdr record = {};

            record.sh_name = listSectionHeaders.at(i).sh_name;
            record.sh_type = listSectionHeaders.at(i).sh_type;
            record.sh_flags = listSectionHeaders.at(i).sh_flags;
            record.sh_addr = listSectionHeaders.at(i).sh_addr;
            record.sh_offset = listSectionHeaders.at(i).sh_offset;
            record.sh_size = listSectionHeaders.at(i).sh_size;
            record.sh_link = listSectionHeaders.at(i).sh_link;
            record.sh_info = listSectionHeaders.at(i).sh_info;
            record.sh_addralign = listSectionHeaders.at(i).sh_addralign;
            record.sh_entsize = listSectionHeaders.at(i).sh_entsize;

            listResult.append(record);
        }
    }

    return listResult;
}

XELF_DEF::Elf32_Shdr XELF::getElf32_Shdr(quint32 nIndex)
{
    XELF_DEF::Elf32_Shdr result = {};

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 nOffset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);

        result = _readElf32_Shdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf64_Shdr XELF::getElf64_Shdr(quint32 nIndex)
{
    XELF_DEF::Elf64_Shdr result = {};

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 nOffset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);

        result = _readElf64_Shdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf32_Shdr XELF::_readElf32_Shdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Shdr result = {};

    result.sh_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_name), bIsBigEndian);
    result.sh_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_type), bIsBigEndian);
    result.sh_flags = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_flags), bIsBigEndian);
    result.sh_addr = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_addr), bIsBigEndian);
    result.sh_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_offset), bIsBigEndian);
    result.sh_size = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_size), bIsBigEndian);
    result.sh_link = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_link), bIsBigEndian);
    result.sh_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_info), bIsBigEndian);
    result.sh_addralign = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_addralign), bIsBigEndian);
    result.sh_entsize = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_entsize), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Shdr XELF::_readElf64_Shdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Shdr result = {};

    result.sh_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_name), bIsBigEndian);
    result.sh_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_type), bIsBigEndian);
    result.sh_flags = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_flags), bIsBigEndian);
    result.sh_addr = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_addr), bIsBigEndian);
    result.sh_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_offset), bIsBigEndian);
    result.sh_size = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_size), bIsBigEndian);
    result.sh_link = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_link), bIsBigEndian);
    result.sh_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_info), bIsBigEndian);
    result.sh_addralign = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_addralign), bIsBigEndian);
    result.sh_entsize = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_entsize), bIsBigEndian);

    return result;
}

quint32 XELF::getElf32_Shdr_name(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_name), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_type), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_flags(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_flags), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_addr(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addr), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_offset(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_offset), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_size(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_size), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_link(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_link), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_info(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_info), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_addralign(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addralign), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_entsize(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_entsize), bIsBigEndian);
    }

    return result;
}

void XELF::setElf32_Shdr_name(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_name), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_flags(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_addr(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addr), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_offset(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_size(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_size), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_link(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_link), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_info(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_info), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_addralign(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addralign), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_entsize(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_entsize), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf64_Shdr_name(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_name), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Shdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_type), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_flags(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_flags), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_addr(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addr), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_offset(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_offset), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_size(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_size), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Shdr_link(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_link), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Shdr_info(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_info), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_addralign(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addralign), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_entsize(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_entsize), bIsBigEndian);
    }

    return result;
}

void XELF::setElf64_Shdr_name(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_name), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_flags(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_addr(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addr), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_offset(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_size(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_size), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_link(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_link), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_info(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_info), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_addralign(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addralign), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_entsize(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_entsize), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf_Shdr_name(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_name;
    }

    return nResult;
}

quint32 XELF::getElf_Shdr_type(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_type;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_flags(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_flags;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_addr(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_addr;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_offset(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_offset;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_size(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_size;
    }

    return nResult;
}

quint32 XELF::getElf_Shdr_link(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_link;
    }

    return nResult;
}

quint32 XELF::getElf_Shdr_info(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_info;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_addralign(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_addralign;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_entsize(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_entsize;
    }

    return nResult;
}

qint64 XELF::getShdrOffset(quint32 nIndex)
{
    quint64 nResult = -1;

    bool bIs64 = is64();
    quint32 nNumberOfSections = 0;

    if (bIs64) {
        nNumberOfSections = getNumberOfSections();

        if (nIndex < nNumberOfSections) {
            nResult = getHdr64_shoff() + nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        }
    } else {
        nNumberOfSections = getNumberOfSections();

        if (nIndex < nNumberOfSections) {
            nResult = getHdr32_shoff() + nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        }
    }

    return nResult;
}

qint64 XELF::getShdrSize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XELF_DEF::Elf64_Shdr);
    } else {
        nResult = sizeof(XELF_DEF::Elf32_Shdr);
    }

    return nResult;
}

QList<XELF_DEF::Elf32_Phdr> XELF::getElf32_PhdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf32_Phdr> result;

    quint32 nNumberOfProgramms = getHdr32_phnum();

    if (nLimit != -1) {
        nNumberOfProgramms = qMin((quint32)nLimit, nNumberOfProgramms);
    }

    quint32 nOffset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    for (quint32 i = 0; i < nNumberOfProgramms; i++) {
        XELF_DEF::Elf32_Phdr record = _readElf32_Phdr(nOffset, bIsBigEndian);

        result.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Phdr);
    }

    return result;
}

QList<XELF_DEF::Elf64_Phdr> XELF::getElf64_PhdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf64_Phdr> result;

    quint32 nNumberOfProgramms = getHdr64_phnum();

    if (nLimit != -1) {
        nNumberOfProgramms = qMin((quint32)nLimit, nNumberOfProgramms);
    }

    quint64 nOffset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    for (quint32 i = 0; i < nNumberOfProgramms; i++) {
        XELF_DEF::Elf64_Phdr record = _readElf64_Phdr(nOffset, bIsBigEndian);

        result.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Phdr);
    }

    return result;
}

QList<XELF_DEF::Elf_Phdr> XELF::getElf_PhdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf_Phdr> listResult;

    bool bIs64 = is64();

    if (bIs64) {
        QList<XELF_DEF::Elf64_Phdr> listProgramHeaders = getElf64_PhdrList(nLimit);

        qint32 nNumberOfPrograms = listProgramHeaders.count();

        for (qint32 i = 0; i < nNumberOfPrograms; i++) {
            XELF_DEF::Elf_Phdr record = {};

            record.p_type = listProgramHeaders.at(i).p_type;
            record.p_flags = listProgramHeaders.at(i).p_flags;
            record.p_offset = listProgramHeaders.at(i).p_offset;
            record.p_vaddr = listProgramHeaders.at(i).p_vaddr;
            record.p_paddr = listProgramHeaders.at(i).p_paddr;
            record.p_filesz = listProgramHeaders.at(i).p_filesz;
            record.p_memsz = listProgramHeaders.at(i).p_memsz;
            record.p_align = listProgramHeaders.at(i).p_align;

            listResult.append(record);
        }
    } else {
        QList<XELF_DEF::Elf32_Phdr> listProgramHeaders = getElf32_PhdrList(nLimit);

        qint32 nNumberOfPrograms = listProgramHeaders.count();

        for (qint32 i = 0; i < nNumberOfPrograms; i++) {
            XELF_DEF::Elf_Phdr record = {};

            record.p_type = listProgramHeaders.at(i).p_type;
            record.p_flags = listProgramHeaders.at(i).p_flags;
            record.p_offset = listProgramHeaders.at(i).p_offset;
            record.p_vaddr = listProgramHeaders.at(i).p_vaddr;
            record.p_paddr = listProgramHeaders.at(i).p_paddr;
            record.p_filesz = listProgramHeaders.at(i).p_filesz;
            record.p_memsz = listProgramHeaders.at(i).p_memsz;
            record.p_align = listProgramHeaders.at(i).p_align;

            listResult.append(record);
        }
    }

    return listResult;
}

XELF_DEF::Elf32_Phdr XELF::getElf32_Phdr(quint32 nIndex)
{
    XELF_DEF::Elf32_Phdr result = {};

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 nOffset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = _readElf32_Phdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf64_Phdr XELF::getElf64_Phdr(quint32 nIndex)
{
    XELF_DEF::Elf64_Phdr result = {};

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 nOffset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = _readElf64_Phdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf32_Phdr XELF::_readElf32_Phdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Phdr result = {};

    result.p_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_type), bIsBigEndian);
    result.p_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_offset), bIsBigEndian);
    result.p_vaddr = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_vaddr), bIsBigEndian);
    result.p_paddr = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_paddr), bIsBigEndian);
    result.p_filesz = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_filesz), bIsBigEndian);
    result.p_memsz = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_memsz), bIsBigEndian);
    result.p_flags = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_flags), bIsBigEndian);
    result.p_align = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_align), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Phdr XELF::_readElf64_Phdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Phdr result = {};

    result.p_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_type), bIsBigEndian);
    result.p_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_offset), bIsBigEndian);
    result.p_vaddr = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_vaddr), bIsBigEndian);
    result.p_paddr = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_paddr), bIsBigEndian);
    result.p_filesz = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_filesz), bIsBigEndian);
    result.p_memsz = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_memsz), bIsBigEndian);
    result.p_flags = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_flags), bIsBigEndian);
    result.p_align = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_align), bIsBigEndian);

    return result;
}

quint32 XELF::getElf32_Phdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_type), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_offset(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_offset), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_vaddr(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_vaddr), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_paddr(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_paddr), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_filesz(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_filesz), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_memsz(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_memsz), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_flags(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_flags), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_align(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_align), bIsBigEndian);
    }

    return result;
}

void XELF::setElf32_Phdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_offset(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_vaddr(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_vaddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_paddr(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_paddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_filesz(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_filesz), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_memsz(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_memsz), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_flags(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_align(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_align), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf64_Phdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_type), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_offset(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_offset), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_vaddr(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_vaddr), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_paddr(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_paddr), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_filesz(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_filesz), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_memsz(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_memsz), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Phdr_flags(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_flags), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_align(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_align), bIsBigEndian);
    }

    return result;
}

void XELF::setElf64_Phdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_offset(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_vaddr(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_vaddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_paddr(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_paddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_filesz(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_filesz), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_memsz(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_memsz), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_flags(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_align(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_align), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf_Phdr_type(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_type;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_offset(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_offset;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_vaddr(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_vaddr;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_paddr(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_paddr;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_filesz(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_filesz;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_memsz(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_memsz;
    }

    return nResult;
}

quint32 XELF::getElf_Phdr_flags(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_flags;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_align(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_align;
    }

    return nResult;
}

qint64 XELF::getPhdrOffset(quint32 nIndex)
{
    quint64 nResult = -1;

    bool bIs64 = is64();
    quint32 nNumberOfPrograms = 0;

    if (bIs64) {
        nNumberOfPrograms = getHdr64_phnum();

        if (nIndex < nNumberOfPrograms) {
            nResult = getHdr64_phoff() + nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        }
    } else {
        nNumberOfPrograms = getHdr32_phnum();

        if (nIndex < nNumberOfPrograms) {
            nResult = getHdr32_phoff() + nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        }
    }

    return nResult;
}

qint64 XELF::getPhdrSize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XELF_DEF::Elf64_Phdr);
    } else {
        nResult = sizeof(XELF_DEF::Elf32_Phdr);
    }

    return nResult;
}

qint32 XELF::getSectionIndexByName(const QString &sSectionName)
{
    qint32 nResult = -1;

    quint32 nNumberOfSections = getNumberOfSections();

    if (is64()) {
        for (quint32 i = 0; i < nNumberOfSections; i++) {
            quint32 nCurrentNameIndex = getElf64_Shdr_name(i);

            if (getStringFromMainSection(nCurrentNameIndex) == sSectionName) {
                nResult = i;
                break;
            }
        }
    } else {
        for (quint32 i = 0; i < nNumberOfSections; i++) {
            quint32 nCurrentNameIndex = getElf32_Shdr_name(i);

            if (getStringFromMainSection(nCurrentNameIndex) == sSectionName) {
                nResult = i;
                break;
            }
        }
    }

    return nResult;
}

qint32 XELF::getSectionIndexByName(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords)
{
    qint32 nResult = -1;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sSectionName) {
            nResult = i;
            break;
        }
    }

    return nResult;
}

QByteArray XELF::getSectionByName(const QString &sSectionName)
{
    QByteArray baResult;

    quint32 nIndex = getSectionIndexByName(sSectionName);

    if (nIndex != (quint32)-1) {
        baResult = getSection(nIndex);
    }

    return baResult;
}

XBinary::OS_STRING XELF::getProgramInterpreterName()
{
    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);

    return getProgramInterpreterName(&listProgramHeaders);
}

XBinary::OS_STRING XELF::getProgramInterpreterName(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    OS_STRING result = {};

    QList<XELF_DEF::Elf_Phdr> listInterps = _getPrograms(pListProgramHeaders, XELF_DEF::S_PT_INTERP);

    if (listInterps.count()) {
        result = getOsAnsiString(listInterps.at(0).p_offset, listInterps.at(0).p_filesz);
    }

    return result;
}

XBinary::OS_STRING XELF::getProgramInterpreterName(QList<SECTION_RECORD> *pListSectionRecords)
{
    OS_STRING result = {};

    QList<SECTION_RECORD> listInterps = _getSectionRecords(pListSectionRecords, ".interp");

    if (listInterps.count()) {
        result = getOsAnsiString(listInterps.at(0).nOffset, listInterps.at(0).nSize);
    }

    return result;
}

QList<QString> XELF::getCommentStrings(QList<SECTION_RECORD> *pListSectionRecords)
{
    // TODO Optimize
    return getCommentStrings(getSectionIndexByName(".comment", pListSectionRecords));
}

QList<QString> XELF::getCommentStrings(qint32 nSection)
{
    return getStringsFromSection(nSection).values();
}

QList<XELF::NOTE> XELF::getNotes()
{
    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);

    return getNotes(&listProgramHeaders);
}

// QString XELF::getCompatibleKernelVersion()
//{
//     QString sResult;

//    QByteArray baData=getSectionByName(".note.ABI-tag");
//    bool bIsBigEndian=isBigEndian();
//    NOTE note=getNote(baData,bIsBigEndian);

//    if((note.nType==1)&&(note.name=="GNU"))
//    {
//        quint32 kv[4];
//        kv[0]=_read_uint32(note.desc.data()+0,bIsBigEndian);
//        kv[1]=_read_uint32(note.desc.data()+4,bIsBigEndian);
//        kv[2]=_read_uint32(note.desc.data()+8,bIsBigEndian);
//        kv[3]=_read_uint32(note.desc.data()+12,bIsBigEndian);

//        if(kv[0]==0)
//        {
//            sResult=QString("%1.%2.%3").arg(QString::number(kv[1]),QString::number(kv[2]),QString::number(kv[3]));
//        }
//    }

//    return sResult;
//}

QList<XELF::NOTE> XELF::getNotes(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    QList<NOTE> listResult;

    QList<XELF_DEF::Elf_Phdr> listNotes = _getPrograms(pListProgramHeaders, XELF_DEF::S_PT_NOTE);

    bool bIsBigEndian = isBigEndian();

    qint32 nNumberOfNotes = listNotes.count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        // TODO Image
        qint64 nOffset = listNotes.at(i).p_offset;
        qint64 nSize = listNotes.at(i).p_filesz;

        listResult.append(_getNotes(nOffset, nSize, bIsBigEndian));
    }

    return listResult;
}

QList<XELF::NOTE> XELF::getNotes(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    QList<NOTE> listResult;

    QList<XELF_DEF::Elf_Shdr> listNotes = _getSections(pListSectionHeaders, XELF_DEF::S_SHT_NOTE);

    bool bIsBigEndian = isBigEndian();

    qint32 nNumberOfNotes = listNotes.count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        // TODO Image
        qint64 nOffset = listNotes.at(i).sh_offset;
        qint64 nSize = listNotes.at(i).sh_size;

        listResult.append(_getNotes(nOffset, nSize, bIsBigEndian));
    }

    return listResult;
}

QList<XELF::NOTE> XELF::_getNotes(qint64 nOffset, qint64 nSize, bool bIsBigEndian)
{
    QList<NOTE> listResult;

    qint32 nNumberOfRecords = 0;

    while (nSize > 0) {
        NOTE note = _readNote(nOffset, nSize, bIsBigEndian);

        if (note.nSize) {
            listResult.append(note);

            nOffset += note.nSize;
            nSize -= note.nSize;
        } else {
            break;
        }

        nNumberOfRecords++;

        if (nNumberOfRecords > 1000) {
            break;
        }
    }

    return listResult;
}

XELF::NOTE XELF::_readNote(qint64 nOffset, qint64 nSize, bool bIsBigEndian)
{
    NOTE result = {};

    if (nSize >= 6) {
        quint32 nNameLength = read_uint32(nOffset + 0, bIsBigEndian);
        quint32 nDescLength = read_uint32(nOffset + 4, bIsBigEndian);
        quint32 nType = read_uint32(nOffset + 8, bIsBigEndian);

        qint32 nNoteSize = 12 + S_ALIGN_UP(nNameLength, 4) + S_ALIGN_UP(nDescLength, 4);

        if (nSize >= nNoteSize) {
            result.nOffset = nOffset;
            result.nSize = nNoteSize;
            result.nType = nType;

            if (nNameLength >= 1) {
                result.sName = read_ansiString(nOffset + 12, nNameLength - 1);
            }

            result.nDataOffset = nOffset + 12 + S_ALIGN_UP(nNameLength, 4);
            result.nDataSize = qMax((nOffset + nSize) - result.nDataOffset, (qint64)0);
        }
    }

    return result;
}

bool XELF::isNotePresent(const QString &sName)
{
    QList<XELF::NOTE> listNotes = getNotes();

    return isNotePresent(&listNotes, sName);
}

bool XELF::isNotePresent(QList<NOTE> *pListNotes, const QString &sName)
{
    bool bResult = false;

    qint32 nNumberOfNotes = pListNotes->count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        if (pListNotes->at(i).sName == sName) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XELF::NOTE XELF::getNote(QList<NOTE> *pListNotes, const QString &sName)
{
    NOTE result = {};

    for (qint32 i = 0; i < pListNotes->count(); i++) {
        if (pListNotes->at(i).sName == sName) {
            result = pListNotes->at(i);
            break;
        }
    }

    return result;
}

bool XELF::isNotePresent(quint32 nType)
{
    QList<XELF::NOTE> listNotes = getNotes();

    return isNotePresent(&listNotes, nType);
}

bool XELF::isNotePresent(QList<NOTE> *pListNotes, quint32 nType)
{
    bool bResult = false;

    qint32 nNumberOfNotes = pListNotes->count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        if (pListNotes->at(i).nType == nType) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XELF::NOTE XELF::getNote(QList<NOTE> *pListNotes, quint32 nType)
{
    NOTE result = {};

    for (qint32 i = 0; i < pListNotes->count(); i++) {
        if (pListNotes->at(i).nType == nType) {
            result = pListNotes->at(i);
            break;
        }
    }

    return result;
}

bool XELF::isNotePresent(quint32 nType, const QString &sName)
{
    QList<XELF::NOTE> listNotes = getNotes();

    return isNotePresent(&listNotes, nType, sName);
}

bool XELF::isNotePresent(QList<NOTE> *pListNotes, quint32 nType, const QString &sName)
{
    bool bResult = false;

    qint32 nNumberOfNotes = pListNotes->count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        if ((pListNotes->at(i).nType == nType) && (pListNotes->at(i).sName == sName)) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XELF::NOTE XELF::getNote(QList<NOTE> *pListNotes, quint32 nType, const QString &sName)
{
    NOTE result = {};

    for (qint32 i = 0; i < pListNotes->count(); i++) {
        if ((pListNotes->at(i).nType == nType) && (pListNotes->at(i).sName == sName)) {
            result = pListNotes->at(i);
            break;
        }
    }

    return result;
}

QList<XELF::TAG_STRUCT> XELF::getTagStructs()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);

    return getTagStructs(&listProgramHeaders, &memoryMap);
}

QList<XELF::TAG_STRUCT> XELF::getTagStructs(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders, XBinary::_MEMORY_MAP *pMemoryMap)
{
    QList<TAG_STRUCT> listResult;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    QList<XELF_DEF::Elf_Phdr> listTags = _getPrograms(pListProgramHeaders, XELF_DEF::S_PT_DYNAMIC);

    qint32 nNumberOfTags = listTags.count();

    for (qint32 i = 0; i < nNumberOfTags; i++) {
        qint64 nOffset = listTags.at(i).p_offset;  //  Check image
        qint64 nSize = listTags.at(i).p_filesz;

        if (isOffsetAndSizeValid(pMemoryMap, nOffset, nSize)) {
            listResult.append(_getTagStructs(nOffset, nSize, bIs64, bIsBigEndian));
        }
    }

    return listResult;
}

QList<XELF::TAG_STRUCT> XELF::_getTagStructs(qint64 nOffset, qint64 nSize, bool bIs64, bool bIsBigEndian)
{
    QList<TAG_STRUCT> listResult;

    while (nSize > 0) {
        TAG_STRUCT tagStruct = {};
        tagStruct.nOffset = nOffset;

        if (bIs64) {
            tagStruct.nTag = read_int64(nOffset, bIsBigEndian);
            tagStruct.nValue = read_int64(nOffset + 8, bIsBigEndian);
            nOffset += 16;
            nSize -= 16;
        } else {
            tagStruct.nTag = read_int32(nOffset, bIsBigEndian);
            tagStruct.nValue = read_int32(nOffset + 4, bIsBigEndian);
            nOffset += 8;
            nSize -= 8;
        }

        if (tagStruct.nTag) {
            listResult.append(tagStruct);
        } else {
            break;
        }
    }

    return listResult;
}

QList<XELF::TAG_STRUCT> XELF::_getTagStructs(QList<XELF::TAG_STRUCT> *pListTagStructs, qint64 nTag)
{
    QList<TAG_STRUCT> listResult;

    qint32 nNumberOfTags = pListTagStructs->count();

    for (qint32 i = 0; i < nNumberOfTags; i++) {
        if (pListTagStructs->at(i).nTag == nTag) {
            listResult.append(pListTagStructs->at(i));
        }
    }

    return listResult;
}

qint64 XELF::getDynamicArraySize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = 16;  // TODO defs
    } else {
        nResult = 8;  // TODO defs
    }

    return nResult;
}

qint64 XELF::getDynamicArrayTag(qint64 nOffset)
{
    qint64 nResult = 0;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        nResult = read_int64(nOffset, bIsBigEndian);
    } else {
        nResult = read_int32(nOffset, bIsBigEndian);
    }

    return nResult;
}

qint64 XELF::getDynamicArrayValue(qint64 nOffset)
{
    qint64 nResult = 0;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        nResult = read_int64(nOffset + 8, bIsBigEndian);
    } else {
        nResult = read_int32(nOffset + 4, bIsBigEndian);
    }

    return nResult;
}

void XELF::setDynamicArrayTag(qint64 nOffset, qint64 nValue)
{
    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        write_int64(nOffset, nValue, bIsBigEndian);
    } else {
        write_int32(nOffset, (qint32)nValue, bIsBigEndian);
    }
}

void XELF::setDynamicArrayValue(qint64 nOffset, qint64 nValue)
{
    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        write_int64(nOffset + 8, nValue, bIsBigEndian);
    } else {
        write_int32(nOffset + 4, (qint32)nValue, bIsBigEndian);
    }
}

XBinary::OFFSETSIZE XELF::getStringTable()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listStructs = getTagStructs();

    return getStringTable(&memoryMap, &listStructs);
}

XBinary::OFFSETSIZE XELF::getStringTable(XBinary::_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pListTagStructs)
{
    OFFSETSIZE osResult = {};

    QList<TAG_STRUCT> listStrTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRTAB);
    QList<TAG_STRUCT> listStrSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRSZ);

    if (listStrTab.count() && listStrSize.count()) {
        qint64 nOffset = addressToOffset(pMemoryMap, listStrTab.at(0).nValue);
        qint64 nSize = listStrSize.at(0).nValue;

        if (isOffsetAndSizeValid(pMemoryMap, nOffset, nSize)) {
            osResult.nOffset = nOffset;
            osResult.nSize = nSize;
        }
    }

    return osResult;
}

QList<QString> XELF::getLibraries()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listTagStructs = getTagStructs();

    return getLibraries(&memoryMap, &listTagStructs);
}

QList<QString> XELF::getLibraries(_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pList)
{
    QList<QString> listResult;

    QList<TAG_STRUCT> listNeeded = _getTagStructs(pList, XELF_DEF::S_DT_NEEDED);

    OFFSETSIZE osStringTable = getStringTable(pMemoryMap, pList);

    if (osStringTable.nSize) {
        qint64 nOffset = osStringTable.nOffset;

        QByteArray baSection = read_array(nOffset, osStringTable.nSize);

        qint64 nSectionTableSize = baSection.size();

        qint32 nNumberOfNeededs = listNeeded.count();

        for (qint32 i = 0; i < nNumberOfNeededs; i++) {
            qint64 nValue = listNeeded.at(i).nValue;

            if ((nValue >= 0) & (nValue < nSectionTableSize)) {
                QString sLibrary = baSection.data() + nValue;

                listResult.append(sLibrary);
            }
        }
    }

    return listResult;
}

XBinary::OS_STRING XELF::getRunPath()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listTagStructs = getTagStructs();

    return getRunPath(&memoryMap, &listTagStructs);
}

XBinary::OS_STRING XELF::getRunPath(XBinary::_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pListTagStructs)
{
    OS_STRING result = {};

    QList<TAG_STRUCT> listRunPath = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_RUNPATH);
    QList<TAG_STRUCT> listStrTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRTAB);
    QList<TAG_STRUCT> listStrSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRSZ);

    if (listStrTab.count() && listStrSize.count() && listRunPath.count()) {
        qint64 nOffset = addressToOffset(pMemoryMap, listStrTab.at(0).nValue);
        qint64 nSize = listStrSize.at(0).nValue;
        qint64 nRunPath = listRunPath.at(0).nValue;

        if ((nRunPath >= 0) & (nRunPath < nSize)) {
            result.nOffset = nOffset + nRunPath;
            result.sString = read_ansiString(result.nOffset);
            result.nSize = result.sString.length();
        }
    }

    return result;
}

QList<XBinary::MAPMODE> XELF::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_SEGMENTS);
    listResult.append(MAPMODE_SECTIONS);

    return listResult;
}

QMap<quint64, QString> XELF::getDynamicTags()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "DT_NULL");
    mapResult.insert(1, "DT_NEEDED");
    mapResult.insert(2, "DT_PLTRELSZ");
    mapResult.insert(3, "DT_PLTGOT");
    mapResult.insert(4, "DT_HASH");
    mapResult.insert(5, "DT_STRTAB");
    mapResult.insert(6, "DT_SYMTAB");
    mapResult.insert(7, "DT_RELA");
    mapResult.insert(8, "DT_RELASZ");
    mapResult.insert(9, "DT_RELAENT");
    mapResult.insert(10, "DT_STRSZ");
    mapResult.insert(11, "DT_SYMENT");
    mapResult.insert(12, "DT_INIT");
    mapResult.insert(13, "DT_FINI");
    mapResult.insert(14, "DT_SONAME");
    mapResult.insert(15, "DT_RPATH");
    mapResult.insert(16, "DT_SYMBOLIC");
    mapResult.insert(17, "DT_REL");
    mapResult.insert(18, "DT_RELSZ");
    mapResult.insert(19, "DT_RELENT");
    mapResult.insert(20, "DT_PLTREL");
    mapResult.insert(21, "DT_DEBUG");
    mapResult.insert(22, "DT_TEXTREL");
    mapResult.insert(23, "DT_JMPREL");
    mapResult.insert(24, "DT_BIND_NOW");
    mapResult.insert(25, "DT_INIT_ARRAY");
    mapResult.insert(26, "DT_FINI_ARRAY");
    mapResult.insert(27, "DT_INIT_ARRAYSZ");
    mapResult.insert(28, "DT_FINI_ARRAYSZ");
    mapResult.insert(29, "DT_RUNPATH");
    mapResult.insert(30, "DT_FLAGS");
    mapResult.insert(32, "DT_PREINIT_ARRAY");  // DT_ENCODING
    mapResult.insert(33, "DT_PREINIT_ARRAYSZ");
    mapResult.insert(0x6000000d, "DT_LOOS");
    mapResult.insert(0x6000000e, "DT_SUNW_RTLDINF");
    mapResult.insert(0x6ffff000, "DT_HIOS");
    mapResult.insert(0x6ffffd00, "DT_VALRNGLO");
    mapResult.insert(0x6ffffdf8, "DT_CHECKSUM");
    mapResult.insert(0x6ffffdf9, "DT_PLTPADSZ");
    mapResult.insert(0x6ffffdfa, "DT_MOVEENT");
    mapResult.insert(0x6ffffdfb, "DT_MOVESZ");
    mapResult.insert(0x6ffffdfc, "DT_FEATURE_1");
    mapResult.insert(0x6ffffdfd, "DT_POSFLAG_1");
    mapResult.insert(0x6ffffdfe, "DT_SYMINSZ");
    mapResult.insert(0x6ffffdff, "DT_SYMINENT");
    mapResult.insert(0x6ffffdff, "DT_VALRNGHI");
    mapResult.insert(0x6ffffe00, "DT_ADDRRNGLO");
    mapResult.insert(0x6ffffef5, "DT_GNU_HASH");
    mapResult.insert(0x6ffffefa, "DT_CONFIG");
    mapResult.insert(0x6ffffefb, "DT_DEPAUDIT");
    mapResult.insert(0x6ffffefc, "DT_AUDIT");
    mapResult.insert(0x6ffffefd, "DT_PLTPAD");
    mapResult.insert(0x6ffffefe, "DT_MOVETAB");
    mapResult.insert(0x6ffffeff, "DT_SYMINFO");
    mapResult.insert(0x6ffffeff, "DT_ADDRRNGHI");
    mapResult.insert(0x6ffffff0, "DT_VERSYM");
    mapResult.insert(0x6ffffff9, "DT_RELACOUNT");
    mapResult.insert(0x6ffffffa, "DT_RELCOUNT");
    mapResult.insert(0x6ffffffb, "DT_FLAGS_1");
    mapResult.insert(0x6ffffffc, "DT_VERDEF");
    mapResult.insert(0x6ffffffd, "DT_VERDEFNUM");
    mapResult.insert(0x6ffffffe, "DT_VERNEED");
    mapResult.insert(0x6fffffff, "DT_VERNEEDNUM");
    mapResult.insert(0x70000000, "DT_LOPROC");
    mapResult.insert(0x70000001, "DT_SPARC_REGISTER");
    mapResult.insert(0x7ffffffd, "DT_AUXILIARY");
    mapResult.insert(0x7ffffffe, "DT_USED");
    mapResult.insert(0x7fffffff, "DT_HIPROC");  // DT_FILTER

    return mapResult;
}

QMap<quint64, QString> XELF::getDynamicTagsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NULL");
    mapResult.insert(1, "NEEDED");
    mapResult.insert(2, "PLTRELSZ");
    mapResult.insert(3, "PLTGOT");
    mapResult.insert(4, "HASH");
    mapResult.insert(5, "STRTAB");
    mapResult.insert(6, "SYMTAB");
    mapResult.insert(7, "RELA");
    mapResult.insert(8, "RELASZ");
    mapResult.insert(9, "RELAENT");
    mapResult.insert(10, "STRSZ");
    mapResult.insert(11, "SYMENT");
    mapResult.insert(12, "INIT");
    mapResult.insert(13, "FINI");
    mapResult.insert(14, "SONAME");
    mapResult.insert(15, "RPATH");
    mapResult.insert(16, "SYMBOLIC");
    mapResult.insert(17, "REL");
    mapResult.insert(18, "RELSZ");
    mapResult.insert(19, "RELENT");
    mapResult.insert(20, "PLTREL");
    mapResult.insert(21, "DEBUG");
    mapResult.insert(22, "TEXTREL");
    mapResult.insert(23, "JMPREL");
    mapResult.insert(24, "BIND_NOW");
    mapResult.insert(25, "INIT_ARRAY");
    mapResult.insert(26, "FINI_ARRAY");
    mapResult.insert(27, "INIT_ARRAYSZ");
    mapResult.insert(28, "FINI_ARRAYSZ");
    mapResult.insert(29, "RUNPATH");
    mapResult.insert(30, "FLAGS");
    mapResult.insert(32, "PREINIT_ARRAY");  // ENCODING
    mapResult.insert(33, "PREINIT_ARRAYSZ");
    mapResult.insert(0x6000000d, "LOOS");
    mapResult.insert(0x6000000e, "SUNW_RTLDINF");
    mapResult.insert(0x6ffff000, "HIOS");
    mapResult.insert(0x6ffffd00, "VALRNGLO");
    mapResult.insert(0x6ffffdf8, "CHECKSUM");
    mapResult.insert(0x6ffffdf9, "PLTPADSZ");
    mapResult.insert(0x6ffffdfa, "MOVEENT");
    mapResult.insert(0x6ffffdfb, "MOVESZ");
    mapResult.insert(0x6ffffdfc, "FEATURE_1");
    mapResult.insert(0x6ffffdfd, "POSFLAG_1");
    mapResult.insert(0x6ffffdfe, "SYMINSZ");
    mapResult.insert(0x6ffffdff, "SYMINENT");
    mapResult.insert(0x6ffffdff, "VALRNGHI");
    mapResult.insert(0x6ffffe00, "ADDRRNGLO");
    mapResult.insert(0x6ffffef5, "GNU_HASH");
    mapResult.insert(0x6ffffefa, "CONFIG");
    mapResult.insert(0x6ffffefb, "DEPAUDIT");
    mapResult.insert(0x6ffffefc, "AUDIT");
    mapResult.insert(0x6ffffefd, "PLTPAD");
    mapResult.insert(0x6ffffefe, "MOVETAB");
    mapResult.insert(0x6ffffeff, "SYMINFO");
    mapResult.insert(0x6ffffeff, "ADDRRNGHI");
    mapResult.insert(0x6ffffff0, "VERSYM");
    mapResult.insert(0x6ffffff9, "RELACOUNT");
    mapResult.insert(0x6ffffffa, "RELCOUNT");
    mapResult.insert(0x6ffffffb, "FLAGS_1");
    mapResult.insert(0x6ffffffc, "VERDEF");
    mapResult.insert(0x6ffffffd, "VERDEFNUM");
    mapResult.insert(0x6ffffffe, "VERNEED");
    mapResult.insert(0x6fffffff, "VERNEEDNUM");
    mapResult.insert(0x70000000, "LOPROC");
    mapResult.insert(0x70000001, "SPARC_REGISTER");
    mapResult.insert(0x7ffffffd, "AUXILIARY");
    mapResult.insert(0x7ffffffe, "USED");
    mapResult.insert(0x7fffffff, "HIPROC");  // FILTER

    return mapResult;
}

XBinary::_MEMORY_MAP XELF::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }
    // TODO Check alignment!
    XBinary::_MEMORY_MAP result = {};

    qint32 nIndex = 0;

    result.mode = getMode();
    if (result.mode == MODE_64) {
        result.fileType = FT_ELF64;
    } else {
        result.fileType = FT_ELF32;
    }

    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = getTypeAsString();
    // XADDR _nModuleAddress = getModuleAddress();
    result.nModuleAddress = getModuleAddress();
    result.nBinarySize = getSize();

    qint64 nMaxUsedOffset = 0;
    XADDR nMinUsedAddress = 0;
    XADDR nMaxUsedAddress = 0;

    if ((mapMode == MAPMODE_UNKNOWN) || (mapMode == MAPMODE_SEGMENTS)) {
        QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);
        QList<XELF_DEF::Elf_Phdr> listSegments = _getPrograms(&listProgramHeaders, XELF_DEF::S_PT_LOAD);

        //    bool bIs64=is64();
        qint32 nNumberOfSegments = listSegments.count();

        if (nNumberOfSegments) {
            nMinUsedAddress = listSegments.at(0).p_vaddr;
        }

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            QString sName = QString("%1(%2)").arg(QString("PT_LOAD"), QString::number(i));

            quint64 nVirtualAlign = listSegments.at(i).p_align;  // TODO Check!

            if (nVirtualAlign <= 1) {
                nVirtualAlign = 1;
            }
            //        quint64 nFileAlign=0x1; // TODO Check!!!
            quint64 nFileAlign = nVirtualAlign;
            XADDR nVirtualAddress = S_ALIGN_DOWN64(listSegments.at(i).p_vaddr, nVirtualAlign);
            qint64 nFileOffset = S_ALIGN_DOWN64(listSegments.at(i).p_offset, nFileAlign);

            qint64 nVirtualDelta = listSegments.at(i).p_vaddr - nVirtualAddress;
            qint64 nFileDelta = listSegments.at(i).p_offset - nFileOffset;

            qint64 nVirtualSize = S_ALIGN_UP64(nVirtualDelta + listSegments.at(i).p_memsz, nVirtualAlign);
            qint64 nFileSize = S_ALIGN_UP64(nFileDelta + listSegments.at(i).p_filesz, nFileAlign);

            if (nFileOffset + nFileSize > result.nBinarySize) {
                nFileSize = result.nBinarySize - nFileOffset;
            }

            if (nFileSize < 0) {
                nFileSize = 0;
            }

            // Padding
            if (nVirtualDelta) {
                if (nVirtualDelta > nFileDelta) {
                    XBinary::_MEMORY_RECORD record = {};

                    record.type = MMT_LOADSEGMENT;
                    record.nAddress = nVirtualAddress;
                    record.nSize = nVirtualDelta - nFileDelta;
                    record.nOffset = -1;
                    record.nIndex = nIndex++;
                    record.bIsVirtual = true;
                    record.sName = sName;

                    result.listRecords.append(record);
                }

                if (nVirtualDelta >= nFileDelta) {
                    XBinary::_MEMORY_RECORD record = {};

                    record.type = MMT_LOADSEGMENT;
                    record.nAddress = nVirtualAddress + nVirtualDelta - nFileDelta;
                    record.nSize = nFileDelta;
                    record.nOffset = nFileOffset;
                    record.nIndex = nIndex++;
                    record.bIsVirtual = false;
                    record.sName = sName;

                    result.listRecords.append(record);
                }
            }

            // Main
            {
                XBinary::_MEMORY_RECORD record = {};

                record.type = MMT_LOADSEGMENT;
                record.nAddress = listSegments.at(i).p_vaddr;
                record.nSize = listSegments.at(i).p_filesz;
                record.nOffset = listSegments.at(i).p_offset;
                record.nIndex = nIndex++;
                record.bIsVirtual = false;
                record.sName = sName;

                result.listRecords.append(record);
            }

            // padding
            if ((nFileOffset + nFileSize) - (listSegments.at(i).p_offset + listSegments.at(i).p_filesz) > 0) {
                XBinary::_MEMORY_RECORD record = {};

                record.type = MMT_LOADSEGMENT;
                record.nSize = (nFileOffset + nFileSize) - (listSegments.at(i).p_offset + listSegments.at(i).p_filesz);
                record.nAddress = listSegments.at(i).p_vaddr + listSegments.at(i).p_filesz;

                record.nOffset = listSegments.at(i).p_offset + listSegments.at(i).p_filesz;
                record.nIndex = nIndex++;
                record.bIsVirtual = false;
                record.sName = sName;

                result.listRecords.append(record);
            }

            // padding
            if (nVirtualSize - nFileSize > 0) {
                XBinary::_MEMORY_RECORD record = {};

                record.type = MMT_LOADSEGMENT;
                record.nSize = nVirtualSize - nFileSize;
                record.nAddress = (nVirtualAddress + nVirtualSize) - record.nSize;

                record.nOffset = -1;
                record.nIndex = nIndex++;
                record.bIsVirtual = true;
                record.sName = sName;

                result.listRecords.append(record);
            }

            nMaxUsedOffset = qMax(nMaxUsedOffset, nFileOffset + nFileSize);
            nMinUsedAddress = qMin(nVirtualAddress, nMinUsedAddress);
            nMaxUsedAddress = qMax(nVirtualAddress + nVirtualSize, nMaxUsedAddress);
        }
    } else if (mapMode == MAPMODE_SECTIONS) {
        bool bIs64 = (result.mode == MODE_64);

        qint32 nStringTableSection = getSectionStringTable(bIs64);
        QByteArray baStringTable = getSection(nStringTableSection);
        QList<XELF_DEF::Elf_Shdr> listSections = getElf_ShdrList(1000);
        QList<SECTION_RECORD> listSectionRecords = getSectionRecords(&listSections, isImage(), &baStringTable);

        qint32 nNumberOfSections = listSectionRecords.count();

        if (nNumberOfSections) {
            nMinUsedAddress = listSectionRecords.at(0).nAddress;
        }

        for (qint32 i = 0; i < nNumberOfSections; i++) {
            QString sSectionName = QString("%1(%2)['%3']").arg(tr("Section"), QString::number(i), listSectionRecords.at(i).sName);

            XBinary::_MEMORY_RECORD record = {};

            record.type = MMT_FILESEGMENT;
            record.nSize = listSectionRecords.at(i).nSize;

            if (record.nSize != 0) {
                record.nOffset = listSectionRecords.at(i).nOffset;
            } else {
                record.nOffset = -1;
                record.bIsVirtual = true;
            }

            if (listSectionRecords.at(i).nAddress != 0) {
                record.nAddress = listSectionRecords.at(i).nAddress;
            } else {
                record.nAddress = -1;
            }

            record.nIndex = nIndex++;
            record.sName = sSectionName;

            result.listRecords.append(record);

            nMaxUsedOffset = qMax(nMaxUsedOffset, listSectionRecords.at(i).nOffset + listSectionRecords.at(i).nSize);
            nMinUsedAddress = qMin(listSectionRecords.at(i).nAddress, nMinUsedAddress);
            nMaxUsedAddress = qMax(listSectionRecords.at(i).nAddress + listSectionRecords.at(i).nSize, nMaxUsedAddress);
        }
    }

    result.nImageSize = nMaxUsedAddress - nMinUsedAddress;

    qint64 nFixAddressDelta = getModuleAddress() - nMinUsedAddress;

    if (result.fileType == FT_ELF64) {
        result.nEntryPointAddress = getHdr64_entry() + nFixAddressDelta;
    } else {
        result.nEntryPointAddress = getHdr32_entry() + nFixAddressDelta;
    }

    qint32 _nNumberOfRecords = result.listRecords.count();

    for (qint32 i = 0; i < _nNumberOfRecords; i++) {
        result.listRecords[i].nAddress += nFixAddressDelta;
    }

    qint64 nMaxSectionOffset = nMaxUsedOffset;

    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    qint32 nNumberOfSections = listSectionHeaders.count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        nMaxSectionOffset = qMax(nMaxSectionOffset, (qint64)(listSectionHeaders.at(i).sh_offset + listSectionHeaders.at(i).sh_size));
    }

    qint64 nNoLoadableSize = result.nBinarySize - nMaxUsedOffset;

    if (nNoLoadableSize > 0) {
        XBinary::_MEMORY_RECORD record = {};

        record.type = MMT_NOLOADABLE;
        // TODO Section number!
        // TODO virtual sections!
        record.nAddress = -1;
        record.nSize = nNoLoadableSize;
        record.nOffset = nMaxUsedOffset;
        record.nIndex = nIndex++;

        result.listRecords.append(record);
    }

    return result;
}

qint64 XELF::getEntryPointOffset(_MEMORY_MAP *pMemoryMap)
{
    return addressToOffset(pMemoryMap, pMemoryMap->nEntryPointAddress);
}

QList<XELF::SECTION_RECORD> XELF::getSectionRecords(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders, bool bIsImage, QByteArray *pbaSectionTable)
{
    QList<SECTION_RECORD> listRecords;

    qint32 nNumberOfSections = pListSectionHeaders->count();
    quint32 nSectionTableSize = pbaSectionTable->size();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        SECTION_RECORD record = {};

        record.nSize = pListSectionHeaders->at(i).sh_size;
        record.nFlags = pListSectionHeaders->at(i).sh_flags;
        record.nAddress = pListSectionHeaders->at(i).sh_addr;

        if (bIsImage) {
            record.nOffset = pListSectionHeaders->at(i).sh_addr;
        } else {
            record.nOffset = pListSectionHeaders->at(i).sh_offset;
        }

        if (pListSectionHeaders->at(i).sh_name < nSectionTableSize) {
            record.sName = pbaSectionTable->data() + pListSectionHeaders->at(i).sh_name;
        }

        listRecords.append(record);
    }

    return listRecords;
}

bool XELF::isSectionNamePresent(const QString &sSectionName)
{
    bool bIs64 = is64();
    quint16 nStringTableSection = getSectionStringTable(bIs64);
    QByteArray baStringTable = getSection(nStringTableSection);
    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    QList<SECTION_RECORD> listSectionRecords = getSectionRecords(&listSectionHeaders, isImage(), &baStringTable);

    return isSectionNamePresent(sSectionName, &listSectionRecords);
}

bool XELF::isSectionNamePresent(const QString &sSectionName, QList<XELF::SECTION_RECORD> *pListSectionRecords)
{
    bool bResult = false;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sSectionName) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

qint32 XELF::getSectionNumber(const QString &sSectionName)
{
    bool bIs64 = is64();
    quint16 nStringTableSection = getSectionStringTable(bIs64);
    QByteArray baStringTable = getSection(nStringTableSection);
    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    QList<SECTION_RECORD> listSectionRecords = getSectionRecords(&listSectionHeaders, isImage(), &baStringTable);

    return getSectionNumber(sSectionName, &listSectionRecords);
}

qint32 XELF::getSectionNumber(const QString &sSectionName, QList<XELF::SECTION_RECORD> *pListSectionRecords)
{
    qint32 nResult = -1;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sSectionName) {
            nResult = i;
            break;
        }
    }

    return nResult;
}

XBinary::MODE XELF::getMode()
{
    MODE result = MODE_32;

    quint8 ident = getIdent_class();

    if (ident == XELF_DEF::S_ELFCLASS32) {
        result = MODE_32;
    } else if (ident == XELF_DEF::S_ELFCLASS64) {
        result = MODE_64;
    }

    return result;
}

QString XELF::getArch()
{
    return getMachinesS().value(getHdr32_machine(), tr("Unknown"));
}

qint32 XELF::getType()
{
    qint32 nResult = TYPE_UNKNOWN;

    quint16 nType = 0;

    if (is64()) {
        nType = getHdr64_type();
    } else {
        nType = getHdr32_type();
    }

    if (nType == XELF_DEF::S_ET_REL) {
        nResult = TYPE_REL;
    } else if (nType == XELF_DEF::S_ET_EXEC) {
        nResult = TYPE_EXEC;
    } else if (nType == XELF_DEF::S_ET_DYN) {
        nResult = TYPE_DYN;
    } else if (nType == XELF_DEF::S_ET_CORE) {
        nResult = TYPE_CORE;
    } else if (nType == XELF_DEF::S_ET_NUM) {
        nResult = TYPE_NUM;
    }

    return nResult;
}

XBinary::FT XELF::getFileType()
{
    FT result = FT_ELF32;

    MODE mode = getMode();

    if (mode == MODE_32) {
        result = FT_ELF32;
    } else if (mode == MODE_64) {
        result = FT_ELF64;
    }

    return result;
}

XBinary::OSINFO XELF::getOsInfo()
{
    OSINFO result = {};

    result.osName = OSNAME_UNIX;

    quint8 osabi = getIdent_osabi();

    if (osabi == XELF_DEF::S_ELFOSABI_HPUX) result.osName = OSNAME_HPUX;
    else if (osabi == XELF_DEF::S_ELFOSABI_NETBSD) result.osName = OSNAME_NETBSD;
    else if (osabi == XELF_DEF::S_ELFOSABI_LINUX) result.osName = OSNAME_LINUX;
    else if (osabi == XELF_DEF::S_ELFOSABI_SOLARIS) result.osName = OSNAME_SOLARIS;
    else if (osabi == XELF_DEF::S_ELFOSABI_AIX) result.osName = OSNAME_AIX;
    else if (osabi == XELF_DEF::S_ELFOSABI_IRIX) result.osName = OSNAME_IRIX;
    else if (osabi == XELF_DEF::S_ELFOSABI_FREEBSD) result.osName = OSNAME_FREEBSD;
    else if (osabi == XELF_DEF::S_ELFOSABI_TRU64) result.osName = OSNAME_TRU64;
    else if (osabi == XELF_DEF::S_ELFOSABI_MODESTO) result.osName = OSNAME_MODESTO;
    else if (osabi == XELF_DEF::S_ELFOSABI_OPENBSD) result.osName = OSNAME_OPENBSD;
    else if (osabi == XELF_DEF::S_ELFOSABI_OPENVMS) result.osName = OSNAME_OPENVMS;
    else if (osabi == XELF_DEF::S_ELFOSABI_NSK) result.osName = OSNAME_NSK;
    else if (osabi == XELF_DEF::S_ELFOSABI_AROS) result.osName = OSNAME_AROS;
    else if (osabi == XELF_DEF::S_ELFOSABI_FENIXOS) result.osName = OSNAME_FENIXOS;
    else if (osabi == XELF_DEF::S_ELFOSABI_OPENVOS) result.osName = OSNAME_OPENVOS;

    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(100);
    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    qint32 nStringTableSection = getSectionStringTable();
    QByteArray baStringTable = getSection(nStringTableSection);

    QList<SECTION_RECORD> listSectionRecords = XELF::getSectionRecords(&listSectionHeaders, isImage(), &baStringTable);

    QList<QString> listComments = getCommentStrings(&listSectionRecords);

    QList<NOTE> listNotes = getNotes(&listProgramHeaders);

    if (listNotes.count() == 0) {
        listNotes = getNotes(&listSectionHeaders);
    }

    QString sInterpteter = getProgramInterpreterName(&listProgramHeaders).sString;

    if (sInterpteter == "") {
        sInterpteter = getProgramInterpreterName(&listSectionRecords).sString;
    }

    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listTagStructs = getTagStructs(&listProgramHeaders, &memoryMap);

    QList<QString> listLibraries = getLibraries(&memoryMap, &listTagStructs);

    if (result.osName == OSNAME_UNIX) {
        if (sInterpteter.contains("ld-elf.so")) {
            result.osName = OSNAME_FREEBSD;
        }
    }

    if (result.osName == OSNAME_UNIX) {
        if (sInterpteter.contains("linux")) {
            result.osName = OSNAME_LINUX;
        }
    }

    if (result.osName == OSNAME_UNIX) {
        if (sInterpteter.contains("ldqnx")) {
            result.osName = OSNAME_QNX;
        }
    }

    if (result.osName == OSNAME_UNIX) {
        if (sInterpteter.contains("uClibc")) {
            result.osName = OSNAME_MCLINUX;
        }
    }

    if ((result.osName == OSNAME_UNIX) || (result.osName == OSNAME_LINUX)) {
        qint32 nNumberOfComments = listComments.count();

        for (qint32 i = 0; i < nNumberOfComments; i++) {
            bool bFound = false;

            QString sComment = listComments.at(i);

            if (sComment.contains("Ubuntu") || sComment.contains("ubuntu")) {
                result.osName = OSNAME_UBUNTULINUX;

                if (sComment.contains("ubuntu1~")) {
                    result.sOsVersion = sComment.section("ubuntu1~", 1, -1).section(")", 0, 0);
                }

                bFound = true;
            } else if (sComment.contains("Debian") || sComment.contains("debian")) {
                result.osName = OSNAME_DEBIANLINUX;

                bFound = true;
            } else if (sComment.contains("StartOS")) {
                result.osName = OSNAME_STARTOSLINUX;

                bFound = true;
            } else if (sComment.contains("Gentoo")) {
                result.osName = OSNAME_GENTOOLINUX;

                bFound = true;
            } else if (sComment.contains("Alpine")) {
                result.osName = OSNAME_ALPINELINUX;

                bFound = true;
            } else if (sComment.contains("Wind River Linux")) {
                result.osName = OSNAME_WINDRIVERLINUX;

                bFound = true;
            } else if (sComment.contains("SuSE") || sComment.contains("SUSE Linux")) {
                result.osName = OSNAME_SUSELINUX;

                bFound = true;
            } else if (sComment.contains("Mandrakelinux") || sComment.contains("Linux-Mandrake") || sComment.contains("Mandrake Linux")) {
                result.osName = OSNAME_MANDRAKELINUX;

                bFound = true;
            } else if (sComment.contains("ASPLinux")) {
                result.osName = OSNAME_ASPLINUX;

                bFound = true;
            } else if (sComment.contains("Red Hat")) {
                result.osName = OSNAME_REDHATLINUX;

                bFound = true;
            } else if (sComment.contains("Hancom Linux")) {
                result.osName = OSNAME_HANCOMLINUX;

                bFound = true;
            } else if (sComment.contains("TurboLinux")) {
                result.osName = OSNAME_TURBOLINUX;

                bFound = true;
            } else if (sComment.contains("Vine Linux")) {
                result.osName = OSNAME_VINELINUX;

                bFound = true;
            }

            if (result.osName != OSNAME_LINUX) {
                if (sComment.contains("SunOS")) {
                    result.osName = OSNAME_SUNOS;

                    if (sComment.contains("@(#)SunOS ")) {
                        result.sOsVersion = sComment.section("@(#)SunOS ", 1, -1);
                    }

                    bFound = true;
                }
            }

            if (bFound) {
                break;
            }
        }
    }

    if (result.osName == OSNAME_FREEBSD) {
        qint32 nNumberOfComments = listComments.count();

        for (qint32 i = 0; i < nNumberOfComments; i++) {
            bool bFound = false;

            QString sComment = listComments.at(i);

            if (sComment.contains("FreeBSD: release/")) {
                result.sOsVersion = sComment.section("FreeBSD: release/", 1, -1).section("/", 0, 0);
            }

            if (bFound) {
                break;
            }
        }
    }

    if (result.osName == OSNAME_UNIX) {
        if (isNotePresent(&listNotes, "Android")) {
            result.osName = OSNAME_ANDROID;
            NOTE note = getNote(&listNotes, "Android");

            if (note.nSize >= 4) {
                quint32 nSDKVersion = read_uint32(note.nDataOffset);
                result.sOsVersion = XBinary::getAndroidVersionFromApi(nSDKVersion);
            }
        } else if ((XBinary::isStringInListPresent(&listLibraries, "liblog.so")) || ((sInterpteter == "system/bin/linker") || (sInterpteter == "system/bin/linker64"))) {
            result.osName = OSNAME_ANDROID;
        }
    }

    if (isNotePresent(&listNotes, 1, "GNU")) {
        NOTE note = getNote(&listNotes, 1, "GNU");

        quint32 nOS = read_uint32(note.nDataOffset);
        quint32 nMajor = read_uint32(note.nDataOffset + 4);
        quint32 nMinor = read_uint32(note.nDataOffset + 8);
        quint32 nSubMinor = read_uint32(note.nDataOffset + 12);

        if (result.osName == OSNAME_UNIX) {
            if (nOS == 0) result.osName = OSNAME_LINUX;
            //            else if (nOS==1)  result.osName=OSNAME_GNU;
            else if (nOS == 2) result.osName = OSNAME_SOLARIS;
            else if (nOS == 3) result.osName = OSNAME_FREEBSD;
            else if (nOS == 4) result.osName = OSNAME_NETBSD;
            else if (nOS == 5) result.osName = OSNAME_SYLLABLE;
        }

        QString sABI = QString("ABI: %1.%2.%3").arg(QString::number(nMajor), QString::number(nMinor), QString::number(nSubMinor));

        result.sOsVersion = appendText(result.sOsVersion, sABI, ",");
    }

    if (result.osName == OSNAME_UNIX) {
        if (isSectionNamePresent(".note.android.ident", &listSectionRecords)) result.osName = OSNAME_ANDROID;
        else if (isSectionNamePresent(".note.minix.ident", &listSectionRecords)) result.osName = OSNAME_MINIX;
        else if (isSectionNamePresent(".note.netbsd.ident", &listSectionRecords)) result.osName = OSNAME_NETBSD;
        else if (isSectionNamePresent(".note.openbsd.ident", &listSectionRecords)) result.osName = OSNAME_OPENBSD;
    }

    if (result.osName == OSNAME_UNIX) {
        result.sOsVersion = QString("%1").arg(osabi);
    }

    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    return result;
}

QString XELF::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_REL: sResult = QString("REL"); break;
        case TYPE_EXEC: sResult = QString("EXEC"); break;
        case TYPE_DYN: sResult = QString("DYN"); break;
        case TYPE_CORE: sResult = QString("CORE"); break;
        case TYPE_NUM: sResult = QString("NUM"); break;
    }

    return sResult;
}

QList<XELF_DEF::Elf_Phdr> XELF::_getPrograms(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders, quint32 nType)
{
    QList<XELF_DEF::Elf_Phdr> listResult;

    qint32 nNumberOfPrograms = pListProgramHeaders->count();

    for (qint32 i = 0; i < nNumberOfPrograms; i++) {
        if (pListProgramHeaders->at(i).p_type == nType) {
            listResult.append(pListProgramHeaders->at(i));
        }
    }

    return listResult;
}

QList<XELF_DEF::Elf_Shdr> XELF::_getSections(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders, quint32 nType)
{
    QList<XELF_DEF::Elf_Shdr> listResult;

    qint32 nNumberOfSections = pListSectionHeaders->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionHeaders->at(i).sh_type == nType) {
            listResult.append(pListSectionHeaders->at(i));
        }
    }

    return listResult;
}

QList<XELF::SECTION_RECORD> XELF::_getSectionRecords(QList<SECTION_RECORD> *pListSectionRecords, const QString &sName)
{
    QList<SECTION_RECORD> listResult;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sName) {
            listResult.append(pListSectionRecords->at(i));
        }
    }

    return listResult;
}

QList<XBinary::DATASET> XELF::getDatasetsFromSections(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    QList<XBinary::DATASET> listResult;

    quint32 nMainStringSection = getSectionStringTable();

    XBinary::OFFSETSIZE osStringTable = getSectionOffsetSize(nMainStringSection);

    qint32 nNumberOfSections = pListSectionHeaders->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        QString sSectionName = getStringFromIndex(osStringTable.nOffset, osStringTable.nSize, pListSectionHeaders->at(i).sh_name);

        if ((pListSectionHeaders->at(i).sh_type == 1) && (sSectionName == ".interp"))  // Interpreter TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_INTERPRETER;
            dataset.sName = QString("%1[%2]").arg("Interpreter", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 2)  // Symbol table TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_SYMBOLTABLE;
            dataset.sName = QString("%1[%2]").arg("Symbol table", sSectionName);  // TODO mb translate

            qint32 nSectionIndex = getSectionIndexByName(".strtab");

            if (nSectionIndex != -1) {
                XBinary::OFFSETSIZE osStringTable = getSectionOffsetSize(nSectionIndex);

                dataset.nStringTableOffset = osStringTable.nOffset;
                dataset.nStringTableSize = osStringTable.nSize;
            }

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 0xb)  // Symbol table dynamic TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_SYMBOLTABLE;
            dataset.sName = QString("%1[%2]").arg("Symbol table", sSectionName);  // TODO mb translate

            qint32 nSectionIndex = getSectionIndexByName(".dynstr");

            if (nSectionIndex != -1) {
                XBinary::OFFSETSIZE osStringTable = getSectionOffsetSize(nSectionIndex);

                dataset.nStringTableOffset = osStringTable.nOffset;
                dataset.nStringTableSize = osStringTable.nSize;
            }

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 3)  // String table TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_STRINGTABLE;
            dataset.sName = QString("%1[%2]").arg("String table", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 4)  // RELA TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_RELA;
            dataset.sName = QString("%1[%2]").arg("RELA", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 6)  // Dynamic TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_DYNAMICTAGS;
            dataset.sName = QString("%1[%2]").arg("Dynamic tags", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 7)  // Notes TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_NOTES;
            dataset.sName = QString("%1[%2]").arg("Notes", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 9)  // REL TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_REL;
            dataset.sName = QString("%1[%2]").arg("REL", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        }
    }

    return listResult;
}

QList<XBinary::DATASET> XELF::getDatasetsFromPrograms(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    QList<XBinary::DATASET> listResult;

    qint32 nNumberOfPrograms = pListProgramHeaders->count();

    for (qint32 i = 0; i < nNumberOfPrograms; i++) {
        if ((pListProgramHeaders->at(i).p_type == 3))  // Interpreter TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListProgramHeaders->at(i).p_vaddr;
            dataset.nOffset = pListProgramHeaders->at(i).p_offset;
            dataset.nSize = pListProgramHeaders->at(i).p_filesz;
            dataset.nType = DS_INTERPRETER;
            dataset.sName = QString("%1").arg("Interpreter");  // TODO mb translate

            listResult.append(dataset);
        } else if ((pListProgramHeaders->at(i).p_type == 4))  // Notes TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListProgramHeaders->at(i).p_vaddr;
            dataset.nOffset = pListProgramHeaders->at(i).p_offset;
            dataset.nSize = pListProgramHeaders->at(i).p_filesz;
            dataset.nType = DS_NOTES;
            dataset.sName = QString("%1").arg("Notes");  // TODO mb translate

            listResult.append(dataset);
        } else if ((pListProgramHeaders->at(i).p_type == 2))  // Tags TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListProgramHeaders->at(i).p_vaddr;
            dataset.nOffset = pListProgramHeaders->at(i).p_offset;
            dataset.nSize = pListProgramHeaders->at(i).p_filesz;
            dataset.nType = DS_DYNAMICTAGS;
            dataset.sName = QString("%1").arg("Dynamic tags");  // TODO mb translate

            listResult.append(dataset);
        }
    }

    return listResult;
}

QList<XBinary::DATASET> XELF::getDatasetsFromTagStructs(XBinary::_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pListTagStructs)
{
    QList<XBinary::DATASET> listResult;

    QList<TAG_STRUCT> listStrTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRTAB);
    QList<TAG_STRUCT> listStrSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRSZ);
    QList<TAG_STRUCT> listStrNeeded = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_NEEDED);
    QList<TAG_STRUCT> listRunPath = _getTagStructs(pListTagStructs, 0x1d);  // TODO const
    QList<TAG_STRUCT> listSymbols = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_SYMTAB);
    QList<TAG_STRUCT> listRelaTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_RELA);
    QList<TAG_STRUCT> listRelaSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_RELASZ);
    QList<TAG_STRUCT> listRelTab = _getTagStructs(pListTagStructs, 17);   // TODO const
    QList<TAG_STRUCT> listRelSize = _getTagStructs(pListTagStructs, 18);  // TODO const

    qint64 nStringTableOffset = 0;
    qint64 nStringTableSize = 0;

    if (listStrTab.count() && listStrSize.count()) {
        DATASET dataset = {};

        dataset.nAddress = listStrTab.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = listStrSize.at(0).nValue;
        dataset.nType = DS_STRINGTABLE;
        dataset.sName = "String table";  // TODO mb translate

        if (isOffsetAndSizeValid(pMemoryMap, dataset.nOffset, dataset.nSize)) {
            listResult.append(dataset);
        }

        nStringTableOffset = dataset.nOffset;
        nStringTableSize = dataset.nSize;
    }

    if (listRelaTab.count() && listRelaSize.count()) {
        DATASET dataset = {};

        dataset.nAddress = listRelaTab.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = listRelaSize.at(0).nValue;
        dataset.nType = DS_RELA;
        dataset.sName = "RELA";  // TODO mb translate
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    if (listRelTab.count() && listRelSize.count()) {
        DATASET dataset = {};

        dataset.nAddress = listRelTab.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = listRelSize.at(0).nValue;
        dataset.nType = DS_REL;
        dataset.sName = "REL";  // TODO mb translate
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    if (listStrNeeded.count()) {
        DATASET dataset = {};

        dataset.nAddress = 0;
        dataset.nOffset = 0;
        dataset.nSize = 0;
        dataset.nType = DS_LIBRARIES;
        dataset.sName = "Libraries";  // TODO mb translate
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    if (listRunPath.count()) {
        XADDR nAddress = listStrTab.at(0).nValue;
        qint64 nOffset = addressToOffset(pMemoryMap, nAddress);
        qint64 nRunPath = listRunPath.at(0).nValue;
        qint64 nSize = listStrSize.at(0).nValue;

        if (nRunPath < nSize) {
            DATASET dataset = {};

            dataset.nAddress = nAddress + nRunPath;
            dataset.nOffset = nOffset + nRunPath;
            QString sAnsiString = read_ansiString(dataset.nOffset);
            dataset.nSize = sAnsiString.length();
            dataset.nType = DS_RUNPATH;
            dataset.sName = "Run path";  // TODO mb translate
            dataset.nStringTableOffset = nStringTableOffset;
            dataset.nStringTableSize = nStringTableSize;

            listResult.append(dataset);
        }
    }

    if (listSymbols.count()) {
        DATASET dataset = {};

        dataset.nAddress = listSymbols.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = getSymTableSize(dataset.nOffset);
        dataset.nType = DS_SYMBOLTABLE;
        dataset.sName = "Symbol table";  // TODO mb translate
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    return listResult;
}

QList<XELF_DEF::Elf32_Sym> XELF::getElf32_SymList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf32_Sym> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf32_Sym record = _readElf32_Sym(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Sym);
        nSize -= sizeof(XELF_DEF::Elf32_Sym);
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Sym> XELF::getElf64_SymList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf64_Sym> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf64_Sym record = _readElf64_Sym(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Sym);
        nSize -= sizeof(XELF_DEF::Elf64_Sym);
    }

    return listResult;
}

QList<XELF_DEF::Elf_Sym> XELF::getElf_SymList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf_Sym> listResult;

    bool bIsBigEndian = isBigEndian();
    bool bIs64 = is64();

    while (nSize > 0) {
        XELF_DEF::Elf_Sym record = {};

        if (bIs64) {
            XELF_DEF::Elf64_Sym _record = _readElf64_Sym(nOffset, bIsBigEndian);

            record.st_name = _record.st_name;
            record.st_info = _record.st_info;
            record.st_other = _record.st_other;
            record.st_shndx = _record.st_shndx;
            record.st_value = _record.st_value;
            record.st_size = _record.st_size;

            nOffset += sizeof(XELF_DEF::Elf64_Sym);
            nSize -= sizeof(XELF_DEF::Elf64_Sym);
        } else {
            XELF_DEF::Elf32_Sym _record = _readElf32_Sym(nOffset, bIsBigEndian);

            record.st_name = _record.st_name;
            record.st_info = _record.st_info;
            record.st_other = _record.st_other;
            record.st_shndx = _record.st_shndx;
            record.st_value = _record.st_value;
            record.st_size = _record.st_size;

            nOffset += sizeof(XELF_DEF::Elf32_Sym);
            nSize -= sizeof(XELF_DEF::Elf32_Sym);
        }

        listResult.append(record);
    }

    return listResult;
}

XELF_DEF::Elf32_Sym XELF::_readElf32_Sym(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Sym result = {};

    result.st_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_name), bIsBigEndian);
    result.st_value = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_value), bIsBigEndian);
    result.st_size = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_size), bIsBigEndian);
    result.st_info = read_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_info));
    result.st_other = read_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_other));
    result.st_shndx = read_uint16(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_shndx), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Sym XELF::_readElf64_Sym(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Sym result = {};

    result.st_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_name), bIsBigEndian);
    result.st_info = read_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_info));
    result.st_other = read_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_other));
    result.st_shndx = read_uint16(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_shndx), bIsBigEndian);
    result.st_value = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_value), bIsBigEndian);
    result.st_size = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_size), bIsBigEndian);

    return result;
}

void XELF::setElf32_Sym_st_name(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_name), nValue, bIsBigEndian);
}

void XELF::setElf32_Sym_st_value(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_value), nValue, bIsBigEndian);
}

void XELF::setElf32_Sym_st_size(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_size), nValue, bIsBigEndian);
}

void XELF::setElf32_Sym_st_info(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_info), nValue);
}

void XELF::setElf32_Sym_st_other(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_other), nValue);
}

void XELF::setElf32_Sym_st_shndx(qint64 nOffset, quint16 nValue, bool bIsBigEndian)
{
    write_uint16(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_shndx), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_name(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_name), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_info(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_info), nValue);
}

void XELF::setElf64_Sym_st_other(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_other), nValue);
}

void XELF::setElf64_Sym_st_shndx(qint64 nOffset, quint16 nValue, bool bIsBigEndian)
{
    write_uint16(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_shndx), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_value(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_value), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_size(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_size), nValue, bIsBigEndian);
}

qint64 XELF::getSymSize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XELF_DEF::Elf64_Sym);
    } else {
        nResult = sizeof(XELF_DEF::Elf32_Sym);
    }

    return nResult;
}

qint64 XELF::getSymTableSize(qint64 nOffset)
{
    qint64 nResult = 0;

    bool bIsBigEndian = isBigEndian();
    bool bIs64 = is64();

    if (bIs64) {
        nResult += sizeof(XELF_DEF::Elf64_Sym);
        nOffset += sizeof(XELF_DEF::Elf64_Sym);

        while (true) {
            XELF_DEF::Elf64_Sym record = _readElf64_Sym(nOffset, bIsBigEndian);

            if ((!record.st_info) || (record.st_other)) {
                break;
            }

            nResult += sizeof(XELF_DEF::Elf64_Sym);
            nOffset += sizeof(XELF_DEF::Elf64_Sym);
        }
    } else {
        nResult += sizeof(XELF_DEF::Elf32_Sym);
        nOffset += sizeof(XELF_DEF::Elf32_Sym);

        while (true) {
            XELF_DEF::Elf32_Sym record = _readElf32_Sym(nOffset, bIsBigEndian);

            if ((!record.st_info) || (record.st_other)) {
                break;
            }

            nResult += sizeof(XELF_DEF::Elf32_Sym);
            nOffset += sizeof(XELF_DEF::Elf32_Sym);
        }
    }

    return nResult;
}

XELF_DEF::Elf32_Rel XELF::_readElf32_Rel(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Rel result = {};

    result.r_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_offset), bIsBigEndian);
    result.r_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_info), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Rel XELF::_readElf64_Rel(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Rel result = {};

    result.r_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_offset), bIsBigEndian);
    result.r_info = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_info), bIsBigEndian);

    return result;
}

XELF_DEF::Elf32_Rela XELF::_readElf32_Rela(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Rela result = {};

    result.r_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_offset), bIsBigEndian);
    result.r_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_info), bIsBigEndian);
    result.r_addend = read_int32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_addend), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Rela XELF::_readElf64_Rela(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Rela result = {};

    result.r_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_offset), bIsBigEndian);
    result.r_info = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_info), bIsBigEndian);
    result.r_addend = read_int64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_addend), bIsBigEndian);

    return result;
}

QList<XELF_DEF::Elf32_Rel> XELF::getElf32_RelList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf32_Rel> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf32_Rel record = _readElf32_Rel(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Rel);
        nSize -= sizeof(XELF_DEF::Elf32_Rel);
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Rel> XELF::getElf64_RelList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf64_Rel> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf64_Rel record = _readElf64_Rel(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Rel);
        nSize -= sizeof(XELF_DEF::Elf64_Rel);
    }

    return listResult;
}

QList<XELF_DEF::Elf32_Rela> XELF::getElf32_RelaList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf32_Rela> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf32_Rela record = _readElf32_Rela(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Rela);
        nSize -= sizeof(XELF_DEF::Elf32_Rela);
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Rela> XELF::getElf64_RelaList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf64_Rela> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf64_Rela record = _readElf64_Rela(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Rela);
        nSize -= sizeof(XELF_DEF::Elf64_Rela);
    }

    return listResult;
}

void XELF::setElf32_Rel_r_offset(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf32_Rel_r_info(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_info), nValue, bIsBigEndian);
}

void XELF::setElf64_Rel_r_offset(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf64_Rel_r_info(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_info), nValue, bIsBigEndian);
}

void XELF::setElf32_Rela_r_offset(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf32_Rela_r_info(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_info), nValue, bIsBigEndian);
}

void XELF::setElf32_Rela_r_addend(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_addend), nValue, bIsBigEndian);
}

void XELF::setElf64_Rela_r_offset(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf64_Rela_r_info(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_info), nValue, bIsBigEndian);
}

void XELF::setElf64_Rela_r_addend(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_addend), nValue, bIsBigEndian);
}

quint32 XELF::getNumberOfSections()
{
    quint32 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = getHdr64_shnum();
    } else {
        nResult = getHdr32_shnum();
    }

    if (nResult == 0) {
        quint64 nOffset = 0;

        if (bIs64) {
            nOffset = getHdr64_shoff();
        } else {
            nOffset = getHdr32_shoff();
        }

        if (nOffset) {
            bool bIsBigEndian = isBigEndian();

            if (bIs64) {
                XELF_DEF::Elf64_Shdr record = _readElf64_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_size;
            } else {
                XELF_DEF::Elf32_Shdr record = _readElf32_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_size;
            }
        }
    }

    return nResult;
}

quint16 XELF::getNumberOfPrograms()
{
    quint16 nResult = 0;

    if (is64()) {
        nResult = getHdr64_phnum();
    } else {
        nResult = getHdr32_phnum();
    }

    return nResult;
}

bool XELF::isSectionsTablePresent()
{
    return (bool)getNumberOfSections();
}

bool XELF::isProgramsTablePresent()
{
    return (bool)getNumberOfPrograms();
}

QString XELF::getFileFormatString()
{
    QString sResult;

    sResult = QString("ELF(%1)").arg(getArch());

    return sResult;
}

QString XELF::getFileFormatExt()
{
    QString sResult;

    TYPE _type = (TYPE)getType();

    if (_type == TYPE_DYN) {
        sResult = "so";
    } else {
        sResult = "elf";
    }

    return sResult;
}

qint64 XELF::getFileFormatSize()
{
    return _calculateRawSize();
}

QList<XBinary::HREGION> XELF::getHighlights(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pMemoryMap)
    Q_UNUSED(pPdStruct)

    QList<XBinary::HREGION> listResult;

    return listResult;
}

bool XELF::fixDump(const QString &sResultFile, const FIXDUMP_OPTIONS &fixDumpOptions, PDSTRUCT *pPdStruct)
{
    bool bResult = true;

    QFile file;
    file.setFileName(sResultFile);

    if (fixDumpOptions.bOptimizeSize) {
        if (file.open(QIODevice::ReadWrite)) {
            QByteArray baHeaders = getHeaders();

            file.write(baHeaders);

            // TODO

            file.close();
        }
    } else {
        QString sSrcFile = getDeviceFileName(getDevice());
        XBinary::copyFile(sSrcFile, sResultFile);
    }

    if (fixDumpOptions.bSetEntryPoint) {
        if (file.open(QIODevice::ReadWrite)) {
            XELF elf(&file);

            if (elf.isValid(pPdStruct)) {
                if ((fixDumpOptions.bFixSegments) && (!(fixDumpOptions.bOptimizeSize))) {
                    if (elf.is64()) {
                        qint32 nNumberOfSegments = getHdr64_phnum();
                        for (qint32 i = 0; i < nNumberOfSegments; i++) {
                            // TODO
                        }
                    } else {
                        qint32 nNumberOfSegments = getHdr32_phnum();
                        for (qint32 i = 0; i < nNumberOfSegments; i++) {
                            // TODO
                        }
                    }
                }

                if (fixDumpOptions.bSetEntryPoint) {
                    if (elf.is64()) {
                        elf.setHdr64_entry((quint64)fixDumpOptions.nEntryPoint);
                    } else {
                        elf.setHdr32_entry((quint32)fixDumpOptions.nEntryPoint);
                    }
                }
            }

            file.close();
        }
    }

    return bResult;
}

XELF::FIXDUMP_OPTIONS XELF::getFixDumpOptions(PDSTRUCT *pPdStruct)
{
    XELF::FIXDUMP_OPTIONS result = {};

    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    result.bOptimizeSize = true;
    result.bFixSegments = true;
    result.bSetEntryPoint = false;
    result.nEntryPoint = getEntryPointAddress(&memoryMap);

    return result;
}

QByteArray XELF::getHeaders()
{
    qint64 nSize = 0;

    if (is64()) {
        nSize = getHdr64_phoff() + getHdr64_phentsize() * getHdr64_phnum();
    } else {
        nSize = getHdr32_phoff() + getHdr32_phentsize() * getHdr32_phnum();
    }

    return read_array(0, nSize);
}

QMap<quint64, QString> XELF::getRelTypes_x86()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "R_386_NONE");
    mapResult.insert(1, "R_386_32");
    mapResult.insert(2, "R_386_PC32");
    mapResult.insert(3, "R_386_GOT32");
    mapResult.insert(4, "R_386_PLT32");
    mapResult.insert(5, "R_386_COPY");
    mapResult.insert(6, "R_386_GLOB_DAT");
    mapResult.insert(7, "R_386_JMP_SLOT");
    mapResult.insert(8, "R_386_RELATIVE");
    mapResult.insert(9, "R_386_GOTOFF");
    mapResult.insert(10, "R_386_GOTPC");
    mapResult.insert(11, "R_386_32PLT");
    mapResult.insert(20, "R_386_16");
    mapResult.insert(21, "R_386_PC16");
    mapResult.insert(22, "R_386_8");
    mapResult.insert(23, "R_386_PC8");
    mapResult.insert(38, "R_386_SIZE32");

    return mapResult;
}

QMap<quint64, QString> XELF::getRelTypesS_x86()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "386_NONE");
    mapResult.insert(1, "386_32");
    mapResult.insert(2, "386_PC32");
    mapResult.insert(3, "386_GOT32");
    mapResult.insert(4, "386_PLT32");
    mapResult.insert(5, "386_COPY");
    mapResult.insert(6, "386_GLOB_DAT");
    mapResult.insert(7, "386_JMP_SLOT");
    mapResult.insert(8, "386_RELATIVE");
    mapResult.insert(9, "386_GOTOFF");
    mapResult.insert(10, "386_GOTPC");
    mapResult.insert(11, "386_32PLT");
    mapResult.insert(20, "386_16");
    mapResult.insert(21, "386_PC16");
    mapResult.insert(22, "386_8");
    mapResult.insert(23, "386_PC8");
    mapResult.insert(38, "386_SIZE32");

    return mapResult;
}

QMap<quint64, QString> XELF::getRelTypes_x64()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "R_X86_64_NONE");
    mapResult.insert(1, "R_X86_64_64");
    mapResult.insert(2, "R_386_PC32");
    mapResult.insert(3, "R_386_GOT32");
    mapResult.insert(4, "R_X86_64_PLT32");
    mapResult.insert(5, "R_X86_64_COPY");
    mapResult.insert(6, "R_X86_64_GLOB_DAT");
    mapResult.insert(7, "R_X86_64_JMP_SLOT");
    mapResult.insert(8, "R_X86_64_RELATIVE");
    mapResult.insert(9, "R_X86_64_GOTPCREL");
    mapResult.insert(10, "R_X86_64_32");
    mapResult.insert(11, "R_X86_64_32S");
    mapResult.insert(12, "R_X86_64_16");
    mapResult.insert(13, "R_X86_64_PC16");
    mapResult.insert(14, "R_X86_64_8");
    mapResult.insert(15, "R_X86_64_PC8");
    mapResult.insert(24, "R_X86_64_PC64");

    return mapResult;
}

QMap<quint64, QString> XELF::getRelTypesS_x64()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "X86_64_NONE");
    mapResult.insert(1, "X86_64_64");
    mapResult.insert(2, "386_PC32");
    mapResult.insert(3, "386_GOT32");
    mapResult.insert(4, "X86_64_PLT32");
    mapResult.insert(5, "X86_64_COPY");
    mapResult.insert(6, "X86_64_GLOB_DAT");
    mapResult.insert(7, "X86_64_JMP_SLOT");
    mapResult.insert(8, "X86_64_RELATIVE");
    mapResult.insert(9, "X86_64_GOTPCREL");
    mapResult.insert(10, "X86_64_32");
    mapResult.insert(11, "X86_64_32S");
    mapResult.insert(12, "X86_64_16");
    mapResult.insert(13, "X86_64_PC16");
    mapResult.insert(14, "X86_64_8");
    mapResult.insert(15, "X86_64_PC8");
    mapResult.insert(24, "X86_64_PC64");

    return mapResult;
}

QMap<quint64, QString> XELF::getRelTypes_SPARC()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "R_SPARC_NONE");
    mapResult.insert(1, "R_SPARC_8");
    mapResult.insert(2, "R_SPARC_16");
    mapResult.insert(3, "R_SPARC_32");
    mapResult.insert(4, "R_SPARC_DISP8");
    mapResult.insert(5, "R_SPARC_DISP16");
    mapResult.insert(6, "R_SPARC_DISP32");
    mapResult.insert(7, "R_SPARC_WDISP30");
    mapResult.insert(8, "R_SPARC_WDISP22");
    mapResult.insert(9, "R_SPARC_HI22");
    mapResult.insert(10, "R_SPARC_22");
    mapResult.insert(11, "R_SPARC_13");
    mapResult.insert(12, "R_SPARC_LO10");
    mapResult.insert(13, "R_SPARC_GOT10");
    mapResult.insert(14, "R_SPARC_GOT13");
    mapResult.insert(15, "R_SPARC_GOT22");
    mapResult.insert(16, "R_SPARC_PC10");
    mapResult.insert(17, "R_SPARC_PC22");
    mapResult.insert(18, "R_SPARC_WPLT30");
    mapResult.insert(19, "R_SPARC_COPY");
    mapResult.insert(20, "R_SPARC_GLOB_DAT");
    mapResult.insert(21, "R_SPARC_JMP_SLOT");
    mapResult.insert(22, "R_SPARC_RELATIVE");
    mapResult.insert(23, "R_SPARC_UA32");
    mapResult.insert(24, "R_SPARC_PLT32");
    mapResult.insert(25, "R_SPARC_HIPLT22");
    mapResult.insert(26, "R_SPARC_LOPLT10");
    mapResult.insert(27, "R_SPARC_PCPLT32");
    mapResult.insert(28, "R_SPARC_PCPLT22");
    mapResult.insert(29, "R_SPARC_PCPLT10");
    mapResult.insert(30, "R_SPARC_10");
    mapResult.insert(31, "R_SPARC_11");
    mapResult.insert(32, "R_SPARC_64");
    mapResult.insert(33, "R_SPARC_OLO10");
    mapResult.insert(34, "R_SPARC_HH22");
    mapResult.insert(35, "R_SPARC_HM10");
    mapResult.insert(36, "R_SPARC_LM22");
    mapResult.insert(37, "R_SPARC_PC_HH22");
    mapResult.insert(38, "R_SPARC_PC_HM10");
    mapResult.insert(39, "R_SPARC_PC_LM22");
    mapResult.insert(40, "R_SPARC_WDISP16");
    mapResult.insert(41, "R_SPARC_WDISP19");
    mapResult.insert(43, "R_SPARC_7");
    mapResult.insert(44, "R_SPARC_5");
    mapResult.insert(45, "R_SPARC_6");
    mapResult.insert(46, "R_SPARC_DISP64");
    mapResult.insert(47, "R_SPARC_PLT64");
    mapResult.insert(48, "R_SPARC_HIX22");
    mapResult.insert(49, "R_SPARC_LOX10");
    mapResult.insert(50, "R_SPARC_H44");
    mapResult.insert(51, "R_SPARC_M44");
    mapResult.insert(52, "R_SPARC_L44");
    mapResult.insert(53, "R_SPARC_REGISTER");
    mapResult.insert(54, "R_SPARC_UA64");
    mapResult.insert(55, "R_SPARC_UA16");
    mapResult.insert(80, "R_SPARC_GOTDATA_HIX22");
    mapResult.insert(81, "R_SPARC_GOTDATA_LOX10");
    mapResult.insert(82, "R_SPARC_GOTDATA_OP_HIX22");
    mapResult.insert(83, "R_SPARC_GOTDATA_OP_LOX10");
    mapResult.insert(84, "R_SPARC_GOTDATA_OP");
    mapResult.insert(85,
                     "R_SPARC_SIZE32");  // mb TODO
                                         // https://docs.oracle.com/cd/E19120-01/open.solaris/819-0690/6n33n7fct/index.html
    mapResult.insert(87, "R_SPARC_SIZE64");

    return mapResult;
}

QMap<quint64, QString> XELF::getRelTypesS_SPARC()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "SPARC_NONE");
    mapResult.insert(1, "SPARC_8");
    mapResult.insert(2, "SPARC_16");
    mapResult.insert(3, "SPARC_32");
    mapResult.insert(4, "SPARC_DISP8");
    mapResult.insert(5, "SPARC_DISP16");
    mapResult.insert(6, "SPARC_DISP32");
    mapResult.insert(7, "SPARC_WDISP30");
    mapResult.insert(8, "SPARC_WDISP22");
    mapResult.insert(9, "SPARC_HI22");
    mapResult.insert(10, "SPARC_22");
    mapResult.insert(11, "SPARC_13");
    mapResult.insert(12, "SPARC_LO10");
    mapResult.insert(13, "SPARC_GOT10");
    mapResult.insert(14, "SPARC_GOT13");
    mapResult.insert(15, "SPARC_GOT22");
    mapResult.insert(16, "SPARC_PC10");
    mapResult.insert(17, "SPARC_PC22");
    mapResult.insert(18, "SPARC_WPLT30");
    mapResult.insert(19, "SPARC_COPY");
    mapResult.insert(20, "SPARC_GLOB_DAT");
    mapResult.insert(21, "SPARC_JMP_SLOT");
    mapResult.insert(22, "SPARC_RELATIVE");
    mapResult.insert(23, "SPARC_UA32");
    mapResult.insert(24, "SPARC_PLT32");
    mapResult.insert(25, "SPARC_HIPLT22");
    mapResult.insert(26, "SPARC_LOPLT10");
    mapResult.insert(27, "SPARC_PCPLT32");
    mapResult.insert(28, "SPARC_PCPLT22");
    mapResult.insert(29, "SPARC_PCPLT10");
    mapResult.insert(30, "SPARC_10");
    mapResult.insert(31, "SPARC_11");
    mapResult.insert(32, "SPARC_64");
    mapResult.insert(33, "SPARC_OLO10");
    mapResult.insert(34, "SPARC_HH22");
    mapResult.insert(35, "SPARC_HM10");
    mapResult.insert(36, "SPARC_LM22");
    mapResult.insert(37, "SPARC_PC_HH22");
    mapResult.insert(38, "SPARC_PC_HM10");
    mapResult.insert(39, "SPARC_PC_LM22");
    mapResult.insert(40, "SPARC_WDISP16");
    mapResult.insert(41, "SPARC_WDISP19");
    mapResult.insert(43, "SPARC_7");
    mapResult.insert(44, "SPARC_5");
    mapResult.insert(45, "SPARC_6");
    mapResult.insert(46, "SPARC_DISP64");
    mapResult.insert(47, "SPARC_PLT64");
    mapResult.insert(48, "SPARC_HIX22");
    mapResult.insert(49, "SPARC_LOX10");
    mapResult.insert(50, "SPARC_H44");
    mapResult.insert(51, "SPARC_M44");
    mapResult.insert(52, "SPARC_L44");
    mapResult.insert(53, "SPARC_REGISTER");
    mapResult.insert(54, "SPARC_UA64");
    mapResult.insert(55, "SPARC_UA16");
    mapResult.insert(80, "SPARC_GOTDATA_HIX22");
    mapResult.insert(81, "SPARC_GOTDATA_LOX10");
    mapResult.insert(82, "SPARC_GOTDATA_OP_HIX22");
    mapResult.insert(83, "SPARC_GOTDATA_OP_LOX10");
    mapResult.insert(84, "SPARC_GOTDATA_OP");
    mapResult.insert(85,
                     "SPARC_SIZE32");  // mb TODO
                                       // https://docs.oracle.com/cd/E19120-01/open.solaris/819-0690/6n33n7fct/index.html
    mapResult.insert(87, "SPARC_SIZE64");

    return mapResult;
}

QMap<quint64, QString> XELF::getStBinds()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "STB_LOCAL");
    mapResult.insert(1, "STB_GLOBAL");
    mapResult.insert(2, "STB_WEAK");
    mapResult.insert(10, "STB_LOOS");
    mapResult.insert(12, "STB_HIOS");
    mapResult.insert(13, "STB_LOPROC");
    mapResult.insert(15, "STB_HIPROC");

    return mapResult;
}

QMap<quint64, QString> XELF::getStBindsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "LOCAL");
    mapResult.insert(1, "GLOBAL");
    mapResult.insert(2, "WEAK");
    mapResult.insert(10, "LOOS");
    mapResult.insert(12, "HIOS");
    mapResult.insert(13, "LOPROC");
    mapResult.insert(15, "HIPROC");

    return mapResult;
}

QMap<quint64, QString> XELF::getStTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "STT_NOTYPE");
    mapResult.insert(1, "STT_OBJECT");
    mapResult.insert(2, "STT_FUNC");
    mapResult.insert(3, "STT_SECTION");
    mapResult.insert(4, "STT_FILE");
    mapResult.insert(5, "STT_COMMON");
    mapResult.insert(10, "STT_LOOS");
    mapResult.insert(12, "STT_HIOS");
    mapResult.insert(13, "STT_LOPROC");  // STT_SPARC_REGISTER
    mapResult.insert(15, "STT_HIPROC");

    return mapResult;
}

QMap<quint64, QString> XELF::getStTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0, "NOTYPE");
    mapResult.insert(1, "OBJECT");
    mapResult.insert(2, "FUNC");
    mapResult.insert(3, "SECTION");
    mapResult.insert(4, "FILE");
    mapResult.insert(5, "COMMON");
    mapResult.insert(10, "LOOS");
    mapResult.insert(12, "HIOS");
    mapResult.insert(13, "LOPROC");  // SPARC_REGISTER
    mapResult.insert(15, "HIPROC");

    return mapResult;
}
