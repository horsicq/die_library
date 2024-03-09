/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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
#include "xarchive.h"

#if defined(_MSC_VER)
#if _MSC_VER > 1800                                   // TODO Check !!!
#pragma comment(lib, "legacy_stdio_definitions.lib")  // bzip2.lib(compress.obj) __imp__fprintf

FILE _iob[] = {*stdin, *stdout, *stderr};  // bzip2.lib(compress.obj) _iob_func

extern "C" FILE *__cdecl __iob_func(void)
{
    return _iob;
}
#endif
#endif

static void *SzAlloc(ISzAllocPtr, size_t size)
{
    return malloc(size);
}

static void SzFree(ISzAllocPtr, void *address)
{
    free(address);
}

static ISzAlloc g_Alloc = {SzAlloc, SzFree};

XArchive::XArchive(QIODevice *pDevice) : XBinary(pDevice)
{
}

XArchive::COMPRESS_RESULT XArchive::decompress(XArchive::COMPRESS_METHOD compressMethod, QIODevice *pSourceDevice, QIODevice *pDestDevice, bool bHeaderOnly,
                                               PDSTRUCT *pPdStruct, qint64 *pnInSize, qint64 *pnOutSize)
{
    // TODO Progress PDSTRUCT
    qint64 __nInSize = 0;
    qint64 __nOutSize = 0;
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (pPdStruct == nullptr) {
        pPdStruct = &pdStructEmpty;
    }

    if (pnInSize == 0) {
        pnInSize = &__nInSize;
    }

    if (pnOutSize == 0) {
        pnOutSize = &__nOutSize;
    }

    COMPRESS_RESULT result = COMPRESS_RESULT_UNKNOWN;

    if (compressMethod == COMPRESS_METHOD_STORE) {
        const qint32 CHUNK = DECOMPRESS_BUFFERSIZE;
        char buffer[CHUNK];
        qint64 nSize = pSourceDevice->size();

        result = COMPRESS_RESULT_OK;

        while (nSize > 0) {
            qint64 nTemp = qMin((qint64)CHUNK, nSize);

            if (pSourceDevice->read(buffer, nTemp) != nTemp) {
                result = COMPRESS_RESULT_READERROR;
                break;
            }

            if (pDestDevice) {
                if (pDestDevice->write(buffer, nTemp) != nTemp) {
                    result = COMPRESS_RESULT_WRITEERROR;
                    break;
                }
            }

            if (bHeaderOnly)  // Only the first bytes
            {
                break;
            }

            if (pPdStruct->bIsStop) {
                break;
            }

            nSize -= nTemp;
            *pnInSize += nTemp;
            *pnOutSize += nTemp;
        }
    } else if (compressMethod == COMPRESS_METHOD_PPMD) {
        // TODO Check
#ifdef PPMD_SUPPORT
        quint8 nOrder = 0;
        quint32 nMemSize = 0;

        pSourceDevice->read((char *)(&nOrder), 1);
        pSourceDevice->read((char *)(&nMemSize), 4);

        bool bSuccess = true;

        if ((nOrder < PPMD7_MIN_ORDER) || (nOrder > PPMD7_MAX_ORDER) || (nMemSize < PPMD7_MIN_MEM_SIZE) || (nMemSize > PPMD7_MAX_MEM_SIZE)) {
            bSuccess = false;
        }

        bSuccess = true;

        if (bSuccess) {
            CPpmd7 ppmd;
            Ppmd7_Construct(&ppmd);

            if (Ppmd7_Alloc(&ppmd, nMemSize, &g_Alloc)) {
                Ppmd7_Init(&ppmd, nOrder);
            }
        }
#endif
    } else if (compressMethod == COMPRESS_METHOD_DEFLATE) {
        const qint32 CHUNK = DECOMPRESS_BUFFERSIZE;

        unsigned char in[CHUNK];
        unsigned char out[CHUNK];

        z_stream strm;

        strm.zalloc = nullptr;
        strm.zfree = nullptr;
        strm.opaque = nullptr;
        strm.avail_in = 0;
        strm.next_in = nullptr;

        qint32 ret = Z_OK;
        result = COMPRESS_RESULT_OK;

        if (inflateInit2(&strm, -MAX_WBITS) == Z_OK)  // -MAX_WBITS for raw data
        {
            do {
                strm.avail_in = pSourceDevice->read((char *)in, CHUNK);

                if (strm.avail_in == 0) {
                    ret = Z_ERRNO;
                    break;
                }

                strm.next_in = in;

                qint64 nTmpInSize = strm.avail_in;

                do {
                    strm.avail_out = CHUNK;
                    //                    strm.avail_out=1;
                    strm.next_out = out;
                    ret = inflate(&strm, Z_NO_FLUSH);
                    //                    ret=inflate(&strm,Z_SYNC_FLUSH);

                    if ((ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR) || (ret == Z_NEED_DICT)) {
                        break;
                    }

                    qint32 nTemp = CHUNK - strm.avail_out;

                    if (pDestDevice) {
                        if (pDestDevice->write((char *)out, nTemp) != nTemp) {
                            ret = Z_ERRNO;
                            break;
                        }
                    }

                    *pnOutSize += nTemp;

                    if (bHeaderOnly)  // Only the first bytes
                    {
                        break;
                    }
                } while (strm.avail_out == 0);

                nTmpInSize -= strm.avail_in;

                *pnInSize += nTmpInSize;

                if ((ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR) || (ret == Z_NEED_DICT) || (ret == Z_ERRNO)) {
                    break;
                }

                if (pPdStruct->bIsStop) {
                    break;
                }
            } while (ret != Z_STREAM_END);

            inflateEnd(&strm);

            if ((ret == Z_OK) || (ret == Z_STREAM_END))  // TODO Check Z_OK
            {
                result = COMPRESS_RESULT_OK;
            } else if (ret == Z_BUF_ERROR) {
                result = COMPRESS_RESULT_BUFFERERROR;
            } else if (ret == Z_MEM_ERROR) {
                result = COMPRESS_RESULT_MEMORYERROR;
            } else if (ret == Z_DATA_ERROR) {
                result = COMPRESS_RESULT_DATAERROR;
            } else {
                result = COMPRESS_RESULT_UNKNOWN;
            }
        }
    } else if (compressMethod == COMPRESS_METHOD_BZIP2) {
        const qint32 CHUNK = DECOMPRESS_BUFFERSIZE;

        char in[CHUNK];
        char out[CHUNK];

        bz_stream strm = {};
        qint32 ret = BZ_MEM_ERROR;
        result = COMPRESS_RESULT_OK;

        qint32 rc = BZ2_bzDecompressInit(&strm, 0, 0);

        if (rc == BZ_OK) {
            do {
                strm.avail_in = pSourceDevice->read((char *)in, CHUNK);

                if (strm.avail_in == 0) {
                    ret = BZ_MEM_ERROR;
                    break;
                }

                strm.next_in = in;

                qint64 nTmpInSize = strm.avail_in;

                do {
                    strm.avail_out = CHUNK;
                    strm.next_out = out;
                    ret = BZ2_bzDecompress(&strm);

                    if ((ret != BZ_STREAM_END) && (ret != BZ_OK)) {
                        break;
                    }

                    qint32 nTemp = CHUNK - strm.avail_out;

                    if (pDestDevice) {
                        if (pDestDevice->write((char *)out, nTemp) != nTemp) {
                            ret = BZ_MEM_ERROR;
                            break;
                        }
                    }

                    *pnOutSize += nTemp;

                    if (bHeaderOnly)  // Only the first bytes
                    {
                        break;
                    }
                } while (strm.avail_out == 0);

                nTmpInSize -= strm.avail_in;
                *pnInSize += nTmpInSize;

                if (ret != BZ_OK) {
                    break;
                }

                if (pPdStruct->bIsStop) {
                    break;
                }
            } while (ret != BZ_STREAM_END);

            BZ2_bzDecompressEnd(&strm);
        }

        // TODO more error codes
        if ((ret == BZ_OK) || (ret == BZ_STREAM_END)) {
            result = COMPRESS_RESULT_OK;
        } else if (ret == BZ_MEM_ERROR) {
            result = COMPRESS_RESULT_DATAERROR;
        } else {
            result = COMPRESS_RESULT_UNKNOWN;
        }
    } else if (compressMethod == COMPRESS_METHOD_LZMA_ZIP) {
        result = COMPRESS_RESULT_OK;

        // TODO more error codes
        qint32 nPropSize = 0;
        char header1[4] = {};
        quint8 properties[32] = {};

        pSourceDevice->read(header1, sizeof(header1));
        nPropSize = header1[2];  // TODO Check

        if (nPropSize && (nPropSize < 30)) {
            pSourceDevice->read((char *)properties, nPropSize);

            CLzmaDec state = {};

            SRes ret = LzmaProps_Decode(&state.prop, (Byte *)properties, nPropSize);

            if (ret == 0)  // S_OK
            {
                LzmaDec_Construct(&state);
                ret = LzmaDec_Allocate(&state, (Byte *)properties, nPropSize, &g_Alloc);

                if (ret == 0)  // S_OK
                {
                    LzmaDec_Init(&state);

                    const qint32 CHUNK = DECOMPRESS_BUFFERSIZE;

                    char in[CHUNK];
                    char out[CHUNK];

                    bool bRun = true;

                    while (bRun) {
                        qint32 nSize = pSourceDevice->read((char *)in, CHUNK);

                        if (nSize) {
                            qint64 nPos = 0;

                            while (true) {
                                ELzmaStatus status;
                                SizeT inProcessed = nSize - nPos;
                                SizeT outProcessed = CHUNK;

                                ret = LzmaDec_DecodeToBuf(&state, (Byte *)out, &outProcessed, (Byte *)(in + nPos), &inProcessed, LZMA_FINISH_ANY, &status);

                                // TODO Check ret

                                nPos += inProcessed;

                                *pnInSize += inProcessed;
                                *pnOutSize += outProcessed;

                                if (pDestDevice) {
                                    if (pDestDevice->write((char *)out, outProcessed) != (qint64)outProcessed) {
                                        result = COMPRESS_RESULT_WRITEERROR;
                                        bRun = false;
                                        break;
                                    }
                                }

                                if (bHeaderOnly)  // Only the first bytes
                                {
                                    break;
                                }

                                if (status != LZMA_STATUS_NOT_FINISHED) {
                                    if (status == LZMA_STATUS_FINISHED_WITH_MARK) {
                                        result = COMPRESS_RESULT_OK;
                                        bRun = false;
                                    }

                                    break;
                                }
                            }
                        } else {
                            result = COMPRESS_RESULT_READERROR;
                            bRun = false;
                        }

                        if (bHeaderOnly)  // Only the first bytes
                        {
                            bRun = false;
                        }

                        if (pPdStruct->bIsStop) {
                            bRun = false;
                        }
                    }
                }

                LzmaDec_Free(&state, &g_Alloc);
            }
        }
    } else if ((compressMethod == COMPRESS_METHOD_LZH5) || (compressMethod == COMPRESS_METHOD_LZH6) || (compressMethod == COMPRESS_METHOD_LZH7)) {
        qint32 nMethod = 5;
        qint32 nBufferSize = 1U << 17;

        if (compressMethod == COMPRESS_METHOD_LZH5) {
            nMethod = 5;
        } else if (compressMethod == COMPRESS_METHOD_LZH6) {
            nMethod = 6;
        } else if (compressMethod == COMPRESS_METHOD_LZH7) {
            nMethod = 7;
        }

        XCompress::lzh_stream strm = {};

        qint32 ret = ARCHIVE_OK;

        //        qDebug("Size: %lld", pSourceDevice->size());

        //        if (pSourceDevice->size() > 25000) {
        //            ret = ARCHIVE_OK;
        //        }

        unsigned char *pInBuffer = (unsigned char *)malloc(nBufferSize);

        result = COMPRESS_RESULT_OK;

        if (XCompress::lzh_decode_init(&strm, nMethod)) {
            strm.avail_in = pSourceDevice->read((char *)pInBuffer, nBufferSize);  // We read from Device so if size < nBufferSize is OK

            if (strm.avail_in) {
                strm.next_in = pInBuffer;

                strm.total_in = 0;
                strm.avail_out = 0;
                // strm.ref_ptr = out;
                ret = XCompress::lzh_decode(&strm, true);

                if (pDestDevice) {
                    if (pDestDevice->write((char *)strm.ref_ptr, strm.total_out) != strm.total_out) {
                        ret = ARCHIVE_FATAL;
                    }
                }

                *pnInSize += strm.total_in;
                *pnOutSize += strm.total_out;
            }

            XCompress::lzh_decode_free(&strm);
        }

        free(pInBuffer);

        if (ret == ARCHIVE_OK) {
            result = COMPRESS_RESULT_OK;
        }
    }

    return result;
}

