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
#include "binary_script.h"

Binary_Script::Binary_Script(XBinary *pBinary, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pBinary = pBinary;
    this->g_pPdStruct = pPdStruct;
    this->g_pOptions = pOptions;

    connect(pBinary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    connect(pBinary, SIGNAL(infoMessage(QString)), this, SIGNAL(infoMessage(QString)));

    g_nSize = pBinary->getSize();
    g_memoryMap = pBinary->getMemoryMap(XBinary::MAPMODE_UNKNOWN, pPdStruct);
    g_nBaseAddress = pBinary->getBaseAddress();

    g_nEntryPointOffset = pBinary->getEntryPointOffset(&g_memoryMap);
    g_nEntryPointAddress = pBinary->getEntryPointAddress(&g_memoryMap);
    g_nOverlayOffset = pBinary->getOverlayOffset(&g_memoryMap);
    g_nOverlaySize = pBinary->getOverlaySize(&g_memoryMap);
    g_bIsOverlayPresent = pBinary->isOverlayPresent(&g_memoryMap);
    g_bIsBigEndian = pBinary->isBigEndian();

    g_sHeaderSignature = pBinary->getSignature(0, 256);  // TODO const
    g_nHeaderSignatureSize = g_sHeaderSignature.size() / 2;

    g_sEntryPointSignature = pBinary->getSignature(g_nEntryPointOffset, 256);  // TODO const
    g_nEntryPointSignatureSize = g_sEntryPointSignature.size();

    if (g_nOverlayOffset > 0) {
        g_sOverlaySignature = pBinary->getSignature(g_nOverlayOffset, 256);  // TODO const
        g_nOverlaySignatureSize = g_sOverlaySignature.size();
    }

    g_sFileDirectory = XBinary::getDeviceDirectory(pBinary->getDevice());
    g_sFileBaseName = XBinary::getDeviceFileBaseName(pBinary->getDevice());
    g_sFileCompleteSuffix = XBinary::getDeviceFileCompleteSuffix(pBinary->getDevice());
    g_sFileSuffix = XBinary::getDeviceFileSuffix(pBinary->getDevice());

    g_bIsPlainText = pBinary->isPlainTextType();
    g_bIsUTF8Text = pBinary->isUTF8TextType();
    XBinary::UNICODE_TYPE unicodeType = pBinary->getUnicodeType();

    if (unicodeType != XBinary::UNICODE_TYPE_NONE) {
        g_sHeaderString = pBinary->read_unicodeString(2, qMin(g_nSize, (qint64)0x1000), (unicodeType == XBinary::UNICODE_TYPE_BE));
        g_bIsUnicodeText = true;
    } else if (g_bIsUTF8Text) {
        g_sHeaderString = pBinary->read_utf8String(3, qMin(g_nSize, (qint64)0x1000));
    } else if (g_bIsPlainText) {
        g_sHeaderString = pBinary->read_ansiString(0, qMin(g_nSize, (qint64)0x1000));
    }

    g_bIsJpeg = false;
    g_osJpegExif = XBinary::OFFSETSIZE();

    g_pJpeg = dynamic_cast<XJpeg *>(pBinary);

    if (g_pJpeg) {
        g_bIsJpeg = true;
        g_listJpegChunks = g_pJpeg->getChunks(pPdStruct);
        g_osJpegExif = g_pJpeg->getExif(&g_listJpegChunks);
        g_listJpegExifChunks = g_pJpeg->getExifChunks(g_osJpegExif);
        g_sJpegExifCameraName = g_pJpeg->getExifCameraName(g_osJpegExif, &g_listJpegExifChunks);
    }

    g_pZip = dynamic_cast<XZip *>(pBinary);

    if (g_pZip) {
        g_listArchiveRecords = g_pZip->getRecords(20000, pPdStruct);
        g_osInfo = g_pZip->getOsInfo(&g_listArchiveRecords, pPdStruct);
    } else {
        g_osInfo = pBinary->getOsInfo();
    }

    g_bIsSigned = pBinary->isSigned();

    XCapstone::openHandle(XBinary::getDisasmMode(&g_memoryMap), &g_disasmHandle, true);
}

Binary_Script::~Binary_Script()
{
    XCapstone::closeHandle(&g_disasmHandle);
}

qint64 Binary_Script::getSize()
{
    return g_nSize;
}

bool Binary_Script::compare(const QString &sSignature, qint64 nOffset)
{
    bool bResult = false;

    qint32 nSignatureSize = sSignature.size();

    if ((nSignatureSize + nOffset < g_nHeaderSignatureSize) && (!sSignature.contains('$')) && (!sSignature.contains('#')) && (!sSignature.contains('+')) &&
        (!sSignature.contains('%')) && (!sSignature.contains('*'))) {
        bResult = g_pBinary->compareSignatureStrings(g_sHeaderSignature.mid(nOffset * 2, nSignatureSize * 2), sSignature);
    } else {
        bResult = g_pBinary->compareSignature(&g_memoryMap, sSignature, nOffset);
    }

    return bResult;
}

bool Binary_Script::compareEP(const QString &sSignature, qint64 nOffset)
{
    bool bResult = false;

    qint32 nSignatureSize = sSignature.size();

    if ((nSignatureSize + nOffset < g_nEntryPointSignatureSize) && (!sSignature.contains('$')) && (!sSignature.contains('#')) && (!sSignature.contains('+')) &&
        (!sSignature.contains('%')) && (!sSignature.contains('*'))) {
        bResult = g_pBinary->compareSignatureStrings(g_sEntryPointSignature.mid(nOffset * 2, nSignatureSize * 2), sSignature);
    } else {
        bResult = g_pBinary->compareEntryPoint(&g_memoryMap, sSignature, nOffset);
    }

    return bResult;
}

quint8 Binary_Script::readByte(qint64 nOffset)
{
    return g_pBinary->read_uint8(nOffset);
}

qint16 Binary_Script::readSByte(qint64 nOffset)
{
    return g_pBinary->read_int8(nOffset);
}

quint16 Binary_Script::readWord(qint64 nOffset)
{
    return g_pBinary->read_uint16(nOffset);
}

qint16 Binary_Script::readSWord(qint64 nOffset)
{
    return g_pBinary->read_int16(nOffset);
}

quint32 Binary_Script::readDword(qint64 nOffset)
{
    return g_pBinary->read_uint32(nOffset);
}

qint32 Binary_Script::readSDword(qint64 nOffset)
{
    return g_pBinary->read_int32(nOffset);
}

quint64 Binary_Script::readQword(qint64 nOffset)
{
    return g_pBinary->read_uint64(nOffset);
}

qint64 Binary_Script::readSQword(qint64 nOffset)
{
    return g_pBinary->read_int64(nOffset);
}

QString Binary_Script::getString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_ansiString(nOffset, nMaxSize);
}

