/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#ifndef XARCHIVE_H
#define XARCHIVE_H

#include "LzmaDec.h"
#include "bzlib.h"
#include "xbinary.h"
#include "zlib.h"
#ifdef PPMD_SUPPORT
#include "Ppmd7.h"
#endif
#include "xcompress.h"

class XArchive : public XBinary {
    Q_OBJECT

public:
    enum COMPRESS_METHOD {
        COMPRESS_METHOD_UNKNOWN = 0,
        COMPRESS_METHOD_STORE,
        COMPRESS_METHOD_FILE,
        COMPRESS_METHOD_DEFLATE,
        COMPRESS_METHOD_DEFLATE64,
        COMPRESS_METHOD_BZIP2,
        COMPRESS_METHOD_LZMA_ZIP,
        COMPRESS_METHOD_PPMD,
        COMPRESS_METHOD_LZH5,
        COMPRESS_METHOD_LZH6,
        COMPRESS_METHOD_LZH7
        // TODO check more methods
    };

    struct RECORD {
        // TODO bIsValid !!!
        // TODO Date
        QString sFileName;
        quint32 nCRC32;
        qint64 nDataOffset;
        qint64 nCompressedSize;
        qint64 nUncompressedSize;
        COMPRESS_METHOD compressMethod;
        qint64 nHeaderOffset;
        qint64 nHeaderSize;
        qint64 nOptHeaderOffset;
        qint64 nOptHeaderSize;
    };

    enum COMPRESS_RESULT {
        COMPRESS_RESULT_UNKNOWN = 0,
        COMPRESS_RESULT_OK,
        COMPRESS_RESULT_DATAERROR,
        COMPRESS_RESULT_MEMORYERROR,
        COMPRESS_RESULT_BUFFERERROR,
        COMPRESS_RESULT_METHODERROR,
        COMPRESS_RESULT_READERROR,
        COMPRESS_RESULT_WRITEERROR
        // TODO more
    };

    static const qint32 COMPRESS_BUFFERSIZE = 0x4000;    // TODO Check mb set/get ???
    static const qint32 DECOMPRESS_BUFFERSIZE = 0x4000;  // TODO Check mb set/get ???

    explicit XArchive(QIODevice *pDevice = nullptr);

    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct) = 0;
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct) = 0;
    static COMPRESS_RESULT decompress(COMPRESS_METHOD compressMethod, QIODevice *pSourceDevice, QIODevice *pDestDevice, bool bHeaderOnly = false,
                                      PDSTRUCT *pPdStruct = nullptr, qint64 *pnInSize = nullptr, qint64 *pnOutSize = nullptr);
    static COMPRESS_RESULT compress(COMPRESS_METHOD compressMethod, QIODevice *pSourceDevice, QIODevice *pDestDevice, PDSTRUCT *pPdStruct = nullptr);
    static COMPRESS_RESULT compress_deflate(QIODevice *pSourceDevice, QIODevice *pDestDevice, qint32 nLevel, qint32 nMethod, qint32 nWindowsBits, qint32 nMemLevel,
                                            qint32 nStrategy);  // TODO PDSTRUCT
    QByteArray decompress(const RECORD *pRecord, bool bHeaderOnly, PDSTRUCT *pPdStruct);
    QByteArray decompress(QList<RECORD> *pListArchive, const QString &sRecordFileName, PDSTRUCT *pPdStruct = nullptr);
    QByteArray decompress(const QString &sRecordFileName, PDSTRUCT *pPdStruct = nullptr);
    bool decompressToFile(const RECORD *pRecord, const QString &sResultFileName, PDSTRUCT *pPdStruct = nullptr);
    bool decompressToFile(QList<RECORD> *pListArchive, const QString &sRecordFileName, const QString &sResultFileName, PDSTRUCT *pPdStruct = nullptr);
    bool decompressToPath(QList<RECORD> *pListArchive, const QString &sRecordFileName, const QString &sResultPathName, PDSTRUCT *pPdStruct = nullptr);
    bool decompressToFile(const QString &sArchiveFileName, const QString &sRecordFileName, const QString &sResultFileName, PDSTRUCT *pPdStruct = nullptr);
    bool decompressToPath(const QString &sArchiveFileName, const QString &sRecordPathName, const QString &sResultPathName, PDSTRUCT *pPdStruct = nullptr);
    bool dumpToFile(const RECORD *pRecord, const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static RECORD getArchiveRecord(const QString &sRecordFileName, QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct = nullptr);
    bool isArchiveRecordPresent(const QString &sRecordFileName, PDSTRUCT *pPdStruct = nullptr);
    static bool isArchiveRecordPresent(const QString &sRecordFileName, QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct = nullptr);
    static quint32 getCompressBufferSize();
    static quint32 getDecompressBufferSize();
    static void showRecords(QList<RECORD> *pListArchive);
    virtual MODE getMode();
    //    virtual _MEMORY_MAP getMemoryMap(); // TODO
};

#endif  // XARCHIVE_H