XArchive::COMPRESS_RESULT XArchive::compress(XArchive::COMPRESS_METHOD compressMethod, QIODevice *pSourceDevice, QIODevice *pDestDevice, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (pPdStruct == nullptr) {
        pPdStruct = &pdStructEmpty;
    }

    COMPRESS_RESULT result = COMPRESS_RESULT_UNKNOWN;

    if (compressMethod == COMPRESS_METHOD_STORE) {
        const qint32 CHUNK = COMPRESS_BUFFERSIZE;
        char buffer[CHUNK];
        qint64 nSize = pSourceDevice->size();

        result = COMPRESS_RESULT_OK;

        while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
            qint64 nTemp = qMin((qint64)CHUNK, nSize);

            if (pSourceDevice->read(buffer, nTemp) != nTemp) {
                result = COMPRESS_RESULT_READERROR;
                break;
            }

            if (pDestDevice->write(buffer, nTemp) != nTemp) {
                result = COMPRESS_RESULT_WRITEERROR;
                break;
            }

            nSize -= nTemp;
        }
    } else if (compressMethod == COMPRESS_METHOD_DEFLATE) {
        result = compress_deflate(pSourceDevice, pDestDevice, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);  // -MAX_WBITS for raw data
    }

    return result;
}

XArchive::COMPRESS_RESULT XArchive::compress_deflate(QIODevice *pSourceDevice, QIODevice *pDestDevice, qint32 nLevel, qint32 nMethod, qint32 nWindowsBits,
                                                     qint32 nMemLevel, qint32 nStrategy)
{
    COMPRESS_RESULT result = COMPRESS_RESULT_UNKNOWN;

    const qint32 CHUNK = COMPRESS_BUFFERSIZE;

    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    z_stream strm;

    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    strm.avail_in = 0;
    strm.next_in = nullptr;

    qint32 ret = Z_OK;

    if (deflateInit2(&strm, nLevel, nMethod, nWindowsBits, nMemLevel, nStrategy) == Z_OK) {
        do {
            strm.avail_in = pSourceDevice->read((char *)in, CHUNK);

            qint32 nFlush = Z_NO_FLUSH;

            if (strm.avail_in != CHUNK) {
                nFlush = Z_FINISH;
            }

            if (strm.avail_in == 0) {
                if (!pSourceDevice->atEnd()) {
                    ret = Z_ERRNO;
                    break;
                }
            }

            strm.next_in = in;

            do {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = deflate(&strm, nFlush);

                if ((ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR) || (ret == Z_NEED_DICT)) {
                    break;
                }

                qint32 nTemp = CHUNK - strm.avail_out;

                if (pDestDevice->write((char *)out, nTemp) != nTemp) {
                    ret = Z_ERRNO;
                    break;
                }
            } while (strm.avail_out == 0);

            if ((ret == Z_DATA_ERROR) || (ret == Z_MEM_ERROR) || (ret == Z_NEED_DICT) || (ret == Z_ERRNO)) {
                break;
            }
        } while (ret != Z_STREAM_END);

        deflateEnd(&strm);

        if ((ret == Z_OK) || (ret == Z_STREAM_END)) {
            result = COMPRESS_RESULT_OK;
        } else if (ret == Z_BUF_ERROR) {
            result = COMPRESS_RESULT_BUFFERERROR;
        } else if (ret == Z_MEM_ERROR) {
            result = COMPRESS_RESULT_MEMORYERROR;
        } else if (ret == Z_DATA_ERROR) {
            result = COMPRESS_RESULT_DATAERROR;
        } else {
            result = COMPRESS_RESULT_UNKNOWN;
        }
    }

    return result;
}

