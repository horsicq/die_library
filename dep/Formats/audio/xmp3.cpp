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
#include "xmp3.h"

XMP3::XMP3(QIODevice *pDevice) : XBinary(pDevice)
{
}

XMP3::~XMP3()
{
}

bool XMP3::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() > 0x20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        if (compareSignature(&memoryMap, "'ID3'0200", 0, pPdStruct) || compareSignature(&memoryMap, "'ID3'0300", 0, pPdStruct) ||
            compareSignature(&memoryMap, "'ID3'0400", 0, pPdStruct)) {
            // TODO more checks
            bResult = true;
        }
    }

    return bResult;
}

bool XMP3::isValid(QIODevice *pDevice)
{
    XMP3 mp3(pDevice);

    return mp3.isValid();
}

QString XMP3::getFileFormatString()
{
    QString sResult;

    sResult = QString("MP3(%1)").arg(getVersion());  // TODO more

    return sResult;
}

QString XMP3::getFileFormatExt()
{
    return "mp3";
}

qint64 XMP3::getFileFormatSize()
{
    return _calculateRawSize();
}

XBinary::_MEMORY_MAP XMP3::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};

    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    _MEMORY_MAP _memoryMap = XBinary::getMemoryMap();

    if (compareSignature(&_memoryMap, "'ID3'..00", 0)) {
        quint32 nVar[4] = {};
        nVar[0] = read_uint8(6);
        nVar[1] = read_uint8(7);
        nVar[2] = read_uint8(8);
        nVar[3] = read_uint8(9);

        qint64 nOffset = 10 + ((nVar[0] << 21) | (nVar[1] << 14) | (nVar[2] << 7) | (nVar[3]));

        {
            _MEMORY_RECORD record = {};

            record.nIndex = nIndex++;
            record.type = MMT_HEADER;
            record.nOffset = 0;
            record.nSize = nOffset;
            record.nAddress = -1;
            record.sName = tr("Header");

            result.listRecords.append(record);
        }
        // TODO
        quint32 nFrameSize = 0;

        do {
            nFrameSize = decodeFrame(nOffset);

            if (nFrameSize) {
                _MEMORY_RECORD record = {};

                record.nIndex = nIndex++;
                record.type = MMT_FILESEGMENT;
                record.nOffset = nOffset;
                record.nSize = nFrameSize;
                record.nAddress = -1;
                record.sName = QString("Frame");  // mb translate

                result.listRecords.append(record);

                nOffset += nFrameSize;
            }
        } while (nFrameSize && (!(pPdStruct->bIsStop)));

        // TODO TAG
    }

    return result;
}

XBinary::FT XMP3::getFileType()
{
    return FT_MP3;
}

QString XMP3::getVersion()
{
    QString sResult;

    if (getSize() > 0x20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap();

        if (compareSignature(&memoryMap, "'ID3'0200", 0)) {
            sResult = "3.2";
        } else if (compareSignature(&memoryMap, "'ID3'0300", 0)) {
            sResult = "3.3";
        } else if (compareSignature(&memoryMap, "'ID3'0400", 0)) {
            sResult = "3.4";
        }
    }

    return sResult;
}

