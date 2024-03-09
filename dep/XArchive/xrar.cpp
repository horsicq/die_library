/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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
#include "xrar.h"

XRar::XRar(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool XRar::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() > 20)  // TODO
    {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        if (compareSignature(&memoryMap, "'RE~^'", 0, pPdStruct) || compareSignature(&memoryMap, "'Rar!'1A07", 0, pPdStruct)) {
            bResult = true;
        }
    }

    return bResult;
}

bool XRar::isValid(QIODevice *pDevice)
{
    XRar xrar(pDevice);

    return xrar.isValid();
}

QString XRar::getVersion()
{
    QString sResult;

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap();

    // TODO more
    if (compareSignature(&memoryMap, "'RE~^'")) {
        sResult = "1.4";
    } else if (compareSignature(&memoryMap, "'Rar!'1A0700")) {
        sResult = "4.X";
    } else if (compareSignature(&memoryMap, "'Rar!'1A070100")) {
        sResult = "5.X";
    }

    return sResult;
}

quint64 XRar::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return 0;  // TODO
}

QList<XArchive::RECORD> XRar::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    Q_UNUSED(pPdStruct)

    QList<XArchive::RECORD> listResult;

    // TODO

    return listResult;
}

QString XRar::getFileFormatExt()
{
    return "rar";
}

qint64 XRar::getFileFormatSize()
{
    // TODO
    return XBinary::getFileFormatSize();
}

QString XRar::getFileFormatString()
{
    QString sResult;

    sResult = QString("RAR(%1)").arg(getVersion());
    // TODO more info

    return sResult;
}

QList<XBinary::MAPMODE> XRar::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XRar::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);  // TODO rewrite

    result.nBinarySize = getSize();

    qint64 nFileHeaderSize = 0;

    if (compareSignature(&memoryMap, "'Rar!'1A0700")) {
        nFileHeaderSize = 7;
    } else if (compareSignature(&memoryMap, "'Rar!'1A070100")) {
        nFileHeaderSize = 8;
    }

    if (nFileHeaderSize) {
        qint32 nIndex = 0;

        {
            _MEMORY_RECORD record = {};

            record.nIndex = nIndex++;
            record.type = MMT_HEADER;
            record.nOffset = 0;
            record.nSize = nFileHeaderSize;
            record.nAddress = -1;
            record.sName = tr("Header");

            result.listRecords.append(record);
        }

        qint64 nCurrentOffset = nFileHeaderSize;

        while (true) {
            GENERICHEADER5 genericHeader = XRar::readGenericHeader5(nCurrentOffset);

            if ((genericHeader.nType > 0) && (genericHeader.nType <= 5)) {
                _MEMORY_RECORD record = {};

                record.nIndex = nIndex++;
                record.type = MMT_DATA;
                record.nOffset = nCurrentOffset;
                record.nSize = genericHeader.nSize;
                record.nAddress = -1;
                record.sName = tr("Data");

                result.listRecords.append(record);

                nCurrentOffset += genericHeader.nSize;
            } else {
                break;
            }

            if (genericHeader.nType == 5) {  // END
                break;
            }
        }

        // TODO Overlay
    }

    return result;
}

XBinary::FT XRar::getFileType()
{
    return FT_RAR;
}

XRar::GENERICHEADER5 XRar::readGenericHeader5(qint64 nOffset)
{
    GENERICHEADER5 result = {};

    qint64 nCurrentOffset = nOffset;
    PACKED_INT packeInt = {};
    qint32 nByteSize = 0;

    result.nCRC32 = read_uint32(nCurrentOffset);
    nCurrentOffset += 4;
    packeInt = read_uleb128(nCurrentOffset, 4);
    result.nHeaderSize = packeInt.nValue;
    nCurrentOffset += packeInt.nByteSize;
    nByteSize = packeInt.nByteSize;
    packeInt = read_uleb128(nCurrentOffset, 4);
    result.nType = packeInt.nValue;
    nCurrentOffset += packeInt.nByteSize;
    packeInt = read_uleb128(nCurrentOffset, 4);
    result.nFlags = packeInt.nValue;
    nCurrentOffset += packeInt.nByteSize;

    if (result.nFlags & 0x0001) {
        packeInt = read_uleb128(nCurrentOffset, 4);
        result.nExtraAreaSize = packeInt.nValue;
        nCurrentOffset += packeInt.nByteSize;
    }

    if (result.nFlags & 0x0002) {
        packeInt = read_uleb128(nCurrentOffset, 8);
        result.nDataSize = packeInt.nValue;
        nCurrentOffset += packeInt.nByteSize;
    }

    result.nSize = 4 + nByteSize + result.nHeaderSize + result.nDataSize;

    return result;
}
