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
#include "xbmp.h"

XBMP::XBMP(QIODevice *pDevice) : XBinary(pDevice)
{
}

XBMP::~XBMP()
{
}

bool XBMP::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap();

    if (compareSignature(&memoryMap, "'BM'..................000000", 0)) {
        quint32 nSize = read_uint32(2);

        if ((nSize > 0) && (nSize <= getSize())) {
            quint32 nDBISize = read_uint32(0x0E);

            if (nDBISize == 40) {
                bResult = true;
            } else if (nDBISize == 108) {
                bResult = true;
            } else if (nDBISize == 124) {
                bResult = true;
            }
        }
    }

    // TODO
    //    BM Windows 3.1x, 95, NT, ... etc.
    //    BA OS/2 struct bitmap array
    //    CI OS/2 struct color icon
    //    CP OS/2 const color pointer
    //    IC OS/2 struct icon
    //    PT OS/2 pointer

    return bResult;
}

bool XBMP::isValid(QIODevice *pDevice)
{
    XBMP xbmp(pDevice);

    return xbmp.isValid();
}

XBinary::FT XBMP::getFileType()
{
    return FT_BMP;
}

QString XBMP::getFileFormatExt()
{
    return "bmp";
}

qint64 XBMP::getFileFormatSize()
{
    qint64 nResult = 0;

    nResult = read_uint32(2);

    return nResult;
}

XBinary::_MEMORY_MAP XBMP::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    // TODO
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    return XBinary::getMemoryMap(mapMode, pPdStruct);
}

QString XBMP::getFileFormatString()
{
    QString sResult;

    // TODO
    sResult = "BMP";

    return sResult;
}
