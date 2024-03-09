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
#include "mach_script.h"

MACH_Script::MACH_Script(XMACH *pMACH, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : Binary_Script(pMACH, pOptions, pPdStruct)
{
    this->g_pMACH = pMACH;

    g_listLibraryRecords = pMACH->getLibraryRecords();
    g_listSectionRecords = pMACH->getSectionRecords();
    g_listCommandRecords = pMACH->getCommandRecords();
    g_listSectionNameStrings = pMACH->getSectionNames(&g_listSectionRecords);

    g_nNumberOfSection = g_listSectionRecords.count();
    g_nNumberOfSegments = g_listSegmentRecords.count();

    bool bIs64 = pMACH->is64(getMemoryMap());

    g_sGeneralOptions = QString("%1%2").arg(XMACH::getHeaderFileTypesS().value(pMACH->getHeader_filetype())).arg(bIs64 ? ("64") : ("32"));
}

MACH_Script::~MACH_Script()
{
}

bool MACH_Script::isLibraryPresent(const QString &sLibraryName)
{
    return g_pMACH->isLibraryRecordNamePresent(sLibraryName, &g_listLibraryRecords);
}

quint32 MACH_Script::getNumberOfSections()
{
    return g_nNumberOfSection;
}

quint32 MACH_Script::getNumberOfSegments()
{
    return g_nNumberOfSegments;
}

qint32 MACH_Script::getSectionNumber(const QString &sSectionName)
{
    return g_pMACH->getSectionNumber(sSectionName, &g_listSectionRecords);
}

QString MACH_Script::getGeneralOptions()
{
    return g_sGeneralOptions;
}

quint32 MACH_Script::getLibraryCurrentVersion(const QString &sLibraryName)
{
    return g_pMACH->getLibraryCurrentVersion(sLibraryName, &g_listLibraryRecords);
}

quint64 MACH_Script::getSectionFileOffset(quint32 nNumber)
{
    return g_pMACH->getSectionFileOffset(nNumber, &g_listSectionRecords);
}

quint64 MACH_Script::getSectionFileSize(quint32 nNumber)
{
    return g_pMACH->getSectionFileSize(nNumber, &g_listSectionRecords);
}

bool MACH_Script::isSectionNamePresent(const QString &sSectionName)
{
    return XBinary::isStringInListPresent(&g_listSectionNameStrings, sSectionName);
}