qint64 Binary_Script::findSignature(qint64 nOffset, qint64 nSize, const QString &sSignature)
{
    qint64 nResult = -1;
    qint64 nResultSize = 0;

    _fixOffsetAndSize(&nOffset, &nSize);

    nResult = g_pBinary->find_signature(&g_memoryMap, nOffset, nSize, sSignature, &nResultSize, g_pPdStruct);

    return nResult;
}

qint64 Binary_Script::findString(qint64 nOffset, qint64 nSize, const QString &sString)
{
    _fixOffsetAndSize(&nOffset, &nSize);

    return g_pBinary->find_ansiString(nOffset, nSize, sString, g_pPdStruct);
}

qint64 Binary_Script::findByte(qint64 nOffset, qint64 nSize, quint8 nValue)
{
    _fixOffsetAndSize(&nOffset, &nSize);

    return g_pBinary->find_uint8(nOffset, nSize, nValue, g_pPdStruct);
}

qint64 Binary_Script::findWord(qint64 nOffset, qint64 nSize, quint16 nValue)
{
    _fixOffsetAndSize(&nOffset, &nSize);

    return g_pBinary->find_uint16(nOffset, nSize, nValue, g_pPdStruct);
}

qint64 Binary_Script::findDword(qint64 nOffset, qint64 nSize, quint32 nValue)
{
    _fixOffsetAndSize(&nOffset, &nSize);

    return g_pBinary->find_uint32(nOffset, nSize, nValue, g_pPdStruct);
}

