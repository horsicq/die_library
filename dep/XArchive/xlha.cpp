/* Copyright (c) 2023 hors<horsicq@gmail.com>
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
#include "xlha.h"

XLHA::XLHA(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool XLHA::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() >= 12) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        if (compareSignature(&memoryMap, "....'-lh'..2d", 0, pPdStruct) || compareSignature(&memoryMap, "....'-lz'..2d", 0, pPdStruct)) {
            QString sMethod = read_ansiString(2, 5);

            if ((sMethod == "-lz4-") || (sMethod == "-lz5-") || (sMethod == "-lzs-") || (sMethod == "-lh0-") || (sMethod == "-lh1-") || (sMethod == "-lh4-") ||
                (sMethod == "-lh5-") || (sMethod == "-lh6-") || (sMethod == "-lh7-") || (sMethod == "-lhx-") || (sMethod == "-pm0-") || (sMethod == "-pm1-") ||
                (sMethod == "-pm2-")) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XLHA::isValid(QIODevice *pDevice)
{
    XLHA xhla(pDevice);

    return xhla.isValid();
}

quint64 XLHA::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    return getRecords(-1, pPdStruct).count();
}

QList<XArchive::RECORD> XLHA::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listResult;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap();

    qint64 nFileSize = getSize();

    qint64 nOffset = 0;

    qint32 nNumberOfFiles = 0;

    while ((nFileSize > 0) && (!(pPdStruct->bIsStop))) {
        if (compareSignature(&memoryMap, "....'-lh'..2d", nOffset) || compareSignature(&memoryMap, "....'-lz'..2d", nOffset)) {
            qint64 nHeaderSize = read_uint8(nOffset) + 2;
            qint64 nCompressedSize = read_uint32(nOffset + 7);
            qint64 nUncompressedSize = read_uint32(nOffset + 11);
            QString sFileName = read_ansiString(nOffset + 22, read_uint8(nOffset + 21));
            sFileName = sFileName.replace("\\", "/");

            if (nHeaderSize < 21) {
                break;
            }

            XArchive::RECORD record = {};
            // TODO CRC
            record.compressMethod = COMPRESS_METHOD_LZH5;

            QString sMethod = read_ansiString(nOffset + 2, 5);

            if (sMethod == "-lh5-") {
                record.compressMethod = COMPRESS_METHOD_LZH5;
            } else if (sMethod == "-lh6-") {
                record.compressMethod = COMPRESS_METHOD_LZH6;
            } else if (sMethod == "-lh7-") {
                record.compressMethod = COMPRESS_METHOD_LZH7;
            }

            record.nHeaderOffset = nOffset;
            record.nDataOffset = nOffset + nHeaderSize;
            record.nUncompressedSize = nUncompressedSize;
            record.nHeaderSize = nHeaderSize;
            record.nCompressedSize = nCompressedSize;
            record.sFileName = sFileName;

            listResult.append(record);

            nNumberOfFiles++;

            if (nLimit != -1) {
                if (nNumberOfFiles > nLimit) {
                    break;
                }
            }

            nOffset += (nHeaderSize + nCompressedSize);
            nFileSize -= (nHeaderSize + nCompressedSize);
        } else {
            break;
        }
    }

    return listResult;
}

qint64 XLHA::getFileFormatSize()
{
    return _calculateRawSize();
}

QList<XBinary::MAPMODE> XLHA::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XLHA::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap();

    _MEMORY_MAP result = {};

    qint64 nFileSize = getSize();

    qint64 nOffset = 0;

    while ((nFileSize > 0) && (!(pPdStruct->bIsStop))) {
        if (compareSignature(&memoryMap, "....'-lh'..2d", nOffset) || compareSignature(&memoryMap, "....'-lz'..2d", nOffset)) {
            qint64 nHeaderSize = read_uint8(nOffset) + 2;
            qint64 nDataSize = read_uint32(nOffset + 7);
            QString sFileName = read_ansiString(nOffset + 22, read_uint8(nOffset + 21));

            if (nHeaderSize < 21) {
                break;
            }

            {
                _MEMORY_RECORD memoryRecord = {};

                memoryRecord.nOffset = nOffset;
                memoryRecord.nAddress = -1;
                memoryRecord.nSize = nHeaderSize;
                memoryRecord.sName = tr("Header");
                memoryRecord.type = MMT_HEADER;

                result.listRecords.append(memoryRecord);
            }

            {
                _MEMORY_RECORD memoryRecord = {};

                memoryRecord.nOffset = nOffset + nHeaderSize;
                memoryRecord.nAddress = -1;
                memoryRecord.nSize = nDataSize;
                memoryRecord.sName = sFileName;
                memoryRecord.type = MMT_FILESEGMENT;

                result.listRecords.append(memoryRecord);
            }

            nOffset += (nHeaderSize + nDataSize);
            nFileSize -= (nHeaderSize + nDataSize);
        } else {
            break;
        }
    }

    result.nBinarySize = nOffset;

    return result;
}

XBinary::FT XLHA::getFileType()
{
    return FT_LHA;
}

QString XLHA::getFileFormatString()
{
    QString sResult;

    sResult = QString("LHA");
    // TODO more info

    return sResult;
}

QString XLHA::getFileFormatExt()
{
    return "lha";
}
