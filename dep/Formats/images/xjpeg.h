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
#ifndef XJPEG_H
#define XJPEG_H

#include "xbinary.h"
#include "xtiff.h"

class XJpeg : public XBinary {
    Q_OBJECT

public:
    struct CHUNK {
        bool bValid;
        bool bEntropyCodedData;
        quint8 nId;
        qint64 nDataOffset;
        qint64 nDataSize;
    };

    explicit XJpeg(QIODevice *pDevice = nullptr);
    ~XJpeg();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType();
    virtual QString getFileFormatString();
    virtual QString getFileFormatExt();
    virtual qint64 getFileFormatSize();

    QList<CHUNK> getChunks(PDSTRUCT *pPdStruct = nullptr);
    static QList<CHUNK> _getChunksById(QList<CHUNK> *pListChunks, quint8 nId);
    QString getComment(QList<CHUNK> *pListChunks);
    QString getComment();

    QString getDqtMD5(QList<CHUNK> *pListChunks);
    QString getDqtMD5();

    bool isChunkPresent(QList<CHUNK> *pListChunks, quint8 nId);

    OFFSETSIZE getExif(QList<CHUNK> *pListChunks);
    bool isExifPresent(OFFSETSIZE osExif);

    QList<XTiff::CHUNK> getExifChunks(OFFSETSIZE osExif, PDSTRUCT *pPdStruct = nullptr);
    QString getExifCameraName(OFFSETSIZE osExif, QList<XTiff::CHUNK> *pListExifChunks);

private:
    CHUNK _readChunk(qint64 nOffset);
};

#endif  // XJPEG_H