QByteArray XArchive::decompress(const XArchive::RECORD *pRecord, bool bHeaderOnly, PDSTRUCT *pPdStruct)
{
    QByteArray result;

    SubDevice sd(getDevice(), pRecord->nDataOffset, pRecord->nCompressedSize);

    if (sd.open(QIODevice::ReadOnly)) {
        QBuffer buffer;
        buffer.setBuffer(&result);
        buffer.open(QIODevice::WriteOnly);

        decompress(pRecord->compressMethod, &sd, &buffer, bHeaderOnly, pPdStruct);

        buffer.close();

        sd.close();
    }

    return result;
}

QByteArray XArchive::decompress(QList<XArchive::RECORD> *pListArchive, const QString &sRecordFileName, PDSTRUCT *pPdStruct)
{
    QByteArray baResult;

    XArchive::RECORD record = XArchive::getArchiveRecord(sRecordFileName, pListArchive, pPdStruct);

    if (!record.sFileName.isEmpty()) {
        if (record.nUncompressedSize) {
            baResult = decompress(&record, false, pPdStruct);
        }
    }

    return baResult;
}

QByteArray XArchive::decompress(const QString &sRecordFileName, PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listArchive = getRecords(-1, pPdStruct);

    return decompress(&listArchive, sRecordFileName, pPdStruct);
}

