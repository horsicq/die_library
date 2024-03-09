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
#include "xpng.h"

XPNG::XPNG(QIODevice *pDevice) : XBinary(pDevice)
{
}

XPNG::~XPNG()
{
}

bool XPNG::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        bIsValid = compareSignature(&memoryMap, "89'PNG\r\n'1A0A", 0, pPdStruct);
    }

    return bIsValid;
}

bool XPNG::isValid(QIODevice *pDevice)
{
    XPNG xpng(pDevice);

    return xpng.isValid();
}

XBinary::FT XPNG::getFileType()
{
    return FT_PNG;
}

QString XPNG::getFileFormatString()
{
    QString sResult;

    QString sResolution = QString("%1x%2").arg(read_uint32(16, true)).arg(read_uint32(20, true));

    sResult = QString("PNG(%1)").arg(sResolution);

    return sResult;
}

QString XPNG::getFileFormatExt()
{
    return "png";
}

qint64 XPNG::getFileFormatSize()
{
    qint64 nResult = 0;

    qint64 nOffset = 8;

    while (true) {
        CHUNK chunk = _readChunk(nOffset);

        nOffset += (12 + chunk.nDataSize);

        if (chunk.sName == "IEND") {
            nResult = nOffset;

            break;
        }

        if (chunk.nCRC == 0) {  // mb TODO more checks mb ANSI names
            break;
        }
    }

    return nResult;
}

XBinary::_MEMORY_MAP XPNG::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};

    qint32 nIndex = 0;

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

    qint64 nOffset = 8;

    while (!(pPdStruct->bIsStop)) {
        CHUNK chunk = _readChunk(nOffset);

        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;

        record.type = MMT_OBJECT;

        record.nOffset = nOffset;
        record.nSize = 12 + chunk.nDataSize;
        record.nAddress = -1;
        record.sName = chunk.sName;

        result.listRecords.append(record);

        nOffset += (12 + chunk.nDataSize);

        if (chunk.sName == "IEND") {
            break;
        }

        if (chunk.nCRC == 0) {  // mb TODO more checks mb ANSI names
            break;
        }
    }

    return result;
}

XPNG::CHUNK XPNG::_readChunk(qint64 nOffset)
{
    CHUNK result = {};

    result.nDataSize = read_uint32(nOffset, true);
    result.nDataOffset = nOffset + 8;
    result.sName = read_ansiString(nOffset + 4, 4);
    result.nCRC = read_uint32(nOffset + 8 + result.nDataSize, true);

    return result;
}
