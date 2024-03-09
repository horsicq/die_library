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
#include "xjpeg.h"

XJpeg::XJpeg(QIODevice *pDevice) : XBinary(pDevice)
{
}

XJpeg::~XJpeg()
{
}

bool XJpeg::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);
        bIsValid = compareSignature(&memoryMap, "FFD8FFE0....'JFIF'00", 0, pPdStruct) || compareSignature(&memoryMap, "FFD8FFE1....'Exif'00", 0, pPdStruct) ||
                   compareSignature(&memoryMap, "FFD8FFDB", 0, pPdStruct);
    }

    return bIsValid;
}

bool XJpeg::isValid(QIODevice *pDevice)
{
    XJpeg xjpeg(pDevice);

    return xjpeg.isValid();
}

XBinary::_MEMORY_MAP XJpeg::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};
    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    QList<XJpeg::CHUNK> listChunks = getChunks(pPdStruct);

    qint32 nNumberOfChunks = listChunks.count();

    for (qint32 i = 0; (i < nNumberOfChunks) && (!(pPdStruct->bIsStop)); i++) {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;

        if (listChunks.at(i).bEntropyCodedData) {
            record.type = MMT_DATA;
            record.sName = tr("Data");
        } else {
            record.type = MMT_OBJECT;
            record.sName = XBinary::valueToHex(listChunks.at(i).nId);
        }

        record.nOffset = listChunks.at(i).nDataOffset;
        record.nSize = listChunks.at(i).nDataSize;
        record.nAddress = -1;

        result.listRecords.append(record);
    }

    return result;
}

XBinary::FT XJpeg::getFileType()
{
    return FT_JPEG;
}

QString XJpeg::getFileFormatString()
{
    QString sResult;

    sResult = QString("JPEG");

    return sResult;
}

QString XJpeg::getFileFormatExt()
{
    return "jpeg";
}

qint64 XJpeg::getFileFormatSize()
{
    return _calculateRawSize();
}

QList<XJpeg::CHUNK> XJpeg::getChunks(PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<CHUNK> listResult;

    qint64 nOffset = 0;

    while (!(pPdStruct->bIsStop)) {
        if (nOffset == -1) {
            break;
        }

        CHUNK chunk = _readChunk(nOffset);

        if (!chunk.bValid) {
            break;
        }

        listResult.append(chunk);

        nOffset = chunk.nDataOffset + chunk.nDataSize;

        if (chunk.nId == 0xDA) {
            CHUNK chunkData = {};
            chunkData.bValid = true;
            chunkData.bEntropyCodedData = true;
            chunkData.nDataOffset = nOffset;

            while (true) {
                nOffset = find_uint8(nOffset, -1, 0xFF, pPdStruct);  // TODO PDStruct;

                if (nOffset == -1) {
                    break;
                }

                if (read_uint8(nOffset + 1) != 0x00) {
                    break;
                } else {
                    nOffset++;
                }
            }

            chunkData.nDataSize = nOffset - chunkData.nDataOffset;

            listResult.append(chunkData);
        }

        if (chunk.nId == 0xD9) {  // END of image
            break;
        }
    }

    return listResult;
}

QList<XJpeg::CHUNK> XJpeg::_getChunksById(QList<CHUNK> *pListChunks, quint8 nId)
{
    QList<XJpeg::CHUNK> listResult;

    qint32 nNumberOfRecords = pListChunks->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListChunks->at(i).nId == nId) {
            listResult.append(pListChunks->at(i));
        }
    }

    return listResult;
}

QString XJpeg::getComment(QList<CHUNK> *pListChunks)
{
    QString sResult;

    QList<XJpeg::CHUNK> listComments = _getChunksById(pListChunks, 0xFE);  // COMMENT

    qint32 nNumberOfRecords = listComments.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        sResult += read_ansiString(listComments.at(i).nDataOffset + 4, listComments.at(i).nDataSize - 4);
    }

    if (sResult.size() > 100) {
        sResult.resize(100);
    }

    sResult = sResult.remove("\r").remove("\n");

    return sResult;
}