qint64 Binary_Script::getEntryPointOffset()
{
    return g_nEntryPointOffset;
}

qint64 Binary_Script::getOverlayOffset()
{
    return g_nOverlayOffset;
}

qint64 Binary_Script::getOverlaySize()
{
    return g_nOverlaySize;
}

qint64 Binary_Script::getAddressOfEntryPoint()
{
    return g_nEntryPointAddress;
}

bool Binary_Script::isOverlayPresent()
{
    return g_bIsOverlayPresent;
}

bool Binary_Script::compareOverlay(const QString &sSignature, qint64 nOffset)
{
    bool bResult = false;

    qint32 nSignatureSize = sSignature.size();

    if ((nSignatureSize + nOffset < g_nOverlaySignatureSize) && (!sSignature.contains('$')) && (!sSignature.contains('#')) && (!sSignature.contains('+')) &&
        (!sSignature.contains('%')) && (!sSignature.contains('*'))) {
        bResult = g_pBinary->compareSignatureStrings(g_sOverlaySignature.mid(nOffset * 2, nSignatureSize * 2), sSignature);
    } else {
        bResult = g_pBinary->compareOverlay(&g_memoryMap, sSignature, nOffset);
    }

    return bResult;
}

bool Binary_Script::isSignaturePresent(qint64 nOffset, qint64 nSize, const QString &sSignature)
{
    return g_pBinary->isSignaturePresent(&g_memoryMap, nOffset, nSize, sSignature, g_pPdStruct);
}

quint32 Binary_Script::swapBytes(quint32 nValue)
{
    return g_pBinary->swapBytes(nValue);
}

QString Binary_Script::getGeneralOptions()
{
    return "";
}

qint64 Binary_Script::RVAToOffset(qint64 nRVA)
{
    return g_pBinary->addressToOffset(&g_memoryMap, nRVA + g_nBaseAddress);
}

qint64 Binary_Script::VAToOffset(qint64 nVA)
{
    return g_pBinary->addressToOffset(&g_memoryMap, nVA);
}

qint64 Binary_Script::OffsetToVA(qint64 nOffset)
{
    return g_pBinary->offsetToAddress(&g_memoryMap, nOffset);
}

qint64 Binary_Script::OffsetToRVA(qint64 nOffset)
{
    qint64 nResult = g_pBinary->offsetToAddress(&g_memoryMap, nOffset);

    if (nResult != -1) {
        nResult -= g_nBaseAddress;
    }

    return nResult;
}

QString Binary_Script::getFileDirectory()
{
    return g_sFileDirectory;
}

QString Binary_Script::getFileBaseName()
{
    return g_sFileBaseName;
}

QString Binary_Script::getFileCompleteSuffix()
{
    return g_sFileCompleteSuffix;
}

QString Binary_Script::getFileSuffix()
{
    return g_sFileSuffix;
}

QString Binary_Script::getSignature(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getSignature(nOffset, nSize);
}

double Binary_Script::calculateEntropy(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getEntropy(nOffset, nSize, g_pPdStruct);
}

QString Binary_Script::calculateMD5(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->getHash(XBinary::HASH_MD5, nOffset, nSize, g_pPdStruct);
}

quint32 Binary_Script::calculateCRC32(qint64 nOffset, qint64 nSize)
{
    return g_pBinary->_getCRC32(nOffset, nSize, g_pPdStruct);
}

bool Binary_Script::isSignatureInSectionPresent(quint32 nNumber, const QString &sSignature)
{
    return g_pBinary->isSignatureInLoadSegmentPresent(&g_memoryMap, nNumber, sSignature, g_pPdStruct);
}

qint64 Binary_Script::getImageBase()
{
    return g_memoryMap.nModuleAddress;
}

QString Binary_Script::upperCase(const QString &sString)
{
    return sString.toUpper();
}

QString Binary_Script::lowerCase(const QString &sString)
{
    return sString.toLower();
}

bool Binary_Script::isPlainText()
{
    return g_bIsPlainText;
}

bool Binary_Script::isUTF8Text()
{
    return g_bIsUTF8Text;
}

