/* Copyright (c) 2022-2024 hors<horsicq@gmail.com>
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
#include "xgif.h"

XGif::XGif(QIODevice *pDevice) : XBinary(pDevice)
{
}

XGif::~XGif()
{
}

bool XGif::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() > 0x320) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        if (compareSignature(&memoryMap, "'GIF87a'", 0, pPdStruct) || compareSignature(&memoryMap, "'GIF89a'", 0, pPdStruct)) {
            // TODO more checks
            bResult = true;
        }
    }

    return bResult;
}

bool XGif::isValid(QIODevice *pDevice)
{
    XGif xgif(pDevice);

    return xgif.isValid();
}

QString XGif::getFileFormatString()
{
    QString sResult;

    sResult = "GIF";

    return sResult;
}

QString XGif::getFileFormatExt()
{
    return "gif";
}

qint64 XGif::getFileFormatSize()
{
    qint64 nResult = 0;

    qint64 nCurrentOffset = 0x320;

    while (true) {
        quint8 nBlockSize = read_uint8(nCurrentOffset);

        if (nBlockSize) {
            nCurrentOffset++;
            nCurrentOffset += nBlockSize;
        } else {
            break;
        }
    }

    nCurrentOffset++;

    if (read_uint8(nCurrentOffset) == 0x3B) {
        nCurrentOffset++;

        nResult = nCurrentOffset;
    }

    return nResult;
}

XBinary::_MEMORY_MAP XGif::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    return XBinary::getMemoryMap(mapMode, pPdStruct);
}

XBinary::FT XGif::getFileType()
{
    return FT_GIF;
}
