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
#ifndef XZIP_H
#define XZIP_H

#include "xarchive.h"

// TODO OSNAME
class XZip : public XArchive {
    Q_OBJECT

public:
    enum SIGNATURE {
        SIGNATURE_ECD = 0x06054B50,
        SIGNATURE_CFD = 0x02014B50,
        SIGNATURE_LFD = 0x04034B50
    };

    //    0 - The file is stored (no compression)
    //    1 - The file is Shrunk
    //    2 - The file is Reduced with compression factor 1
    //    3 - The file is Reduced with compression factor 2
    //    4 - The file is Reduced with compression factor 3
    //    5 - The file is Reduced with compression factor 4
    //    6 - The file is Imploded
    //    7 - Reserved for Tokenizing compression algorithm
    //    8 - The file is Deflated
    //    9 - Enhanced Deflating using Deflate64(tm)
    //    10 - PKWARE Data Compression Library Imploding (old IBM TERSE)
    //    11 - Reserved by PKWARE
    //    12 - File is compressed using BZIP2 algorithm
    //    13 - Reserved by PKWARE
    //    14 - LZMA (EFS)
    //    15 - Reserved by PKWARE
    //    16 - Reserved by PKWARE
    //    17 - Reserved by PKWARE
    //    18 - File is compressed using IBM TERSE (new)
    //    19 - IBM LZ77 z Architecture (PFS)
    //    97 - WavPack compressed data
    //    98 - PPMd version I, Rev 1
    //    99 - Apple LZFSE

    enum CMETHOD {
        CMETHOD_STORE = 0,
        CMETHOD_DEFLATE = 8,
        CMETHOD_DEFLATE64 = 9,
        CMETHOD_BZIP2 = 12,
        CMETHOD_LZMA = 14,
        CMETHOD_PPMD = 98,
        CMETHOD_LZFSE = 99,  // Apple
    };

#pragma pack(push)
#pragma pack(1)
    struct LOCALFILEHEADER {
        quint32 nSignature;  // SIGNATURE_LFD
        quint16 nMinVersion;
        quint16 nFlags;
        quint16 nMethod;
        quint16 nLastModTime;
        quint16 nLastModDate;
        quint32 nCRC32;
        quint32 nCompressedSize;
        quint32 nUncompressedSize;
        quint16 nFileNameLength;
        quint16 nExtraFieldLength;
        // File name
        // Extra field
    };

    struct ENDOFCENTRALDIRECTORYRECORD {
        quint32 nSignature;  // SIGNATURE_ECD
        quint16 nDiskNumber;
        quint16 nStartDisk;
        quint16 nDiskNumberOfRecords;
        quint16 nTotalNumberOfRecords;
        quint32 nSizeOfCentralDirectory;
        quint32 nOffsetToCentralDirectory;
        quint16 nCommentLength;
        // Comment
    };

    struct CENTRALDIRECTORYFILEHEADER {
        quint32 nSignature;  // SIGNATURE_CFD
        quint16 nVersion;
        quint16 nMinVersion;
        quint16 nFlags;
        quint16 nMethod;
        quint16 nLastModTime;
        quint16 nLastModDate;
        quint32 nCRC32;
        quint32 nCompressedSize;
        quint32 nUncompressedSize;
        quint16 nFileNameLength;
        quint16 nExtraFieldLength;
        quint16 nFileCommentLength;
        quint16 nStartDisk;
        quint16 nInternalFileAttributes;
        quint32 nExternalFileAttributes;
        quint32 nOffsetToLocalFileHeader;
        // File name
        // Extra field
        // File Comment
    };
#pragma pack(pop)

    struct ZIPFILE_RECORD {
        QString sFileName;
        quint16 nVersion;
        quint16 nMinVersion;
        quint16 nFlags;
        CMETHOD method;
        QDateTime dtTime;
        quint32 nCRC32;
        qint64 nCompressedSize;
        qint64 nUncompressedSize;
        qint64 nHeaderOffset;
        qint64 nDataOffset;
        // TODO Comment!!!
    };

    explicit XZip(QIODevice *pDevice = nullptr);
    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice);
    virtual QString getVersion();
    virtual bool isEncrypted();
    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct);
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct);
    virtual bool isSigned();
    virtual OFFSETSIZE getSignOffsetSize();

    virtual FT getFileType();

    static FT _getFileType(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, bool bDeep);

    virtual FILEFORMATINFO getFileFormatInfo();

    bool isAPKSignBlockPresent();  // For APK Check jar

    struct APK_SIG_BLOCK_RECORD {
        quint32 nID;
        quint64 nDataOffset;
        quint64 nDataSize;
    };

    QList<APK_SIG_BLOCK_RECORD> getAPKSignaturesBlockRecordsList();
    static bool isAPKSignatureBlockRecordPresent(QList<APK_SIG_BLOCK_RECORD> *pList, quint32 nID);
    static APK_SIG_BLOCK_RECORD getAPKSignatureBlockRecord(QList<APK_SIG_BLOCK_RECORD> *pList, quint32 nID);

    static bool addLocalFileRecord(QIODevice *pSource, QIODevice *pDest, ZIPFILE_RECORD *pZipFileRecord);
    static bool addCentralDirectory(QIODevice *pDest, QList<ZIPFILE_RECORD> *pListZipFileRecords, const QString &sComment = "");

    virtual QString getFileFormatExt();
    virtual QString getFileFormatString();
    virtual qint64 getFileFormatSize();

    virtual OSINFO getOsInfo(QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct);

private:
    qint64 findECDOffset();
    qint64 findAPKSignBlockOffset(PDSTRUCT *pPdStruct = nullptr);
    COMPRESS_METHOD zipToCompressMethod(quint16 nZipMethod);
};

#endif  // XZIP_H
