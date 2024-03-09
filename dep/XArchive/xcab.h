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
#ifndef XCAB_H
#define XCAB_H

#include "xarchive.h"

class XCab : public XArchive {
    Q_OBJECT

public:
#pragma pack(push)
#pragma pack(1)
    struct CFHEADER {
        quint8 signature[4];  // Cabinet file signature
        quint32 reserved1;    // reserved
        quint32 cbCabinet;    // size of this cabinet file in bytes
        quint32 reserved2;    // reserved
        quint32 coffFiles;    // offset of the first CFFILE entry
        quint32 reserved3;    // reserved
        quint8 versionMinor;  // cabinet file format version, minor
        quint8 versionMajor;  // cabinet file format version, major
        quint16 cFolders;     // number of CFFOLDER entries in this cabinet */
        quint16 cFiles;       // number of CFFILE entries in this cabinet */
        quint16 flags;        // cabinet file option indicators */
        quint16 setID;        // must be the same for all cabinets in a set */
        quint16 iCabinet;     // number of this cabinet file in a set */
        // Optional
        quint16 cbCFHeader;  // (optional) size of per-cabinet reserved area */
        quint8 cbCFFolder;   // (optional) size of per-folder reserved area */
        quint8 cbCFData;     // (optional) size of per-datablock reserved area */
        // u1  abReserve[];          /* (optional) per-cabinet reserved area */
        // u1  szCabinetPrev[];      /* (optional) name of previous cabinet file */
        // u1  szDiskPrev[];         /* (optional) name of previous disk */
        // u1  szCabinetNext[];      /* (optional) name of next cabinet file */
        // u1  szDiskNext[];         /* (optional) name of next disk */
    };

    struct CFFOLDER {
        quint32 coffCabStart; /* offset of the first CFDATA block in this folder */
        quint16 cCFData;      /* number of CFDATA blocks in this folder */
        quint16 typeCompress; /* compression type indicator */
        // u1  abReserve[];          /* (optional) per-folder reserved area */
    };

    struct CFFILE {
        quint32 cbFile;          /* uncompressed size of this file in bytes */
        quint32 uoffFolderStart; /* uncompressed offset of this file in the folder */
        quint16 iFolder;         /* index into the CFFOLDER area */
        quint16 date;            /* date stamp for this file */
        quint16 time;            /* time stamp for this file */
        quint16 attribs;         /* attribute flags for this file */
        // u1  szName[];             /* name of this file */
    };

    struct CFDATA {
        quint32 csum;     /* checksum of this CFDATA entry */
        quint16 cbData;   /* number of compressed bytes in this block */
        quint16 cbUncomp; /* number of uncompressed bytes in this block */
                          //        u1  abReserve[];          /* (optional) per-datablock reserved area */
                          //        u1  ab[cbData];           /* compressed data bytes */
    };
#pragma pack(pop)

    explicit XCab(QIODevice *pDevice = nullptr);

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice);
    virtual QString getVersion();
    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct);
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct);

    CFHEADER readCFHeader();
    CFFOLDER readCFFolder(qint64 nOffset);
    CFDATA readCFData(qint64 nOffset);

    virtual FT getFileType();
    virtual QString getFileFormatString();
    virtual QString getFileFormatExt();
    virtual qint64 getFileFormatSize();
    virtual QList<MAPMODE> getMapModesList(PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
};

#endif  // XCAB_H
