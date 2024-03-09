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
#include "xtiff.h"

XTiff::XTiff(QIODevice *pDevice) : XBinary(pDevice)
{
}

XTiff::~XTiff()
{
}

bool XTiff::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 8) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap();
        if (compareSignature(&memoryMap, "'MM'002A") || compareSignature(&memoryMap, "'II'2A00")) {
            bool bIsBigEndian = isBigEndian();

            quint32 nOffset = read_uint32(4, bIsBigEndian);

            if ((nOffset > 0) && (nOffset < getSize())) {
                bIsValid = true;
            }
        }
    }

    return bIsValid;
}

bool XTiff::isValid(QIODevice *pDevice)
{
    XTiff xtiff(pDevice);

    return xtiff.isValid();
}

XBinary::_MEMORY_MAP XTiff::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint32 nIndex = 0;

    _MEMORY_MAP result = {};

    qint64 nTotalSize = getSize();

    result.nBinarySize = nTotalSize;
    result.endian = getEndian();

    bool bIsBigEndian = (result.endian == ENDIAN_BIG);

    if (result.nBinarySize > 8) {
        {
            _MEMORY_RECORD record = {};

            record.nIndex = nIndex++;
            record.type = MMT_HEADER;
            record.nOffset = 0;
            record.nSize = 8;
            record.nAddress = -1;
            record.sName = tr("Header");

            result.listRecords.append(record);
        }

        qint64 nTableOffset = read_uint32(4, bIsBigEndian);

        while (nTableOffset) {
            quint16 nTableCount = read_uint16(nTableOffset, bIsBigEndian);

            {
                _MEMORY_RECORD record = {};

                record.nIndex = nIndex++;
                record.type = MMT_TABLE;
                record.nOffset = nTableOffset;
                record.nSize = sizeof(quint16) + sizeof(IFD_ENTRY) * nTableCount;
                record.nAddress = -1;
                record.sName = tr("Table");

                result.listRecords.append(record);
            }

            qint64 nCurrentOffset = nTableOffset + sizeof(quint16);

            for (qint32 i = 0; i < nTableCount; i++) {
                quint16 nTag = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nTag), bIsBigEndian);
                quint16 nType = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nType), bIsBigEndian);
                quint32 nCount = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nCount), bIsBigEndian);

                qint32 nBaseTypeSize = getBaseTypeSize(nType);

                qint64 nDataSize = nBaseTypeSize * nCount;

                if (nDataSize > 4) {
                    quint32 nOffset = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nOffset), bIsBigEndian);

                    _MEMORY_RECORD record = {};

                    record.nIndex = nIndex++;
                    record.type = MMT_FILESEGMENT;
                    record.nOffset = nOffset;
                    record.nSize = nDataSize;
                    record.nAddress = -1;
                    record.sName = QString("%1-%2").arg(XBinary::valueToHex(nTag), XBinary::valueToHex(nType));

                    result.listRecords.append(record);
                }

                nCurrentOffset += sizeof(IFD_ENTRY);
            }

            nTableOffset = read_uint32(nCurrentOffset, bIsBigEndian);

            {
                _MEMORY_RECORD record = {};

                record.nIndex = nIndex++;
                record.type = MMT_DATA;
                record.nOffset = nCurrentOffset;
                record.nSize = sizeof(quint32);
                record.nAddress = -1;

                result.listRecords.append(record);
            }
        }
    }

    return result;
}

XBinary::FT XTiff::getFileType()
{
    return FT_TIFF;
}

QString XTiff::getFileFormatString()
{
    QString sResult;

    sResult = QString("TIFF");

    return sResult;
}

QString XTiff::getFileFormatExt()
{
    return "tiff";
}

qint64 XTiff::getFileFormatSize()
{
    return _calculateRawSize();
}

XBinary::ENDIAN XTiff::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint32 nData = read_uint32(0);

    if (nData == 0x4D4D002A) {
        result = ENDIAN_LITTLE;
    } else if (nData == 0x2A004D4D) {
        result = ENDIAN_BIG;
    }

    return result;
}