bool XArchive::decompressToFile(const XArchive::RECORD *pRecord, const QString &sResultFileName, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFileInfo fi(sResultFileName);

    bResult = XBinary::createDirectory(fi.absolutePath());

    if (pRecord->nCompressedSize) {
        QFile file;
        file.setFileName(sResultFileName);

        if (file.open(QIODevice::ReadWrite)) {
            SubDevice sd(getDevice(), pRecord->nDataOffset, pRecord->nCompressedSize);

            if (sd.open(QIODevice::ReadOnly)) {
                file.resize(0);

                bResult = (decompress(pRecord->compressMethod, &sd, &file, false, pPdStruct) == COMPRESS_RESULT_OK);

                sd.close();
            }

            file.close();
        }
    }

    return bResult;
}

bool XArchive::decompressToFile(QList<XArchive::RECORD> *pListArchive, const QString &sRecordFileName, const QString &sResultFileName, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    XArchive::RECORD record = getArchiveRecord(sRecordFileName, pListArchive);

    if (record.sFileName != "")  // TODO bIsValid
    {
        bResult = decompressToFile(&record, sResultFileName, pPdStruct);
    }

    return bResult;
}

bool XArchive::decompressToPath(QList<XArchive::RECORD> *pListArchive, const QString &sRecordFileName, const QString &sResultPathName, PDSTRUCT *pPdStruct)
{
    bool bResult = true;

    QFileInfo fi(sResultPathName);

    XBinary::createDirectory(fi.absolutePath());

    qint32 nNumberOfArchives = pListArchive->count();

    for (qint32 i = 0; i < nNumberOfArchives; i++) {
        XArchive::RECORD record = pListArchive->at(i);

        bool bNamePresent = XBinary::isRegExpPresent(QString("^%1").arg(sRecordFileName), record.sFileName);

        if (bNamePresent || (sRecordFileName == "/") || (sRecordFileName == "")) {
            QString sFileName = record.sFileName;

            if (bNamePresent) {
                sFileName = sFileName.mid(sRecordFileName.size(), -1);
            }

            QString sResultFileName = sResultPathName + QDir::separator() + sFileName;

            QFileInfo fi(sResultFileName);
            XBinary::createDirectory(fi.absolutePath());

            if (!decompressToFile(&record, sResultFileName, pPdStruct)) {
                bResult = false;
                break;
            }
        }
    }

    // TODO Progressbar pPdStruct
    // TODO emits

    return bResult;
}

