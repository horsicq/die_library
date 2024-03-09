/* Copyright (c) 2020-2024 hors<horsicq@gmail.com>
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
#include "xarchives.h"

XArchives::XArchives(QObject *pParent) : QObject(pParent)
{
}

QList<XArchive::RECORD> XArchives::getRecords(QIODevice *pDevice, qint32 nLimit, XBinary::PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<XArchive::RECORD> listResult;

    // TODO more !!!
    // CAB RAR 7ZIP
    QSet<XBinary::FT> stFileTypes = XBinary::getFileTypes(pDevice, true);

    if (stFileTypes.contains(XArchive::FT_ZIP)) {
        XZip xzip(pDevice);

        listResult = xzip.getRecords(nLimit, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_MACHOFAT)) {
        XMACHOFat xmachofat(pDevice);

        listResult = xmachofat.getRecords(nLimit, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_AR)) {
        X_Ar x_ar(pDevice);

        listResult = x_ar.getRecords(nLimit, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_GZIP)) {
        XGzip xgzip(pDevice);

        listResult = xgzip.getRecords(nLimit, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_ZLIB)) {
        XZlib xzlib(pDevice);

        listResult = xzlib.getRecords(nLimit, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_LHA)) {
        XLHA xhla(pDevice);

        listResult = xhla.getRecords(nLimit, pPdStruct);
    }

    return listResult;
}

QList<XArchive::RECORD> XArchives::getRecords(const QString &sFileName, qint32 nLimit, XBinary::PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        listResult = getRecords(&file, nLimit, pPdStruct);

        file.close();
    }

    return listResult;
}

QList<XArchive::RECORD> XArchives::getRecordsFromDirectory(const QString &sDirectoryName, qint32 nLimit, XBinary::PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listResult;

    _findFiles(sDirectoryName, &listResult, nLimit, pPdStruct);

    return listResult;
}

QByteArray XArchives::decompress(QIODevice *pDevice, XArchive::RECORD *pRecord, bool bHeaderOnly, XBinary::PDSTRUCT *pPdStruct)
{
    QByteArray baResult;

    QSet<XBinary::FT> stFileTypes = XBinary::getFileTypes(pDevice, true);

    if (stFileTypes.contains(XArchive::FT_ZIP)) {
        XZip xzip(pDevice);

        baResult = xzip.decompress(pRecord, bHeaderOnly, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_MACHOFAT)) {
        XMACHOFat xmachofat(pDevice);

        baResult = xmachofat.decompress(pRecord, bHeaderOnly, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_AR)) {
        X_Ar x_ar(pDevice);

        baResult = x_ar.decompress(pRecord, bHeaderOnly, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_GZIP)) {
        XGzip xgzip(pDevice);

        baResult = xgzip.decompress(pRecord, bHeaderOnly, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_ZLIB)) {
        XZlib xzlib(pDevice);

        baResult = xzlib.decompress(pRecord, bHeaderOnly, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_LHA)) {
        XLHA xlha(pDevice);

        baResult = xlha.decompress(pRecord, bHeaderOnly, pPdStruct);
    }

    return baResult;
}

QByteArray XArchives::decompress(const QString &sFileName, XArchive::RECORD *pRecord, bool bHeaderOnly, XBinary::PDSTRUCT *pPdStruct)
{
    QByteArray baResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        baResult = decompress(&file, pRecord, bHeaderOnly, pPdStruct);
        file.close();
    }

    return baResult;
}

QByteArray XArchives::decompress(QIODevice *pDevice, const QString &sRecordFileName, bool bHeaderOnly, XBinary::PDSTRUCT *pPdStruct)
{
    QList<XArchive::RECORD> listRecords = getRecords(pDevice);

    XArchive::RECORD record = XArchive::getArchiveRecord(sRecordFileName, &listRecords);

    return decompress(pDevice, &record, bHeaderOnly, pPdStruct);
}

QByteArray XArchives::decompress(const QString &sFileName, const QString &sRecordFileName, bool bHeaderOnly, XBinary::PDSTRUCT *pPdStruct)
{
    QByteArray baResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        baResult = decompress(&file, sRecordFileName, bHeaderOnly, pPdStruct);
        file.close();
    }

    return baResult;
}

bool XArchives::decompressToFile(QIODevice *pDevice, XArchive::RECORD *pRecord, const QString &sResultFileName, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QSet<XBinary::FT> stFileTypes = XBinary::getFileTypes(pDevice, true);

    // TODO more !!!
    // 7Zip
    // WinRAR
    // CAB
    if (stFileTypes.contains(XArchive::FT_ZIP)) {
        XZip xzip(pDevice);

        bResult = xzip.decompressToFile(pRecord, sResultFileName, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_MACHOFAT)) {
        XMACHOFat xmachofat(pDevice);

        bResult = xmachofat.decompressToFile(pRecord, sResultFileName, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_AR)) {
        X_Ar x_ar(pDevice);

        bResult = x_ar.decompressToFile(pRecord, sResultFileName, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_GZIP)) {
        XGzip xgzip(pDevice);

        bResult = xgzip.decompressToFile(pRecord, sResultFileName, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_ZLIB)) {
        XZlib xzlib(pDevice);

        bResult = xzlib.decompressToFile(pRecord, sResultFileName, pPdStruct);
    } else if (stFileTypes.contains(XArchive::FT_LHA)) {
        XLHA xlha(pDevice);

        bResult = xlha.decompressToFile(pRecord, sResultFileName, pPdStruct);
    }

    return bResult;
}

bool XArchives::decompressToFile(const QString &sFileName, XArchive::RECORD *pRecord, const QString &sResultFileName, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        bResult = decompressToFile(&file, pRecord, sResultFileName, pPdStruct);

        file.close();
    }

    return bResult;
}

bool XArchives::decompressToFile(const QString &sFileName, const QString &sRecordFileName, const QString &sResultFileName, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        QList<XArchive::RECORD> listRecords = getRecords(&file, -1, pPdStruct);

        XArchive::RECORD record = XArchive::getArchiveRecord(sRecordFileName, &listRecords, pPdStruct);

        if (record.sFileName != "") {
            bResult = decompressToFile(&file, &record, sResultFileName, pPdStruct);
        }

        file.close();
    }

    return bResult;
}

bool XArchives::decompressToFolder(QIODevice *pDevice, const QString &sResultFileFolder, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<XArchive::RECORD> listRecords = getRecords(pDevice, -1, pPdStruct);

    qint32 nNumberOfRecords = listRecords.count();

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        XArchive::RECORD record = listRecords.at(i);
        QString sResultFileName = sResultFileFolder + QDir::separator() + record.sFileName;

        bResult = decompressToFile(pDevice, &record, sResultFileName, pPdStruct);

        if (!bResult) {
            //            break;
        }
    }

    return bResult;
}

bool XArchives::decompressToFolder(const QString &sFileName, const QString &sResultFileFolder, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        bResult = decompressToFolder(&file, sResultFileFolder, pPdStruct);
        file.close();
    }

    return bResult;
}

bool XArchives::isArchiveRecordPresent(QIODevice *pDevice, const QString &sRecordFileName)
{
    bool bResult = false;

    QSet<XBinary::FT> stFileTypes = XBinary::getFileTypes(pDevice, true);

    // TODO more
    if (stFileTypes.contains(XArchive::FT_ZIP)) {
        XZip xzip(pDevice);

        bResult = xzip.isArchiveRecordPresent(sRecordFileName);
    } else if (stFileTypes.contains(XArchive::FT_MACHOFAT)) {
        XMACHOFat xmachofat(pDevice);

        bResult = xmachofat.isArchiveRecordPresent(sRecordFileName);
    } else if (stFileTypes.contains(XArchive::FT_AR)) {
        X_Ar x_ar(pDevice);

        bResult = x_ar.isArchiveRecordPresent(sRecordFileName);
    } else if (stFileTypes.contains(XArchive::FT_GZIP)) {
        XGzip xgzip(pDevice);

        bResult = xgzip.isArchiveRecordPresent(sRecordFileName);
    } else if (stFileTypes.contains(XArchive::FT_ZLIB)) {
        XZlib xzlib(pDevice);

        bResult = xzlib.isArchiveRecordPresent(sRecordFileName);
    } else if (stFileTypes.contains(XArchive::FT_LHA)) {
        XLHA xlha(pDevice);

        bResult = xlha.isArchiveRecordPresent(sRecordFileName);
    }

    return bResult;
}

bool XArchives::isArchiveRecordPresent(const QString &sFileName, const QString &sRecordFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        bResult = isArchiveRecordPresent(&file, sRecordFileName);
        file.close();
    }

    return bResult;
}

bool XArchives::isArchiveOpenValid(QIODevice *pDevice, const QSet<XBinary::FT> &stAvailable)
{
    bool bResult = false;

    QSet<XBinary::FT> _stAvailable = stAvailable;

    if (pDevice) {
        QSet<XBinary::FT> stFT = XBinary::getFileTypes(pDevice, true);

        if (!_stAvailable.count()) {
            _stAvailable.insert(XBinary::FT_ZIP);
            _stAvailable.insert(XBinary::FT_MACHOFAT);
            _stAvailable.insert(XBinary::FT_AR);
            _stAvailable.insert(XBinary::FT_GZIP);
            _stAvailable.insert(XBinary::FT_ZLIB);
            _stAvailable.insert(XBinary::FT_LHA);
        }

        bResult = XBinary::isFileTypePresent(&stFT, &_stAvailable);
    }

    return bResult;
}

bool XArchives::isArchiveOpenValid(const QString &sFileName, const QSet<XBinary::FT> &stAvailable)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        bResult = isArchiveOpenValid(&file, stAvailable);
        file.close();
    }

    return bResult;
}

void XArchives::_findFiles(const QString &sDirectoryName, QList<XArchive::RECORD> *pListRecords, qint32 nLimit, XBinary::PDSTRUCT *pPdStruct)
{
    if (!(pPdStruct->bIsStop)) {
        if ((nLimit < pListRecords->count()) || (nLimit == -1)) {
            QFileInfo fi(sDirectoryName);

            if (fi.isFile()) {
                XArchive::RECORD record = {};

                record.compressMethod = XArchive::COMPRESS_METHOD_FILE;
                record.sFileName = fi.absoluteFilePath();
                record.nCompressedSize = fi.size();
                record.nUncompressedSize = fi.size();

                if ((nLimit < pListRecords->count()) || (nLimit == -1)) {
                    pListRecords->append(record);
                }
            } else if (fi.isDir()) {
                QDir dir(sDirectoryName);

                QFileInfoList eil = dir.entryInfoList();

                qint32 nNumberOfFiles = eil.count();

                for (qint32 i = 0; (i < nNumberOfFiles) && (!(pPdStruct->bIsStop)); i++) {
                    QString sFN = eil.at(i).fileName();

                    if ((sFN != ".") && (sFN != "..")) {
                        _findFiles(eil.at(i).absoluteFilePath(), pListRecords, nLimit, pPdStruct);
                    }
                }
            }
        }
    }
}