QList<XTiff::CHUNK> XTiff::getChunks(PDSTRUCT *pPdStruct)
{
    // Image
    // 0fe NewSubfileType LONG 1
    // 100 ImageWidth SHORT or LONG
    // 101 ImageLength SHORT or LONG
    // 102 BitsPerSample  SHORT 4 or 8
    // 103 Compression  SHORT 1, 2 or 32773
    // 106 PhotometricInterpretation  SHORT 0 or 1
    // 111 StripOffsets SHORT or LONG
    // 115 SamplesPerPixel SHORT
    // 116 RowsPerStrip  SHORT or LONG
    // 117 StripByteCounts LONG or SHORT
    // 11a XResolution RATIONAL
    // 11b YResolution RATIONAL
    // 11c PlanarConfiguration SHORT
    // 128 ResolutionUnit  SHORT 1, 2 or 3
    // 13d Predictor SHORT
    // 152 ExtraSamples SHORT 1

    // Exif
    // 10f Make ASCII
    // 110 Model ASCII
    // 112 Orientation SHORT 1
    // 131 Software ASCII
    // 132 DateTime ASCII
    // 213 YCbCrPositioning 1
    // 8769
    // 8825
    // 201 JPEGInterchangeFormat LONG 1
    // 202 JPEGInterchangeFormatLngth LONG 1
    // 213 YCbCrPositioning 1

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<XTiff::CHUNK> listResult;

    bool bIsBigEndian = isBigEndian();

    qint64 nTableOffset = read_uint32(4, bIsBigEndian);

    while ((nTableOffset) && (!(pPdStruct->bIsStop))) {
        quint16 nTableCount = read_uint16(nTableOffset, bIsBigEndian);

        qint64 nCurrentOffset = nTableOffset + sizeof(quint16);

        for (qint32 i = 0; i < nTableCount; i++) {
            XTiff::CHUNK record = {};

            quint16 nTag = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nTag), bIsBigEndian);
            quint16 nType = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nType), bIsBigEndian);
            quint32 nCount = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nCount), bIsBigEndian);

            qint32 nBaseTypeSize = getBaseTypeSize(nType);

            qint64 nDataSize = nBaseTypeSize * nCount;

            if (nDataSize > 4) {
                record.nOffset = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nOffset), bIsBigEndian);
            } else {
                record.nOffset = nCurrentOffset + offsetof(IFD_ENTRY, nOffset);
            }

            record.nSize = nDataSize;
            record.nTag = nTag;

            nCurrentOffset += sizeof(IFD_ENTRY);

            listResult.append(record);
        }

        qint64 nTempTableOffset = read_uint32(nCurrentOffset, bIsBigEndian);
        if (nTempTableOffset < (qint64)(nTableOffset + sizeof(quint16) + nTableCount * sizeof(IFD_ENTRY) + sizeof(quint64))) {
            break;
        } else {
            nTableOffset = nTempTableOffset;
        }
    }

    return listResult;
}

QList<XTiff::CHUNK> XTiff::_getChunksByTag(QList<CHUNK> *pListChunks, quint16 nTag)
{
    QList<XTiff::CHUNK> listResult;

    qint32 nNumberOfRecords = pListChunks->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListChunks->at(i).nTag == nTag) {
            listResult.append(pListChunks->at(i));
        }
    }

    return listResult;
}

QString XTiff::getCameraName(QList<CHUNK> *pListChunks)
{
    QString sResult;

    QList<CHUNK> listMake = _getChunksByTag(pListChunks, 0x10f);
    QList<CHUNK> listModel = _getChunksByTag(pListChunks, 0x110);

    QString sMake;
    QString sModel;

    if (listMake.count()) {
        sMake = read_ansiString(listMake.at(0).nOffset, listMake.at(0).nSize);
    }

    if (listModel.count()) {
        sModel = read_ansiString(listModel.at(0).nOffset, listModel.at(0).nSize);
    }

    if ((sMake != "") || (sModel != "")) {
        sResult = QString("%1(%2)").arg(sMake, sModel);
    }

    return sResult;
}

qint32 XTiff::getBaseTypeSize(quint16 nType)
{
    // 1 = BYTE 8-bit unsigned integer.
    // 2 = ASCII 8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL (binary zero).
    // 3 = SHORT 16-bit (2-byte) unsigned integer.
    // 4 = LONG 32-bit (4-byte) unsigned integer.
    // 5 = RATIONAL Two LONGs: the first represents the numerator of a fraction; the second, the denominator
    // 6 = SBYTE An 8-bit signed (twos-complement) integer.
    // 7 = UNDEFINED An 8-bit byte that may contain anything, depending on the definition of the field.
    // 8 = SSHORT A 16-bit (2-byte) signed (twos-complement) integer.
    // 9 = SLONG A 32-bit (4-byte) signed (twos-complement) integer.
    // 10 = SRATIONAL Two SLONG’s: the first represents the numerator of a fraction, the second the denominator.
    // 11 = FLOAT Single precision (4-byte) IEEE format.
    // 12 = DOUBLE Double precision (8-byte) IEEE format.

    qint32 nResult = 0;

    switch (nType) {
        case 1: nResult = 1; break;
        case 2: nResult = 1; break;
        case 3: nResult = 2; break;
        case 4: nResult = 4; break;
        case 5: nResult = 8; break;
        case 6: nResult = 1; break;
        case 7: nResult = 1; break;
        case 8: nResult = 2; break;
        case 9: nResult = 4; break;
        case 10: nResult = 8; break;
        case 11: nResult = 4; break;
        case 12: nResult = 8; break;
        default: nResult = 0;
    }

    return nResult;
}
