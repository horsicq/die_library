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
#ifndef PE_SCRIPT_H
#define PE_SCRIPT_H

#include "msdos_script.h"
#include "xpe.h"

class PE_Script : public MSDOS_Script {
    Q_OBJECT

public:
    explicit PE_Script(XPE *pPE, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);
    ~PE_Script();

public slots:
    quint16 getNumberOfSections();
    QString getSectionName(quint32 nNumber);
    quint32 getSectionVirtualSize(quint32 nNumber);
    quint32 getSectionVirtualAddress(quint32 nNumber);
    quint32 getSectionFileSize(quint32 nNumber);
    quint32 getSectionFileOffset(quint32 nNumber);
    quint32 getSectionCharacteristics(quint32 nNumber);
    quint32 getNumberOfResources();
    bool isSectionNamePresent(const QString &sSectionName);
    bool isSectionNamePresentExp(const QString &sSectionName);
    bool isNET();
    bool isNet();
    bool isPEPlus();
    virtual QString getGeneralOptions();
    quint32 getResourceIdByNumber(quint32 nNumber);
    QString getResourceNameByNumber(quint32 nNumber);
    qint64 getResourceOffsetByNumber(quint32 nNumber);
    qint64 getResourceSizeByNumber(quint32 nNumber);
    quint32 getResourceTypeByNumber(quint32 nNumber);
    bool isNETStringPresent(const QString &sString);
    bool isNetObjectPresent(const QString &sString);
    bool isNETUnicodeStringPresent(const QString &sString);
    bool isNetUStringPresent(const QString &sString);
    qint64 findSignatureInBlob_NET(const QString &sSignature);
    bool isSignatureInBlobPresent_NET(const QString &sSignature);
    qint32 getNumberOfImports();
    QString getImportLibraryName(quint32 nNumber);
    bool isLibraryPresent(const QString &sLibraryName, bool bCheckCase = false);
    bool isLibraryFunctionPresent(const QString &sLibraryName, const QString &sFunctionName);
    QString getImportFunctionName(quint32 nImport, quint32 nFunctionNumber);
    qint32 getImportSection();
    qint32 getExportSection();
    qint32 getResourceSection();
    qint32 getEntryPointSection();
    qint32 getRelocsSection();
    qint32 getTLSSection();
    quint8 getMajorLinkerVersion();
    quint8 getMinorLinkerVersion();
    QString getManifest();
    QString getVersionStringInfo(const QString &sKey);
    qint32 getNumberOfImportThunks(quint32 nNumber);
    qint64 getResourceNameOffset(const QString &sName);
    bool isResourceNamePresent(const QString &sName);
    bool isResourceGroupNamePresent(const QString &sName);
    bool isResourceGroupIdPresent(quint32 nID);
    QString getCompilerVersion();
    bool isConsole();
    bool isSignedFile();
    QString getSectionNameCollision(const QString &sString1, const QString &sString2);  // mb TODO move to Binary
    qint32 getSectionNumber(const QString &sSectionName);
    qint32 getSectionNumberExp(const QString &sSectionName);
    bool isDll();
    bool isDriver();
    QString getNETVersion();
    bool compareEP_NET(const QString &sSignature, qint64 nOffset = 0);
    quint32 getSizeOfCode();
    quint32 getSizeOfUninitializedData();
    QString getPEFileVersion(const QString &sFileName);
    QString getFileVersion();
    QString getFileVersionMS();
    qint64 calculateSizeOfHeaders();
    bool isExportFunctionPresent(const QString &sFunctionName);
    bool isExportFunctionPresentExp(const QString &sFunctionName);
    qint32 getNumberOfExportFunctions();
    bool isExportPresent();
    bool isTLSPresent();
    bool isImportPresent();
    bool isResourcesPresent();
    quint32 getImportHash32();
    quint64 getImportHash64();
    bool isImportPositionHashPresent(qint32 nIndex, quint32 nHash);

private:
    XPE *pPE;
    qint32 g_nNumberOfSections;
    XPE::CLI_INFO g_cliInfo;
    QList<XPE::RESOURCE_RECORD> g_listResourceRecords;
    qint32 g_nNumberOfResources;
    QList<XPE_DEF::IMAGE_SECTION_HEADER> g_listSectionHeaders;
    QList<XPE::SECTION_RECORD> g_listSectionRecords;
    QList<QString> g_listSectionNameStrings;
    QList<XPE::IMPORT_HEADER> g_listImportHeaders;
    QList<XPE::IMPORT_RECORD> g_listImportRecords;
    qint32 g_nNumberOfImports;
    qint32 g_nNumberOfExportFunctions;
    XPE::RESOURCES_VERSION g_resourcesVersion;
    bool g_bIsNETPresent;
    bool g_bIsDll;
    bool g_bIsDriver;
    bool g_bIsConsole;
    bool g_bIsSignPresent;
    bool g_bIsExportPresent;
    bool g_bIsTLSPresent;
    bool g_bIsImportPresent;
    bool g_bIsResourcesPresent;
    QString g_sGeneralOptions;
    qint32 g_nImportSection;
    qint32 g_nExportSection;
    qint32 g_nResourcesSection;
    qint32 g_nEntryPointSection;
    qint32 g_nRelocsSection;
    qint32 g_nTLSSection;
    quint8 g_nMajorLinkerVersion;
    quint8 g_nMinorLinkerVersion;
    quint32 g_nSizeOfCode;
    quint32 g_nSizeOfUninitializedData;
    QString g_sCompilerVersion;
    QString g_sFileVersion;
    QString g_sFileVersionMS;
    qint32 g_nCalculateSizeOfHeaders;
    XPE::EXPORT_HEADER g_exportHeader;
    QList<QString> g_listExportFunctionNameStrings;
    quint64 g_nImportHash64;
    quint64 g_nImportHash32;
    QList<quint32> g_listImportPositionHashes;
};

#endif  // PE_SCRIPT_H