QString XJpeg::getComment()
{
    QList<CHUNK> listChunks = getChunks();

    return getComment(&listChunks);
}

QString XJpeg::getDqtMD5(QList<CHUNK> *pListChunks)
{
    QString sResult;

    QList<XJpeg::CHUNK> listComments = _getChunksById(pListChunks, 0xDB);  // DQT

    qint32 nNumberOfRecords = listComments.count();

    QCryptographicHash crypto(QCryptographicHash::Md5);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        QByteArray baData = read_array(listComments.at(i).nDataOffset + 4, listComments.at(i).nDataSize - 4);

        crypto.addData(baData);
    }

    sResult = crypto.result().toHex();

    return sResult;
}

QString XJpeg::getDqtMD5()
{
    QList<CHUNK> listChunks = getChunks();

    return getDqtMD5(&listChunks);
}

bool XJpeg::isChunkPresent(QList<CHUNK> *pListChunks, quint8 nId)
{
    bool bResult = false;

    qint32 nNumberOfRecords = pListChunks->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListChunks->at(i).nId == nId) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XBinary::OFFSETSIZE XJpeg::getExif(QList<CHUNK> *pListChunks)
{
    OFFSETSIZE result = {};

    QList<CHUNK> listExif = _getChunksById(pListChunks, 0xE1);

    if (listExif.count() > 0) {
        CHUNK chunkExif = listExif.at(0);

        if (chunkExif.nDataSize > 10) {
            if (read_ansiString(chunkExif.nDataOffset + 4) == "Exif") {
                result.nOffset = chunkExif.nDataOffset + 10;
                result.nSize = chunkExif.nDataSize - 10;
            }
        }
    }

    return result;
}

bool XJpeg::isExifPresent(OFFSETSIZE osExif)
{
    return osExif.nSize;
}

QList<XTiff::CHUNK> XJpeg::getExifChunks(OFFSETSIZE osExif, PDSTRUCT *pPdStruct)
{
    QList<XTiff::CHUNK> listResult;

    if (osExif.nSize) {
        SubDevice sd(getDevice(), osExif.nOffset, osExif.nSize);

        if (sd.open(QIODevice::ReadOnly)) {
            XTiff tiff(&sd);

            if (tiff.isValid()) {
                listResult = tiff.getChunks(pPdStruct);
            }

            sd.close();
        }
    }

    return listResult;
}

QString XJpeg::getExifCameraName(OFFSETSIZE osExif, QList<XTiff::CHUNK> *pListExifChunks)
{
    QString sResult;

    if (osExif.nSize) {
        SubDevice sd(getDevice(), osExif.nOffset, osExif.nSize);

        if (sd.open(QIODevice::ReadOnly)) {
            XTiff tiff(&sd);

            if (tiff.isValid()) {
                sResult = tiff.getCameraName(pListExifChunks);
            }

            sd.close();
        }
    }

    return sResult;
}

XJpeg::CHUNK XJpeg::_readChunk(qint64 nOffset)
{
    CHUNK result = {};

    quint8 nBegin = read_uint8(nOffset);

    if (nBegin == 0xFF) {
        result.bValid = true;
        result.nId = read_uint8(nOffset + 1);

        result.nDataOffset = nOffset;

        if ((result.nId == 0xD8) || (result.nId == 0xD9)) {
            result.nDataSize = 2;
        } else if (result.nId == 0xDD) {
            result.nDataSize = 2 + 4;
        } else if ((result.nId >= 0xD0) && (result.nId <= 0xD7)) {
            result.nDataSize = 2;
        } else if (result.nId != 0x00) {
            result.nDataSize = 2 + read_uint16(nOffset + 2, true);
        } else {
            result.bValid = false;
        }
    }

    return result;
}
