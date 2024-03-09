/* Copyright (c) 2022-2024 hors<horsicq@gmail.com>
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
#include "x_ar.h"

X_Ar::X_Ar(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool X_Ar::isValid(PDSTRUCT *pPdStruct)
{
    // TODO more checks
    bool bResult = false;

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    if (getSize() > (qint64)(8 + sizeof(RECORD)))  // TODO const
    {
        if (compareSignature(&memoryMap, "'!<arch>'0a", 0, pPdStruct)) {
            bResult = true;
        }
    }

    return bResult;
}

bool X_Ar::isValid(QIODevice *pDevice)
{
    X_Ar x_ar(pDevice);

    return x_ar.isValid();
}

quint64 X_Ar::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    quint64 nResult = 0;

    qint64 nOffset = 0;
    qint64 nSize = getSize();

    nOffset += 8;
    nSize -= 8;

    while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
        char fileSize[16];

        read_array(nOffset + offsetof(FRECORD, fileSize), fileSize, 10);

        QString sSize = QString(fileSize);

        sSize.resize(10);

        qint32 nRecordSize = sSize.trimmed().toULongLong();

        if (nRecordSize == 0) {
            break;
        }

        nOffset += sizeof(FRECORD);
        nOffset += S_ALIGN_UP(nRecordSize, 2);

        nSize -= sizeof(FRECORD);
        nSize -= S_ALIGN_UP(nRecordSize, 2);

        nResult++;
    }

    return nResult;
}

QList<XArchive::RECORD> X_Ar::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    // TODO Limit
    // TODO GetMemoryMap

    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<XArchive::RECORD> listRecords;

    qint64 nOffset = 0;
    qint64 nSize = getSize();

    nOffset += 8;
    nSize -= 8;

    QString sList;

    while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
        RECORD record = {};
        record.nHeaderOffset = nOffset;

        FRECORD frecord = readFRECORD(nOffset);

        QString sSize = QString(frecord.fileSize);

        sSize.resize(sizeof(frecord.fileSize));

        qint32 nRecordSize = sSize.trimmed().toULongLong();

        if (nRecordSize == 0) {
            break;
        }

        record.sFileName = frecord.fileId;
        record.sFileName.resize(sizeof(frecord.fileId));
        record.sFileName = record.sFileName.trimmed();

        if (record.sFileName == "//")  // Linux/GNU
        {
            sList = read_ansiString(nOffset + sizeof(FRECORD), nRecordSize);
        }

        if (record.sFileName.section("/", 0, 0) == "#1")  // BSD style
        {
            qint32 nFileNameLength = record.sFileName.section("/", 1, 1).toInt();

            record.sFileName = read_ansiString(nOffset + sizeof(FRECORD), nFileNameLength);  // TODO Check UTF8

            record.nDataOffset = nOffset + sizeof(FRECORD) + nFileNameLength;
            record.nCompressedSize = nRecordSize - nFileNameLength;
            record.nUncompressedSize = nRecordSize - nFileNameLength;
            record.nHeaderSize = record.nDataOffset - record.nHeaderOffset;
        } else {
            qint32 nFileNameSie = record.sFileName.size();

            if (nFileNameSie >= 2)  // Linux/GNU
            {
                if ((record.sFileName.at(0) == QChar('/')) && (record.sFileName.at(nFileNameSie - 1) != QChar('/'))) {
                    qint32 nIndex = record.sFileName.section("/", 1, 1).toULong();

                    if (nIndex < sList.size()) {
                        if (nIndex) {
                            record.sFileName = sList.right(nIndex).section("/", 0, 0);
                        } else {
                            record.sFileName = sList.section("/", 0, 0);
                        }
                    }
                } else if ((nFileNameSie > 2) && (record.sFileName.at(nFileNameSie - 1) == QChar('/'))) {
                    record.sFileName.remove(nFileNameSie - 1, 1);
                }
            }

            record.nDataOffset = nOffset + sizeof(FRECORD);
            record.nCompressedSize = nRecordSize;
            record.nUncompressedSize = nRecordSize;
        }

        if (record.nCompressedSize < 0) {
            record.nCompressedSize = 0;
        }

        if (record.nUncompressedSize < 0) {
            record.nUncompressedSize = 0;
        }

        record.compressMethod = COMPRESS_METHOD_STORE;

        listRecords.append(record);

        nOffset += sizeof(FRECORD);
        nOffset += S_ALIGN_UP(nRecordSize, 2);

        nSize -= sizeof(FRECORD);
        nSize -= S_ALIGN_UP(nRecordSize, 2);
    }

    return listRecords;
}

QString X_Ar::getFileFormatExt()
{
    return "ar";  // TODO
}

QList<XBinary::MAPMODE> X_Ar::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

X_Ar::FRECORD X_Ar::readFRECORD(qint64 nOffset)
{
    FRECORD record = {};

    read_array(nOffset + offsetof(FRECORD, fileId), record.fileId, sizeof(record.fileId));
    read_array(nOffset + offsetof(FRECORD, fileMod), record.fileMod, sizeof(record.fileMod));
    read_array(nOffset + offsetof(FRECORD, ownerId), record.ownerId, sizeof(record.ownerId));
    read_array(nOffset + offsetof(FRECORD, groupId), record.groupId, sizeof(record.groupId));
    read_array(nOffset + offsetof(FRECORD, fileMode), record.fileMode, sizeof(record.fileMode));
    read_array(nOffset + offsetof(FRECORD, fileSize), record.fileSize, sizeof(record.fileSize));
    read_array(nOffset + offsetof(FRECORD, endChar), record.endChar, sizeof(record.endChar));

    return record;
}
