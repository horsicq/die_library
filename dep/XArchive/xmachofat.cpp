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
#include "xmachofat.h"

XMACHOFat::XMACHOFat(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool XMACHOFat::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    quint32 nMagic = read_uint32(0);

    if ((nMagic == XMACH_DEF::S_FAT_MAGIC) || (nMagic == XMACH_DEF::S_FAT_CIGAM)) {
        PDSTRUCT pdStruct = XBinary::createPdStruct();

        bResult = (getNumberOfRecords(&pdStruct) < 10);  // TODO Check !!!
    }

    return bResult;
}

bool XMACHOFat::isValid(QIODevice *pDevice)
{
    XMACHOFat xmachofat(pDevice);

    return xmachofat.isValid();
}

XBinary::ENDIAN XMACHOFat::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint32 nData = read_uint32(0);

    if (nData == XMACH_DEF::S_FAT_MAGIC) {
        result = ENDIAN_LITTLE;
    } else if (nData == XMACH_DEF::S_FAT_CIGAM) {
        result = ENDIAN_BIG;
    }

    return result;
}

quint64 XMACHOFat::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return read_uint32(offsetof(XMACH_DEF::fat_header, nfat_arch), isBigEndian());
}

QList<XArchive::RECORD> XMACHOFat::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<RECORD> listResult;

    qint32 nNumberOfRecords = (qint32)getNumberOfRecords(pPdStruct);

    if (nLimit != -1) {
        nNumberOfRecords = qMin(nNumberOfRecords, nLimit);
    }

    bool bIsBigEndian = isBigEndian();

    QMap<quint64, QString> mapCpuTypes = XMACH::getHeaderCpuTypesS();

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        qint64 nOffset = sizeof(XMACH_DEF::fat_header) + i * sizeof(XMACH_DEF::fat_arch);

        quint32 _cputype = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, cputype), bIsBigEndian);
        quint32 _cpusubtype = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, cpusubtype), bIsBigEndian);
        quint32 _offset = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, offset), bIsBigEndian);
        quint32 _size = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, size), bIsBigEndian);
        //        quint32 _align=read_uint32(nOffset+offsetof(XMACH_DEF::fat_arch,align),bIsBigEndian);

        RECORD record = {};

        record.sFileName = QString("%1").arg(mapCpuTypes.value(_cputype, tr("Unknown")));

        if (_cpusubtype) {
            record.sFileName += QString("-%1").arg(_cpusubtype, 0, 16);
        }

        record.nHeaderOffset = nOffset;
        record.nHeaderSize = sizeof(XMACH_DEF::fat_arch);
        record.nDataOffset = _offset;
        record.nCompressedSize = _size;
        record.nUncompressedSize = _size;
        record.compressMethod = COMPRESS_METHOD_STORE;

        listResult.append(record);
    }

    return listResult;
}

QString XMACHOFat::getFileFormatExt()
{
    return "machofat";
}

qint64 XMACHOFat::getFileFormatSize()
{
    return _calculateRawSize();
}

QString XMACHOFat::getFileFormatString()
{
    QString sResult;

    sResult = QString("MACHOFAT");
    // TODO more info, number of records

    return sResult;
}

QList<XBinary::MAPMODE> XMACHOFat::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XMACHOFat::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    Q_UNUSED(pPdStruct)

    XBinary::_MEMORY_MAP result = {};

    result.endian = getEndian();
    result.nBinarySize = getSize();

    bool bIsBigEndian = (result.endian == ENDIAN_BIG);

    qint32 nIndex = 0;

    {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.type = MMT_HEADER;
        record.nOffset = 0;
        record.nSize = sizeof(XMACH_DEF::fat_header);
        record.nAddress = -1;
        record.sName = tr("Header");

        result.listRecords.append(record);
    }

    quint32 nNumberOfRecords = read_uint32(offsetof(XMACH_DEF::fat_header, nfat_arch), bIsBigEndian);

    QMap<quint64, QString> mapCpuTypes = XMACH::getHeaderCpuTypesS();

    for (qint32 i = 0; i < (qint32)nNumberOfRecords; i++) {
        _MEMORY_RECORD record = {};

        qint64 nOffset = sizeof(XMACH_DEF::fat_header) + i * sizeof(XMACH_DEF::fat_arch);

        quint32 _cputype = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, cputype), bIsBigEndian);
        quint32 _cpusubtype = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, cpusubtype), bIsBigEndian);
        quint32 _offset = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, offset), bIsBigEndian);
        quint32 _size = read_uint32(nOffset + offsetof(XMACH_DEF::fat_arch, size), bIsBigEndian);

        record.sName = QString("%1").arg(mapCpuTypes.value(_cputype, tr("Unknown")));

        if (_cpusubtype) {
            record.sName += QString("-%1").arg(_cpusubtype, 0, 16);
        }

        record.nOffset = _offset;
        record.nSize = _size;
        record.nAddress = -1;
        record.type = MMT_LOADSEGMENT;

        result.listRecords.append(record);
    }

    return result;
}
