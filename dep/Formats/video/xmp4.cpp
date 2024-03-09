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
#include "xmp4.h"

XMP4::XMP4(QIODevice *pDevice) : XBinary(pDevice)
{
}

XMP4::~XMP4()
{
}

bool XMP4::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() > 0x20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        if (compareSignature(&memoryMap, "000000..'ftyp'", 0, pPdStruct)) {
            // TODO more checks !!!
            bResult = true;
        }
    }

    return bResult;
}

bool XMP4::isValid(QIODevice *pDevice)
{
    XMP4 mp4(pDevice);

    return mp4.isValid();
}

QString XMP4::getFileFormatString()
{
    QString sResult;

    sResult = "MP4";

    return sResult;
}

QString XMP4::getFileFormatExt()
{
    return "mp4";
}

qint64 XMP4::getFileFormatSize()
{
    return _calculateRawSize();
}

XBinary::_MEMORY_MAP XMP4::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};

    result.nBinarySize = getSize();

    qint32 nIndex = 0;
    qint64 nOffset = 0;

    while (!(pPdStruct->bIsStop)) {
        quint32 nChunkSize = read_uint32(nOffset, true);

        if (nChunkSize == 0) {
            break;
        }

        QString sTag = read_ansiString(nOffset + 4, 4);

        if (!isTagValid(sTag)) {
            break;
        }

        {
            _MEMORY_RECORD record = {};

            record.nIndex = nIndex++;
            record.type = MMT_FILESEGMENT;
            record.nOffset = nOffset;
            record.nSize = nChunkSize;
            record.nAddress = -1;
            record.sName = sTag;

            result.listRecords.append(record);
        }

        nOffset += nChunkSize;
    }

    return result;
}

XBinary::FT XMP4::getFileType()
{
    return FT_MP4;
}

bool XMP4::isTagValid(const QString &sTagName)
{
    bool bResult = false;

    if ((sTagName == "ftyp") || (sTagName == "pdin") || (sTagName == "moov") || (sTagName == "moof") || (sTagName == "mfra") || (sTagName == "mdat") ||
        (sTagName == "stts") || (sTagName == "stsc") || (sTagName == "stsz") || (sTagName == "meta") || (sTagName == "mvhd") || (sTagName == "trak") ||
        (sTagName == "udta") || (sTagName == "iods") || (sTagName == "uuid") || (sTagName == "free")) {
        bResult = true;
    } else {
#ifdef QT_DEBUG
        qDebug("%s", sTagName.toLatin1().data());
#endif
    }

    return bResult;
}