bool XArchive::decompressToFile(const QString &sArchiveFileName, const QString &sRecordFileName, const QString &sResultFileName, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sArchiveFileName);

    if (file.open(QIODevice::ReadOnly)) {
        setDevice(&file);

        if (isValid()) {
            QList<RECORD> listRecords = getRecords(-1, pPdStruct);

            bResult = decompressToFile(&listRecords, sRecordFileName, sResultFileName, pPdStruct);
        }

        file.close();
    }

    return bResult;
}

bool XArchive::decompressToPath(const QString &sArchiveFileName, const QString &sRecordPathName, const QString &sResultPathName, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sArchiveFileName);

    if (file.open(QIODevice::ReadOnly)) {
        setDevice(&file);

        if (isValid()) {
            QList<RECORD> listRecords = getRecords(-1, pPdStruct);

            bResult = decompressToPath(&listRecords, sRecordPathName, sResultPathName, pPdStruct);
        }

        file.close();
    }

    return bResult;
}

bool XArchive::dumpToFile(const XArchive::RECORD *pRecord, const QString &sFileName, PDSTRUCT *pPdStruct)
{
    return XBinary::dumpToFile(sFileName, pRecord->nDataOffset, pRecord->nCompressedSize, pPdStruct);
}

XArchive::RECORD XArchive::getArchiveRecord(const QString &sRecordFileName, QList<XArchive::RECORD> *pListRecords, PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    RECORD result = {};

    qint32 nNumberOfArchives = pListRecords->count();

    for (qint32 i = 0; (i < nNumberOfArchives) && (!(pPdStruct->bIsStop)); i++) {
        if (pListRecords->at(i).sFileName == sRecordFileName) {
            result = pListRecords->at(i);
            break;
        }
    }

    return result;
}

