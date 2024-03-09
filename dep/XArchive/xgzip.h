/* Copyright (c) 2022-2023 hors<horsicq@gmail.com>
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
#ifndef XGZIP_H
#define XGZIP_H

#include "xarchive.h"

class XGzip : public XArchive {
    Q_OBJECT
public:
#pragma pack(push)
#pragma pack(1)
    struct GZIP_HEADER {
        quint16 nMagic;             // 0x1f 0x8b	Magic number identifying file type
        quint8 nCompressionMethod;  // Compression Method * 0-7 (Reserved) * 8 (Deflate)
        quint8 nFileFlags;          // File Flags
        quint32 nTimeStamp;         // 32-bit timestamp
        quint8 nCompressionFlags;   // Compression flags
        quint8 nOS;                 // Operating system ID
    };
#pragma pack(pop)

    explicit XGzip(QIODevice *pDevice = nullptr);

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice);
    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct);
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct);
    virtual qint64 getFileFormatSize();
    virtual QList<MAPMODE> getMapModesList(PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);

    virtual QString getFileFormatExt();
    virtual QString getFileFormatString();
};

#endif  // XGZIP_H