qint64 XMP3::decodeFrame(qint64 nOffset)
{
    qint64 nResult = 0;

    quint32 nHeader = read_uint32(nOffset, true);

    if (nHeader & 0xFFE00000) {
        quint8 nVersion = (nHeader >> 19) & 0x3;
        quint8 nLayer = (nHeader >> 17) & 0x3;
        quint8 nErrorProtection = (nHeader >> 16) & 0x1;
        quint8 nBitRateIndex = (nHeader >> 12) & 0xF;
        quint8 nFrequencyIndex = (nHeader >> 10) & 0x3;
        quint8 nPadBit = (nHeader >> 9) & 0x1;
        quint8 nPrivBit = (nHeader >> 8) & 0x1;
        quint8 nMode = (nHeader >> 7) & 0x3;
        quint8 nIntensiveStereo = (nHeader >> 5) & 0x1;
        quint8 nMSStereo = (nHeader >> 4) & 0x1;
        quint8 nCopy = (nHeader >> 3) & 0x1;
        quint8 nOriginal = (nHeader >> 2) & 0x1;
        quint8 nEmphasis = (nHeader >> 0) & 0x3;

        Q_UNUSED(nPrivBit)
        Q_UNUSED(nMode)
        Q_UNUSED(nIntensiveStereo)
        Q_UNUSED(nErrorProtection)
        Q_UNUSED(nMSStereo)
        Q_UNUSED(nCopy)
        Q_UNUSED(nOriginal)
        Q_UNUSED(nEmphasis)

        quint32 nBitRate = 0;
        quint32 nFrequency = 0;
        double dTime = 0;

        if ((nVersion == 1) || (nVersion == 2)) {
            if ((nLayer == 1) || (nLayer == 2)) {  // III & II
                if (nBitRateIndex == 1) {
                    nBitRate = 8;
                } else if (nBitRateIndex == 2) {
                    nBitRate = 16;
                } else if (nBitRateIndex == 3) {
                    nBitRate = 24;
                } else if (nBitRateIndex == 4) {
                    nBitRate = 32;
                } else if (nBitRateIndex == 5) {
                    nBitRate = 40;
                } else if (nBitRateIndex == 6) {
                    nBitRate = 48;
                } else if (nBitRateIndex == 7) {
                    nBitRate = 56;
                } else if (nBitRateIndex == 8) {
                    nBitRate = 64;
                } else if (nBitRateIndex == 9) {
                    nBitRate = 80;
                } else if (nBitRateIndex == 10) {
                    nBitRate = 96;
                } else if (nBitRateIndex == 11) {
                    nBitRate = 112;
                } else if (nBitRateIndex == 12) {
                    nBitRate = 128;
                } else if (nBitRateIndex == 13) {
                    nBitRate = 144;
                } else if (nBitRateIndex == 14) {
                    nBitRate = 160;
                }
            } else if (nLayer == 3) {  // I
                if (nBitRateIndex == 1) {
                    nBitRate = 32;
                } else if (nBitRateIndex == 2) {
                    nBitRate = 48;
                } else if (nBitRateIndex == 3) {
                    nBitRate = 56;
                } else if (nBitRateIndex == 4) {
                    nBitRate = 64;
                } else if (nBitRateIndex == 5) {
                    nBitRate = 80;
                } else if (nBitRateIndex == 6) {
                    nBitRate = 96;
                } else if (nBitRateIndex == 7) {
                    nBitRate = 112;
                } else if (nBitRateIndex == 8) {
                    nBitRate = 128;
                } else if (nBitRateIndex == 9) {
                    nBitRate = 144;
                } else if (nBitRateIndex == 10) {
                    nBitRate = 160;
                } else if (nBitRateIndex == 11) {
                    nBitRate = 176;
                } else if (nBitRateIndex == 12) {
                    nBitRate = 192;
                } else if (nBitRateIndex == 13) {
                    nBitRate = 224;
                } else if (nBitRateIndex == 14) {
                    nBitRate = 256;
                }
            }
        } else if (nVersion == 3) {
            if (nLayer == 1) {  // III
                if (nBitRateIndex == 1) {
                    nBitRate = 32;
                } else if (nBitRateIndex == 2) {
                    nBitRate = 40;
                } else if (nBitRateIndex == 3) {
                    nBitRate = 48;
                } else if (nBitRateIndex == 4) {
                    nBitRate = 56;
                } else if (nBitRateIndex == 5) {
                    nBitRate = 64;
                } else if (nBitRateIndex == 6) {
                    nBitRate = 80;
                } else if (nBitRateIndex == 7) {
                    nBitRate = 96;
                } else if (nBitRateIndex == 8) {
                    nBitRate = 112;
                } else if (nBitRateIndex == 9) {
                    nBitRate = 128;
                } else if (nBitRateIndex == 10) {
                    nBitRate = 160;
                } else if (nBitRateIndex == 11) {
                    nBitRate = 192;
                } else if (nBitRateIndex == 12) {
                    nBitRate = 224;
                } else if (nBitRateIndex == 13) {
                    nBitRate = 256;
                } else if (nBitRateIndex == 14) {
                    nBitRate = 320;
                }
            } else if (nLayer == 2) {  // II
                if (nBitRateIndex == 1) {
                    nBitRate = 32;
                } else if (nBitRateIndex == 2) {
                    nBitRate = 48;
                } else if (nBitRateIndex == 3) {
                    nBitRate = 56;
                } else if (nBitRateIndex == 4) {
                    nBitRate = 64;
                } else if (nBitRateIndex == 5) {
                    nBitRate = 80;
                } else if (nBitRateIndex == 6) {
                    nBitRate = 96;
                } else if (nBitRateIndex == 7) {
                    nBitRate = 112;
                } else if (nBitRateIndex == 8) {
                    nBitRate = 128;
                } else if (nBitRateIndex == 9) {
                    nBitRate = 160;
                } else if (nBitRateIndex == 10) {
                    nBitRate = 192;
                } else if (nBitRateIndex == 11) {
                    nBitRate = 224;
                } else if (nBitRateIndex == 12) {
                    nBitRate = 256;
                } else if (nBitRateIndex == 13) {
                    nBitRate = 320;
                } else if (nBitRateIndex == 14) {
                    nBitRate = 384;
                }
            } else if (nLayer == 3) {  // I
                if (nBitRateIndex == 1) {
                    nBitRate = 32;
                } else if (nBitRateIndex == 2) {
                    nBitRate = 64;
                } else if (nBitRateIndex == 3) {
                    nBitRate = 96;
                } else if (nBitRateIndex == 4) {
                    nBitRate = 128;
                } else if (nBitRateIndex == 5) {
                    nBitRate = 160;
                } else if (nBitRateIndex == 6) {
                    nBitRate = 192;
                } else if (nBitRateIndex == 7) {
                    nBitRate = 224;
                } else if (nBitRateIndex == 8) {
                    nBitRate = 256;
                } else if (nBitRateIndex == 9) {
                    nBitRate = 288;
                } else if (nBitRateIndex == 10) {
                    nBitRate = 320;
                } else if (nBitRateIndex == 11) {
                    nBitRate = 352;
                } else if (nBitRateIndex == 12) {
                    nBitRate = 384;
                } else if (nBitRateIndex == 13) {
                    nBitRate = 416;
                } else if (nBitRateIndex == 14) {
                    nBitRate = 448;
                }
            }
        }

        if (nVersion == 1) {  // v2.5
            if (nFrequencyIndex == 0) {
                nFrequency = 11025;
            } else if (nFrequencyIndex == 1) {
                nFrequency = 12000;
            } else if (nFrequencyIndex == 2) {
                nFrequency = 8000;
            }
        } else if (nVersion == 2) {  // v2
            if (nFrequencyIndex == 0) {
                nFrequency = 22050;
            } else if (nFrequencyIndex == 1) {
                nFrequency = 24000;
            } else if (nFrequencyIndex == 2) {
                nFrequency = 16000;
            }
        } else if (nVersion == 3) {  // v1
            if (nFrequencyIndex == 0) {
                nFrequency = 44100;
            } else if (nFrequencyIndex == 1) {
                nFrequency = 48000;
            } else if (nFrequencyIndex == 2) {
                nFrequency = 32000;
            }
        }

        if (nFrequency) {
            if (nLayer == 3) {
                dTime = (double)384 / nFrequency;
            } else if ((nLayer == 2) || (nVersion == 3)) {
                dTime = (double)1152 / nFrequency;
            } else {
                dTime = (double)576 / nFrequency;
            }

            if (nLayer == 3) {
                nResult += std::floor(12000 * (double)nBitRate / nFrequency + nPadBit) * 4;
            } else {
                // 125 = 1000/8 = kilobits to bytes
                nResult += std::floor(125 * nBitRate * dTime + nPadBit);
            }
        }
    }

    return nResult;
}
