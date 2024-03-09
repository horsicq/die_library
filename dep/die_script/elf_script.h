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
#ifndef ELF_SCRIPT_H
#define ELF_SCRIPT_H

#include "binary_script.h"
#include "xelf.h"

class ELF_Script : public Binary_Script {
    Q_OBJECT

public:
    explicit ELF_Script(XELF *pELF, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);
    ~ELF_Script();

public slots:
    bool isSectionNamePresent(const QString &sSectionName);
    quint32 getNumberOfSections();
    quint32 getNumberOfPrograms();
    virtual QString getGeneralOptions();
    qint32 getSectionNumber(const QString &sSectionName);
    quint16 getElfHeader_type();
    quint16 getElfHeader_machine();
    quint32 getElfHeader_version();
    quint64 getElfHeader_entry();
    quint64 getElfHeader_phoff();
    quint64 getElfHeader_shoff();
    quint32 getElfHeader_flags();
    quint16 getElfHeader_ehsize();
    quint16 getElfHeader_phentsize();
    quint16 getElfHeader_phnum();
    quint16 getElfHeader_shentsize();
    quint16 getElfHeader_shnum();
    quint16 getElfHeader_shstrndx();
    quint64 getProgramFileSize(quint32 nNumber);
    quint64 getProgramFileOffset(quint32 nNumber);
    quint64 getSectionFileOffset(quint32 nNumber);
    quint64 getSectionFileSize(quint32 nNumber);
    bool isStringInTablePresent(const QString &sSectionName, const QString &sString);
    bool isNotePresent(const QString &sNote);
    bool isLibraryPresent(const QString &sLibraryName);

private:
    XELF *g_pELF;
    XELF_DEF::Elf_Ehdr g_elfHeader;
    quint32 g_nStringTableSection;
    QByteArray g_baStringTable;
    QList<XELF_DEF::Elf_Shdr> g_listSectionHeaders;
    QList<XELF_DEF::Elf_Phdr> g_listProgramHeaders;
    QList<XELF::NOTE> g_listNotes;
    QList<XELF::SECTION_RECORD> g_listSectionRecords;
    QString g_sGeneralOptions;
    QList<QString> g_listLibraryNames;
    QList<XELF::TAG_STRUCT> g_listTagStruct;
};

#endif  // ELF_SCRIPT_H