bool XArchive::isArchiveRecordPresent(const QString &sRecordFileName, PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listRecords = getRecords(-1, pPdStruct);

    return isArchiveRecordPresent(sRecordFileName, &listRecords, pPdStruct);
}

bool XArchive::isArchiveRecordPresent(const QString &sRecordFileName, QList<XArchive::RECORD> *pListRecords, PDSTRUCT *pPdStruct)
{
    return (!getArchiveRecord(sRecordFileName, pListRecords, pPdStruct).sFileName.isEmpty());
}

quint32 XArchive::getCompressBufferSize()
{
    return COMPRESS_BUFFERSIZE;
}

quint32 XArchive::getDecompressBufferSize()
{
    return DECOMPRESS_BUFFERSIZE;
}

void XArchive::showRecords(QList<XArchive::RECORD> *pListArchive)
{
    qint32 nNumberOfRecords = pListArchive->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
#ifdef QT_DEBUG
        qDebug("%s", pListArchive->at(i).sFileName.toLatin1().data());
#endif
    }
}

XBinary::MODE XArchive::getMode()
{
    return MODE_DATA;
}

// XBinary::_MEMORY_MAP XArchive::getMemoryMap()
//{
//     _MEMORY_MAP result={};

//    qint64 nTotalSize=getSize();

//    result.nBaseAddress=_getBaseAddress();
//    result.nRawSize=nTotalSize;
//    result.nImageSize=nTotalSize;
//    result.fileType=FT_ARCHIVE;
//    result.mode=getMode();
//    result.sArch=getArch();
//    result.bIsBigEndian=isBigEndian();
//    result.sType=getTypeAsString();

//    qint32 nIndex=0;

//    QList<XArchive::RECORD> listRecords=getRecords();

//    qint32 nNumberOfRecords=listRecords.count();

//    for(qint32 i=0;i<nNumberOfRecords;i++)
//    {
//        _MEMORY_RECORD record={};
//        record.nAddress=-1;
//        record.segment=ADDRESS_SEGMENT_FLAT;
//        record.nOffset=listRecords.at(i).nDataOffset;
//        record.nSize=listRecords.at(i).nCompressedSize;
//        record.nIndex=nIndex++;
//        record.type=MMT_FILESEGMENT;
//        record.sName=listRecords.at(i).sFileName;

//        result.listRecords.append(record);
//    }

//    return result;
//}
