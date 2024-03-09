/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "elf_script.h"

ELF_Script::ELF_Script(XELF *pELF, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : Binary_Script(pELF, pOptions, pPdStruct)
{
    this->g_pELF = pELF;

    bool bIs64 = pELF->is64(getMemoryMap());

    g_elfHeader = pELF->getHdr();

    g_nStringTableSection = pELF->getSectionStringTable(bIs64);
    g_baStringTable = pELF->getSection(g_nStringTableSection);
    g_listSectionHeaders = pELF->getElf_ShdrList(100);
    g_listProgramHeaders = pELF->getElf_PhdrList(100);

    g_listNotes = pELF->getNotes(&g_listProgramHeaders);

    if (g_listNotes.count() == 0) {
        g_listNotes = pELF->getNotes(&g_listSectionHeaders);
    }

    g_listSectionRecords = pELF->getSectionRecords(&g_listSectionHeaders, bIs64, &g_baStringTable);
    g_listTagStruct = pELF->getTagStructs(&g_listProgramHeaders, getMemoryMap());
    g_listLibraryNames = pELF->getLibraries(getMemoryMap(), &g_listTagStruct);

    g_sGeneralOptions = QString("%1 %2-%3")
                            .arg(XELF::getTypesS().value(g_elfHeader.e_type))
                            .arg(XELF::getMachinesS().value(g_elfHeader.e_machine))
                            .arg(bIs64 ? ("64") : ("32"));  // TODO Check
}

ELF_Script::~ELF_Script()
{
}

bool ELF_Script::isSectionNamePresent(const QString &sSectionName)
{
    return g_pELF->isSectionNamePresent(sSectionName, &g_listSectionRecords);  // TODO get pdStruct
}

quint32 ELF_Script::getNumberOfSections()
{
    return g_elfHeader.e_shnum;
}

quint32 ELF_Script::getNumberOfPrograms()
{
    return g_elfHeader.e_phnum;
}

QString ELF_Script::getGeneralOptions()
{
    return g_sGeneralOptions;
}

qint32 ELF_Script::getSectionNumber(const QString &sSectionName)
{
    return g_pELF->getSectionNumber(sSectionName, &g_listSectionRecords);  // TODO get pdStruct
}

quint16 ELF_Script::getElfHeader_type()
{
    return g_elfHeader.e_type;
}

quint16 ELF_Script::getElfHeader_machine()
{
    return g_elfHeader.e_machine;
}

quint32 ELF_Script::getElfHeader_version()
{
    return g_elfHeader.e_version;
}

quint64 ELF_Script::getElfHeader_entry()
{
    return g_elfHeader.e_entry;
}

quint64 ELF_Script::getElfHeader_phoff()
{
    return g_elfHeader.e_phoff;
}

quint64 ELF_Script::getElfHeader_shoff()
{
    return g_elfHeader.e_shoff;
}

quint32 ELF_Script::getElfHeader_flags()
{
    return g_elfHeader.e_flags;
}

quint16 ELF_Script::getElfHeader_ehsize()
{
    return g_elfHeader.e_ehsize;
}

quint16 ELF_Script::getElfHeader_phentsize()
{
    return g_elfHeader.e_phentsize;
}

quint16 ELF_Script::getElfHeader_phnum()
{
    return g_elfHeader.e_phnum;
}

quint16 ELF_Script::getElfHeader_shentsize()
{
    return g_elfHeader.e_shentsize;
}

quint16 ELF_Script::getElfHeader_shnum()
{
    return g_elfHeader.e_shnum;
}

quint16 ELF_Script::getElfHeader_shstrndx()
{
    return g_elfHeader.e_shstrndx;
}

quint64 ELF_Script::getProgramFileSize(quint32 nNumber)
{
    return g_pELF->getElf_Phdr_filesz(nNumber, &g_listProgramHeaders);
}

quint64 ELF_Script::getProgramFileOffset(quint32 nNumber)
{
    return g_pELF->getElf_Phdr_offset(nNumber, &g_listProgramHeaders);
}

quint64 ELF_Script::getSectionFileOffset(quint32 nNumber)
{
    return g_pELF->getElf_Shdr_offset(nNumber, &g_listSectionHeaders);
}

quint64 ELF_Script::getSectionFileSize(quint32 nNumber)
{
    return g_pELF->getElf_Shdr_size(nNumber, &g_listSectionHeaders);
}

bool ELF_Script::isStringInTablePresent(const QString &sSectionName, const QString &sString)
{
    bool bResult = false;

    qint32 nSection = g_pELF->getSectionNumber(sSectionName, &g_listSectionRecords);

    if (nSection != -1) {
        bResult = (g_pELF->getStringsFromSection(nSection).key(sString, -1) != (quint32)-1);
    }

    return bResult;
}

bool ELF_Script::isNotePresent(const QString &sNote)
{
    return g_pELF->isNotePresent(&g_listNotes, sNote);
}

bool ELF_Script::isLibraryPresent(const QString &sLibraryName)
{
    return g_pELF->isStringInListPresent(&g_listLibraryNames, sLibraryName, getPdStruct());
}