bool Binary_Script::isUnicodeText()
{
    return g_bIsUnicodeText;
}

bool Binary_Script::isText()
{
    return g_bIsPlainText | g_bIsUTF8Text | g_bIsUnicodeText;
}

QString Binary_Script::getHeaderString()
{
    return g_sHeaderString;
}

qint32 Binary_Script::getDisasmLength(qint64 nAddress)
{
    return XCapstone::getDisasmLength(g_disasmHandle, g_pBinary->getDevice(), XBinary::addressToOffset(&g_memoryMap, nAddress), nAddress);
}

QString Binary_Script::getDisasmString(qint64 nAddress)
{
    qint64 nOffset = XBinary::addressToOffset(&g_memoryMap, nAddress);
    return XCapstone::disasm(g_disasmHandle, g_pBinary->getDevice(), nOffset, nAddress).sString.toUpper();
}

qint64 Binary_Script::getDisasmNextAddress(qint64 nAddress)
{
    return XCapstone::getNextAddress(XBinary::getDisasmFamily(&g_memoryMap), g_disasmHandle, g_pBinary->getDevice(), XBinary::addressToOffset(&g_memoryMap, nAddress),
                                     nAddress);
}

bool Binary_Script::is16()
{
    return XBinary::is16(&g_memoryMap);
}

bool Binary_Script::is32()
{
    return XBinary::is32(&g_memoryMap);
}

bool Binary_Script::is64()
{
    return XBinary::is64(&g_memoryMap);
}

bool Binary_Script::isDeepScan()
{
    return g_pOptions->bIsDeepScan;
}

bool Binary_Script::isHeuristicScan()
{
    return g_pOptions->bIsHeuristicScan;
}

bool Binary_Script::isVerbose()
{
    return g_pOptions->bIsVerbose;
}

quint8 Binary_Script::read_uint8(qint64 nOffset)
{
    return g_pBinary->read_uint8(nOffset);
}

qint16 Binary_Script::read_int8(qint64 nOffset)
{
    return g_pBinary->read_int8(nOffset);
}

quint16 Binary_Script::read_uint16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint16(nOffset, bIsBigEndian);
}

qint16 Binary_Script::read_int16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int16(nOffset, bIsBigEndian);
}

quint32 Binary_Script::read_uint32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint32(nOffset, bIsBigEndian);
}

qint32 Binary_Script::read_int32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int32(nOffset, bIsBigEndian);
}

quint64 Binary_Script::read_uint64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint64(nOffset, bIsBigEndian);
}

qint64 Binary_Script::read_int64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int64(nOffset, bIsBigEndian);
}

QString Binary_Script::read_ansiString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_ansiString(nOffset, nMaxSize);
}

QString Binary_Script::read_unicodeString(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_unicodeString(nOffset, nMaxSize);
}

QString Binary_Script::read_utf8String(qint64 nOffset, qint64 nMaxSize)
{
    return g_pBinary->read_utf8String(nOffset, nMaxSize);
}

QString Binary_Script::read_ucsdString(qint64 nOffset)
{
    return g_pBinary->read_ucsdString(nOffset);
}

QString Binary_Script::read_codePageString(qint64 nOffset, qint64 nMaxByteSize, const QString &sCodePage)
{
    return g_pBinary->read_codePageString(nOffset, nMaxByteSize, sCodePage);
}

QString Binary_Script::bytesCountToString(quint64 nValue)
{
    return g_pBinary->bytesCountToString(nValue);
}

qint64 Binary_Script::find_ansiString(qint64 nOffset, qint64 nSize, const QString &sString)
{
    return g_pBinary->find_ansiString(nOffset, nSize, sString, g_pPdStruct);
}

qint64 Binary_Script::find_unicodeString(qint64 nOffset, qint64 nSize, const QString &sString)
{
    return g_pBinary->find_unicodeString(nOffset, nSize, sString, g_bIsBigEndian, g_pPdStruct);
}

qint64 Binary_Script::find_utf8String(qint64 nOffset, qint64 nSize, const QString &sString)
{
    return g_pBinary->find_utf8String(nOffset, nSize, sString, g_pPdStruct);
}

