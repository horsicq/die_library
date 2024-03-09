/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
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
#include "xcab.h"

XCab::XCab(QIODevice *pDevice) : XArchive(pDevice)
{
}

bool XCab::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() > (qint64)sizeof(CFHEADER)) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);
        if (compareSignature(&memoryMap, "'MSCF'00........00", 0, pPdStruct)) {
            bResult = true;
        }
    }

    return bResult;
}

bool XCab::isValid(QIODevice *pDevice)
{
    XCab xcab(pDevice);

    return xcab.isValid();
}

QString XCab::getVersion()
{
    return QString("%1.%2").arg(read_uint8(25)).arg(read_uint8(24), 2, 10, QChar('0'));
}

quint64 XCab::getNumberOfRecords(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    quint64 nResult = 0;

    nResult = read_uint16(offsetof(CFHEADER, cFolders)) + read_uint16(offsetof(CFHEADER, cFiles));

    return nResult;
}

QList<XArchive::RECORD> XCab::getRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    Q_UNUSED(pPdStruct)

    QList<XArchive::RECORD> listResult;

    qint64 nOffset = 0;  // TODO

    CFHEADER cfHeader = readCFHeader();

    nOffset += sizeof(CFHEADER) - 4;

    if (cfHeader.flags & 0x0004)  // TODO const
    {
        nOffset += 4;

        nOffset += cfHeader.cbCFHeader;
    }

    // CFFOLDER cfFolder=readCFFolder(nOffset);

    // TODO

    return listResult;
}

XCab::CFHEADER XCab::readCFHeader()
{
    CFHEADER result = {};

    result.signature[0] = read_uint8(0);
    result.signature[1] = read_uint8(1);
    result.signature[2] = read_uint8(2);
    result.signature[3] = read_uint8(3);
    result.reserved1 = read_uint32(offsetof(CFHEADER, reserved1));
    result.cbCabinet = read_uint32(offsetof(CFHEADER, cbCabinet));
    result.reserved2 = read_uint32(offsetof(CFHEADER, reserved2));
    result.coffFiles = read_uint32(offsetof(CFHEADER, coffFiles));
    result.reserved3 = read_uint32(offsetof(CFHEADER, reserved3));
    result.versionMinor = read_uint8(offsetof(CFHEADER, versionMinor));
    result.versionMajor = read_uint8(offsetof(CFHEADER, versionMajor));
    result.cFolders = read_uint16(offsetof(CFHEADER, cFolders));
    result.cFiles = read_uint16(offsetof(CFHEADER, cFiles));
    result.flags = read_uint16(offsetof(CFHEADER, flags));
    result.setID = read_uint16(offsetof(CFHEADER, setID));
    result.iCabinet = read_uint16(offsetof(CFHEADER, iCabinet));

    if (result.flags & 0x0004)  // TODO const
    {
        result.cbCFHeader = read_uint16(offsetof(CFHEADER, cbCFHeader));
        result.cbCFFolder = read_uint8(offsetof(CFHEADER, cbCFFolder));
        result.cbCFData = read_uint8(offsetof(CFHEADER, cbCFData));
    }

    return result;
}

XCab::CFFOLDER XCab::readCFFolder(qint64 nOffset)
{
    CFFOLDER result = {};

    result.coffCabStart = read_uint32(nOffset + offsetof(CFFOLDER, coffCabStart));
    result.cCFData = read_uint16(nOffset + offsetof(CFFOLDER, cCFData));
    result.typeCompress = read_uint16(nOffset + offsetof(CFFOLDER, typeCompress));

    return result;
}

XCab::CFDATA XCab::readCFData(qint64 nOffset)
{
    CFDATA result = {};

    result.csum = read_uint32(nOffset + offsetof(CFDATA, csum));
    result.cbData = read_uint16(nOffset + offsetof(CFDATA, cbData));
    result.cbUncomp = read_uint16(nOffset + offsetof(CFDATA, cbUncomp));

    return result;
}

XBinary::FT XCab::getFileType()
{
    return FT_CAB;
}

QString XCab::getFileFormatString()
{
    QString sResult;

    sResult = QString("CAB(%1)").arg(getVersion());
    // TODO more info

    return sResult;
}

QString XCab::getFileFormatExt()
{
    return "cab";
}

qint64 XCab::getFileFormatSize()
{
    qint64 nResult = 0;

    nResult = readCFHeader().cbCabinet;  // TODO check mb _getRawSize !!!

    return nResult;
}

QList<XBinary::MAPMODE> XCab::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XCab::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }
    // TODO
    return XBinary::getMemoryMap(mapMode, pPdStruct);
}
