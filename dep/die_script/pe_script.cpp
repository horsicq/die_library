/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#include "pe_script.h"

PE_Script::PE_Script(XPE *pPE, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : MSDOS_Script(pPE, pOptions, pPdStruct)
{
    this->pPE = pPE;

    g_nNumberOfSections = pPE->getFileHeader_NumberOfSections();

    g_listSectionHeaders = pPE->getSectionHeaders();
    g_listSectionRecords = pPE->getSectionRecords(&g_listSectionHeaders);
    g_listSectionNameStrings = pPE->getSectionNames(&g_listSectionRecords);

    g_cliInfo = pPE->getCliInfo(true, getMemoryMap());
    g_listResourceRecords = pPE->getResources(getMemoryMap());

    g_resourcesVersion = pPE->getResourcesVersion(&g_listResourceRecords);

    g_nNumberOfResources = g_listResourceRecords.count();

    g_listImportHeaders = pPE->getImports(getMemoryMap());
    g_listImportRecords = pPE->getImportRecords(getMemoryMap());

    g_nNumberOfImports = g_listImportHeaders.count();

    g_bIsNETPresent = (pPE->isNETPresent()) && (g_cliInfo.bValid);
    bool bIs64 = pPE->is64(getMemoryMap());
    g_bIsDll = pPE->isDll();
    g_bIsDriver = pPE->isDriver();
    g_bIsConsole = pPE->isConsole();
    g_bIsSignPresent = pPE->isSignPresent();
    g_bIsExportPresent = pPE->isExportPresent();
    g_bIsTLSPresent = pPE->isTLSPresent();
    g_bIsImportPresent = pPE->isImportPresent();
    g_bIsResourcesPresent = pPE->isResourcesPresent();

    g_nImportSection = pPE->getImportSection(getMemoryMap());
    g_nExportSection = pPE->getExportSection(getMemoryMap());
    g_nResourcesSection = pPE->getResourcesSection(getMemoryMap());
    g_nEntryPointSection = pPE->getEntryPointSection(getMemoryMap());
    g_nRelocsSection = pPE->getRelocsSection(getMemoryMap());
    g_nTLSSection = pPE->getTLSSection(getMemoryMap());

    g_nMajorLinkerVersion = pPE->getOptionalHeader_MajorLinkerVersion();
    g_nMinorLinkerVersion = pPE->getOptionalHeader_MinorLinkerVersion();
    g_nSizeOfCode = pPE->getOptionalHeader_SizeOfCode();
    g_nSizeOfUninitializedData = pPE->getOptionalHeader_SizeOfUninitializedData();

    g_sCompilerVersion = QString("%1.%2").arg(g_nMajorLinkerVersion).arg(g_nMinorLinkerVersion);
    g_sGeneralOptions = QString("%1%2").arg(pPE->getTypeAsString()).arg(bIs64 ? ("64") : ("32"));

    g_sFileVersion = pPE->getFileVersion(&g_resourcesVersion);
    g_sFileVersionMS = pPE->getFileVersionMS(&g_resourcesVersion);

    g_nCalculateSizeOfHeaders = pPE->calculateHeadersSize();

    g_exportHeader = pPE->getExport();
    g_nNumberOfExportFunctions = g_exportHeader.listPositions.count();

    g_listExportFunctionNameStrings = pPE->getExportFunctionsList(&g_exportHeader);

    g_nImportHash64 = pPE->getImportHash64(&g_listImportRecords);
    g_nImportHash32 = pPE->getImportHash32(&g_listImportRecords);
    g_listImportPositionHashes = pPE->getImportPositionHashes(&g_listImportHeaders);
}

PE_Script::~PE_Script()
{
}

quint16 PE_Script::getNumberOfSections()
{
    return g_nNumberOfSections;
}

QString PE_Script::getSectionName(quint32 nNumber)
{
    return pPE->getSection_NameAsString(nNumber, &g_listSectionNameStrings);
}

quint32 PE_Script::getSectionVirtualSize(quint32 nNumber)
{
    return pPE->getSection_VirtualSize(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionVirtualAddress(quint32 nNumber)
{
    return pPE->getSection_VirtualAddress(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionFileSize(quint32 nNumber)
{
    return pPE->getSection_SizeOfRawData(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionFileOffset(quint32 nNumber)
{
    return pPE->getSection_PointerToRawData(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getSectionCharacteristics(quint32 nNumber)
{
    return pPE->getSection_Characteristics(nNumber, &g_listSectionHeaders);
}

quint32 PE_Script::getNumberOfResources()
{
    return g_nNumberOfResources;
}

bool PE_Script::isSectionNamePresent(const QString &sSectionName)
{
    return XBinary::isStringInListPresent(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

bool PE_Script::isSectionNamePresentExp(const QString &sSectionName)
{
    return XBinary::isStringInListPresentExp(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

bool PE_Script::isNET()
{
    return g_bIsNETPresent;
}

bool PE_Script::isNet()
{
    return g_bIsNETPresent;
}

bool PE_Script::isPEPlus()
{
    return is64();
}

QString PE_Script::getGeneralOptions()
{
    return g_sGeneralOptions;
}

quint32 PE_Script::getResourceIdByNumber(quint32 nNumber)
{
    return pPE->getResourceIdByNumber(nNumber, &g_listResourceRecords);
}

QString PE_Script::getResourceNameByNumber(quint32 nNumber)
{
    return pPE->getResourceNameByNumber(nNumber, &g_listResourceRecords);
}

qint64 PE_Script::getResourceOffsetByNumber(quint32 nNumber)
{
    return pPE->getResourceOffsetByNumber(nNumber, &g_listResourceRecords);
}

qint64 PE_Script::getResourceSizeByNumber(quint32 nNumber)
{
    return pPE->getResourceSizeByNumber(nNumber, &g_listResourceRecords);
}

quint32 PE_Script::getResourceTypeByNumber(quint32 nNumber)
{
    return pPE->getResourceTypeByNumber(nNumber, &g_listResourceRecords);
}

bool PE_Script::isNETStringPresent(const QString &sString)
{
    return pPE->isNETAnsiStringPresent(sString, &g_cliInfo);
}

bool PE_Script::isNetObjectPresent(const QString &sString)
{
    return pPE->isNETAnsiStringPresent(sString, &g_cliInfo);
}

bool PE_Script::isNETUnicodeStringPresent(const QString &sString)
{
    return pPE->isNETUnicodeStringPresent(sString, &g_cliInfo);
}

bool PE_Script::isNetUStringPresent(const QString &sString)
{
    return pPE->isNETUnicodeStringPresent(sString, &g_cliInfo);
}

qint64 PE_Script::findSignatureInBlob_NET(const QString &sSignature)
{
    return pPE->findSignatureInBlob_NET(sSignature, getMemoryMap(), getPdStruct());
}

bool PE_Script::isSignatureInBlobPresent_NET(const QString &sSignature)
{
    return pPE->isSignatureInBlobPresent_NET(sSignature, getMemoryMap(), getPdStruct());
}

qint32 PE_Script::getNumberOfImports()
{
    return g_nNumberOfImports;
}

QString PE_Script::getImportLibraryName(quint32 nNumber)
{
    return pPE->getImportLibraryName(nNumber, &g_listImportHeaders);
}

bool PE_Script::isLibraryPresent(const QString &sLibraryName, bool bCheckCase)
{
    bool bResult = false;

    if (bCheckCase) {
        bResult = pPE->isImportLibraryPresent(sLibraryName, &g_listImportHeaders);  // TODO pdStruct
    } else {
        bResult = pPE->isImportLibraryPresentI(sLibraryName, &g_listImportHeaders);  // TODO pdStruct
    }

    return bResult;
}

bool PE_Script::isLibraryFunctionPresent(const QString &sLibraryName, const QString &sFunctionName)
{
    return pPE->isImportFunctionPresentI(sLibraryName, sFunctionName, &g_listImportHeaders);  // TODO pdStruct
}

QString PE_Script::getImportFunctionName(quint32 nImport, quint32 nFunctionNumber)
{
    return pPE->getImportFunctionName(nImport, nFunctionNumber, &g_listImportHeaders);
}

qint32 PE_Script::getImportSection()
{
    return g_nImportSection;
}

qint32 PE_Script::getExportSection()
{
    return g_nExportSection;
}

qint32 PE_Script::getResourceSection()
{
    return g_nResourcesSection;
}

qint32 PE_Script::getEntryPointSection()
{
    return g_nEntryPointSection;
}

qint32 PE_Script::getRelocsSection()
{
    return g_nRelocsSection;
}

qint32 PE_Script::getTLSSection()
{
    return g_nTLSSection;
}

quint8 PE_Script::getMajorLinkerVersion()
{
    return g_nMajorLinkerVersion;
}

quint8 PE_Script::getMinorLinkerVersion()
{
    return g_nMinorLinkerVersion;
}

QString PE_Script::getManifest()
{
    return pPE->getResourceManifest(&g_listResourceRecords);
}

QString PE_Script::getVersionStringInfo(const QString &sKey)
{
    return pPE->getResourcesVersionValue(sKey, &g_resourcesVersion);
}

qint32 PE_Script::getNumberOfImportThunks(quint32 nNumber)
{
    return pPE->getNumberOfImportThunks(nNumber, &g_listImportHeaders);
}

qint64 PE_Script::getResourceNameOffset(const QString &sName)
{
    return pPE->getResourceNameOffset(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceNamePresent(const QString &sName)
{
    return pPE->isResourceNamePresent(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceGroupNamePresent(const QString &sName)
{
    return pPE->isResourceGroupNamePresent(sName, &g_listResourceRecords);
}

bool PE_Script::isResourceGroupIdPresent(quint32 nID)
{
    return pPE->isResourceGroupIdPresent(nID, &g_listResourceRecords);
}

QString PE_Script::getCompilerVersion()
{
    return g_sCompilerVersion;
}

bool PE_Script::isConsole()
{
    return g_bIsConsole;
}

bool PE_Script::isSignedFile()
{
    return g_bIsSignPresent;
}

QString PE_Script::getSectionNameCollision(const QString &sString1, const QString &sString2)
{
    return pPE->getStringCollision(&g_listSectionNameStrings, sString1, sString2);
}

qint32 PE_Script::getSectionNumber(const QString &sSectionName)
{
    return XBinary::getStringNumberFromList(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

qint32 PE_Script::getSectionNumberExp(const QString &sSectionName)
{
    return XBinary::getStringNumberFromListExp(&g_listSectionNameStrings, sSectionName, getPdStruct());
}

bool PE_Script::isDll()
{
    return g_bIsDll;
}

bool PE_Script::isDriver()
{
    return g_bIsDriver;
}

QString PE_Script::getNETVersion()
{
    return g_cliInfo.metaData.header.sVersion;
}

bool PE_Script::compareEP_NET(const QString &sSignature, qint64 nOffset)
{
    return pPE->compareSignatureOnAddress(getMemoryMap(), sSignature, getBaseAddress() + g_cliInfo.metaData.nEntryPoint + nOffset);
}

quint32 PE_Script::getSizeOfCode()
{
    return g_nSizeOfCode;
}

quint32 PE_Script::getSizeOfUninitializedData()
{
    return g_nSizeOfUninitializedData;
}

QString PE_Script::getPEFileVersion(const QString &sFileName)
{
    QString sResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        XPE pe(&file);
        sResult = pe.getFileVersion();
        file.close();
    }

    return sResult;
}

QString PE_Script::getFileVersion()
{
    return g_sFileVersion;
}

QString PE_Script::getFileVersionMS()
{
    return g_sFileVersionMS;
}

qint64 PE_Script::calculateSizeOfHeaders()
{
    return g_nCalculateSizeOfHeaders;
}

bool PE_Script::isExportFunctionPresent(const QString &sFunctionName)
{
    return XBinary::isStringInListPresent(&g_listExportFunctionNameStrings, sFunctionName, getPdStruct());
}

bool PE_Script::isExportFunctionPresentExp(const QString &sFunctionName)
{
    return XBinary::isStringInListPresentExp(&g_listExportFunctionNameStrings, sFunctionName, getPdStruct());
}

qint32 PE_Script::getNumberOfExportFunctions()
{
    return g_nNumberOfExportFunctions;
}

bool PE_Script::isExportPresent()
{
    return g_bIsExportPresent;
}

bool PE_Script::isTLSPresent()
{
    return g_bIsTLSPresent;
}

bool PE_Script::isImportPresent()
{
    return g_bIsImportPresent;
}

bool PE_Script::isResourcesPresent()
{
    return g_bIsResourcesPresent;
}

quint32 PE_Script::getImportHash32()
{
    return g_nImportHash32;
}

quint64 PE_Script::getImportHash64()
{
    return g_nImportHash64;
}

bool PE_Script::isImportPositionHashPresent(qint32 nIndex, quint32 nHash)
{
    return XPE::isImportPositionHashPresent(&g_listImportPositionHashes, nIndex, nHash);
}
