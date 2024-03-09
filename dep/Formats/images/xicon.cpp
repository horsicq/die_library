/* Copyright (c) 2022-2023 hors<horsicq@gmail.com>
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
#include "xicon.h"

XIcon::XIcon(QIODevice *pDevice) : XBinary(pDevice)
{
}

XIcon::~XIcon()
{
}

bool XIcon::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;
    // TODO more checks !!!
    if (getSize() > (qint64)(sizeof(ICONDIR) + sizeof(ICONDIRENTRY))) {
        ICONDIR iconDir = readICONDIR();

        if ((iconDir.idReserved == 0) && ((iconDir.idType == 1) || (iconDir.idType == 2)) && (iconDir.idCount > 0) && (iconDir.idCount < 100)) {
            ICONDIRENTRY iconDirectory = readICONDIRENTRY(sizeof(ICONDIR));

            if ((iconDirectory.bReserved == 0) && (iconDirectory.dwBytesInRes > 0)) {
                if (iconDir.idType == 1) {
                    if ((iconDirectory.wPlanes == 0) || (iconDirectory.wPlanes == 1)) {
                        bResult = true;
                    }
                } else if (iconDir.idType == 2) {
                    bResult = true;
                }
            }
        }
    }

    return bResult;
}

bool XIcon::isValid(QIODevice *pDevice)
{
    XIcon xicon(pDevice);

    return xicon.isValid();
}

XBinary::FT XIcon::getFileType()
{
    FT result = FT_ICO;

    quint32 nHeader = read_uint32(0);

    if (nHeader == 0x00010000) {
        result = FT_ICO;
    } else if (nHeader == 0x00020000) {
        result = FT_CUR;
    }

    return result;
}

QString XIcon::getFileFormatString()
{
    QString sResult;

    if (getFileType() == FT_CUR) {
        sResult = QString("CUR");
    } else {
        sResult = QString("ICO");
    }

    // TODO more info

    return sResult;
}

QString XIcon::getFileFormatExt()
{
    QString sResult;

    if (getFileType() == FT_CUR) {
        sResult = QString("cur");
    } else {
        sResult = QString("ico");
    }

    return sResult;
}

qint64 XIcon::getFileFormatSize()
{
    return _calculateRawSize();
}

XBinary::_MEMORY_MAP XIcon::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP result = {};

    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.type = MMT_HEADER;
        record.nOffset = 0;
        record.nSize = sizeof(ICONDIR);
        record.nAddress = -1;
        record.sName = tr("Header");

        result.listRecords.append(record);
    }

    ICONDIR iconDir = readICONDIR();

    qint64 nDataOffset = sizeof(ICONDIR) + (iconDir.idCount * sizeof(ICONDIRENTRY));

    bool bError = false;

    qint32 nNumberOfRecords = iconDir.idCount;

    qint64 nOffset = sizeof(ICONDIR);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        ICONDIRENTRY iconDirectory = readICONDIRENTRY(nOffset);

        if ((iconDirectory.dwBytesInRes == 0) || (iconDirectory.dwImageOffset < nDataOffset) || (iconDirectory.bReserved != 0)) {
            bError = true;
            break;
        }

        if (iconDir.idType == 1) {
            if ((iconDirectory.wPlanes != 0) && (iconDirectory.wPlanes != 1)) {
                bError = true;
                break;
            }
        }

        quint32 nHeader = read_uint32(iconDirectory.dwImageOffset);

        if ((nHeader != 0x00000028) && nHeader != (0x474e5089)) {  // PDF
            bError = true;
            break;
        }

        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.type = MMT_DATA;
        record.nOffset = iconDirectory.dwImageOffset;
        record.nSize = iconDirectory.dwBytesInRes;
        record.nAddress = -1;

        result.listRecords.append(record);

        nOffset += sizeof(ICONDIRENTRY);
    }

    if (bError) {
        result.listRecords.clear();
    }

    return result;
}

XIcon::ICONDIR XIcon::readICONDIR()
{
    ICONDIR result = {};

    result.idReserved = read_uint32(offsetof(ICONDIR, idReserved));
    result.idType = read_uint32(offsetof(ICONDIR, idType));
    result.idCount = read_uint32(offsetof(ICONDIR, idCount));

    return result;
}

XIcon::ICONDIRENTRY XIcon::readICONDIRENTRY(qint64 nOffset)
{
    ICONDIRENTRY result = {};

    result.bWidth = read_uint8(nOffset + offsetof(ICONDIRENTRY, bWidth));
    result.bHeight = read_uint8(nOffset + offsetof(ICONDIRENTRY, bHeight));
    result.bColorCount = read_uint8(nOffset + offsetof(ICONDIRENTRY, bColorCount));
    result.bReserved = read_uint8(nOffset + offsetof(ICONDIRENTRY, bReserved));
    result.wPlanes = read_uint16(nOffset + offsetof(ICONDIRENTRY, wPlanes));
    result.wBitCount = read_uint16(nOffset + offsetof(ICONDIRENTRY, wBitCount));
    result.dwBytesInRes = read_uint32(nOffset + offsetof(ICONDIRENTRY, dwBytesInRes));
    result.dwImageOffset = read_uint32(nOffset + offsetof(ICONDIRENTRY, dwImageOffset));

    return result;
}

XIcon::GRPICONDIRENTRY XIcon::readGPRICONDIRENTRY(qint64 nOffset)
{
    GRPICONDIRENTRY result = {};

    result.bWidth = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bWidth));
    result.bHeight = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bHeight));
    result.bColorCount = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bColorCount));
    result.bReserved = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bReserved));
    result.wPlanes = read_uint16(nOffset + offsetof(GRPICONDIRENTRY, wPlanes));
    result.wBitCount = read_uint16(nOffset + offsetof(GRPICONDIRENTRY, wBitCount));
    result.dwBytesInRes = read_uint32(nOffset + offsetof(GRPICONDIRENTRY, dwBytesInRes));
    result.nID = read_uint32(nOffset + offsetof(GRPICONDIRENTRY, nID));

    return result;
}

QList<XIcon::ICONDIRENTRY> XIcon::getIconDirectories()
{
    QList<XIcon::ICONDIRENTRY> listResult;

    ICONDIR iconDir = readICONDIR();

    qint32 nNumberOfRecords = iconDir.idCount;

    qint64 nOffset = sizeof(ICONDIR);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        ICONDIRENTRY record = readICONDIRENTRY(nOffset);

        if ((record.dwBytesInRes == 0) || (record.dwImageOffset == 0)) {
            break;
        }

        listResult.append(record);

        nOffset += sizeof(ICONDIRENTRY);
    }

    return listResult;
}

QList<XIcon::GRPICONDIRENTRY> XIcon::getIconGPRDirectories()
{
    QList<XIcon::GRPICONDIRENTRY> listResult;

    ICONDIR iconDir = readICONDIR();

    qint32 nNumberOfRecords = iconDir.idCount;

    qint64 nOffset = sizeof(ICONDIR);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        GRPICONDIRENTRY record = readGPRICONDIRENTRY(nOffset);

        if ((record.dwBytesInRes == 0) || (record.nID == 0)) {
            break;
        }

        listResult.append(record);

        nOffset += sizeof(GRPICONDIRENTRY);
    }

    return listResult;
}