QString Binary_Script::read_UUID_bytes(qint64 nOffset)
{
    return g_pBinary->read_UUID_bytes(nOffset);
}

QString Binary_Script::read_UUID(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_UUID(nOffset, bIsBigEndian);
}

float Binary_Script::read_float(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_float(nOffset, bIsBigEndian);
}

double Binary_Script::read_double(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_double(nOffset, bIsBigEndian);
}

float Binary_Script::read_float16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_float16(nOffset, bIsBigEndian);
}

float Binary_Script::read_float32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_float(nOffset, bIsBigEndian);
}

double Binary_Script::read_float64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_double(nOffset, bIsBigEndian);
}

quint32 Binary_Script::read_uint24(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_uint24(nOffset, bIsBigEndian);
}

qint32 Binary_Script::read_int24(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_int24(nOffset, bIsBigEndian);
}

quint8 Binary_Script::read_bcd_uint8(qint64 nOffset)
{
    return g_pBinary->read_bcd_uint8(nOffset);
}

quint16 Binary_Script::read_bcd_uint16(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_bcd_uint16(nOffset, bIsBigEndian);
}

quint16 Binary_Script::read_bcd_uint32(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_bcd_uint32(nOffset, bIsBigEndian);
}

quint16 Binary_Script::read_bcd_uint64(qint64 nOffset, bool bIsBigEndian)
{
    return g_pBinary->read_bcd_uint64(nOffset, bIsBigEndian);
}

bool Binary_Script::isJpeg()
{
    return g_bIsJpeg;
}

QString Binary_Script::getJpegComment()
{
    QString sResult;

    if (g_pJpeg) {
        sResult = g_pJpeg->getComment(&g_listJpegChunks);
    }

    return sResult;
}

QString Binary_Script::getJpegDqtMD5()
{
    QString sResult;

    if (g_pJpeg) {
        sResult = g_pJpeg->getDqtMD5(&g_listJpegChunks);
    }

    return sResult;
}

bool Binary_Script::isJpegChunkPresent(qint32 nID)
{
    bool bResult = false;

    if (g_pJpeg) {
        bResult = g_pJpeg->isChunkPresent(&g_listJpegChunks, (qint8)nID);
    }

    return bResult;
}

bool Binary_Script::isJpegExifPresent()
{
    bool bResult = false;

    if (g_pJpeg) {
        bResult = g_pJpeg->isExifPresent(g_osJpegExif);
    }

    return bResult;
}

QString Binary_Script::getJpegExifCameraName()
{
    return g_sJpegExifCameraName;
}

QString Binary_Script::getOperationSystemName()
{
    return XBinary::osNameIdToString(g_osInfo.osName);
}

QString Binary_Script::getOperationSystemVersion()
{
    return g_osInfo.sOsVersion;
}

QString Binary_Script::getOperationSystemOptions()
{
    QString sResult = QString("%1, %2, %3").arg(g_osInfo.sArch, XBinary::modeIdToString(g_osInfo.mode), g_osInfo.sType);

    if (g_osInfo.endian == XBinary::ENDIAN_BIG) {
        if (sResult != "") {
            sResult.append(", ");
        }
        sResult.append(XBinary::endianToString(XBinary::ENDIAN_BIG));
    }

    return sResult;
}

bool Binary_Script::isSigned()
{
    return g_bIsSigned;
}

void Binary_Script::_fixOffsetAndSize(qint64 *pnOffset, qint64 *pnSize)
{
    if ((*pnOffset) < g_nSize) {
        if ((*pnOffset) + (*pnSize) > g_nSize) {
            *pnSize = g_nSize - (*pnOffset);
        }
    }
}

XBinary::_MEMORY_MAP *Binary_Script::getMemoryMap()
{
    return &g_memoryMap;
}

XADDR Binary_Script::getBaseAddress()
{
    return g_nBaseAddress;
}

XBinary::PDSTRUCT *Binary_Script::getPdStruct()
{
    return g_pPdStruct;
}

QList<XArchive::RECORD> *Binary_Script::getArchiveRecords()
{
    return &g_listArchiveRecords;
}
