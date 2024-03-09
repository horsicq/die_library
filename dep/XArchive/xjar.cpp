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
#include "xjar.h"

XJAR::XJAR(QIODevice *pDevice) : XZip(pDevice)
{
}

bool XJAR::isValid(PDSTRUCT *pPdStruct)  // PDSTRUCT
{
    bool bResult = false;

    XZip xzip(getDevice());

    if (xzip.isValid()) {
        QList<XArchive::RECORD> listArchiveRecords = xzip.getRecords(20000, pPdStruct);

        bResult = isValid(&listArchiveRecords, pPdStruct);
    }

    return bResult;
}

bool XJAR::isValid(QIODevice *pDevice)
{
    XJAR xjar(pDevice);

    return xjar.isValid();
}

bool XJAR::isValid(QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    bResult = XArchive::isArchiveRecordPresent("META-INF/MANIFEST.MF", pListRecords, pPdStruct);

    return bResult;
}

XBinary::FT XJAR::getFileType()
{
    return FT_JAR;
}

XBinary::FILEFORMATINFO XJAR::getFileFormatInfo()
{
    XBinary::FILEFORMATINFO result = {};

    XJAR xjar(getDevice());

    if (xjar.isValid()) {
        result.bIsValid = true;
        result.nSize = xjar.getFileFormatSize();
        result.sString = "JAR";
        result.sExt = "jar";
        result.fileType = FT_JAR;
    }

    return result;
}

QString XJAR::getFileFormatExt()
{
    return "jar";
}

XBinary::OSINFO XJAR::getOsInfo()
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    QList<XArchive::RECORD> listRecords = getRecords(20, &pdStructEmpty);

    return getOsInfo(&listRecords, &pdStructEmpty);
}

XBinary::OSINFO XJAR::getOsInfo(QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct)
{
    XBinary::OSINFO result = {};

    result.osName = OSNAME_JVM;
    result.bIsVM = true;

    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    qint32 nNumberOfRecords = pListRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListRecords->at(i).sFileName.section(".", -1, -1) == "class") {
            RECORD record = pListRecords->at(i);
            QByteArray baData = XArchive::decompress(&record, true, pPdStruct);

            if (baData.size() > 10) {
                char *pData = baData.data();
                if (XBinary::_read_uint32(pData, true) == 0xCAFEBABE) {
                    quint16 nMinor = XBinary::_read_uint16(pData + 4, true);
                    quint16 nMajor = XBinary::_read_uint16(pData + 6, true);

                    result.sOsVersion = _getJDKVersion(nMajor, nMinor);

                    break;
                }
            }
        }
    }

    return result;
}

XBinary::ENDIAN XJAR::getEndian()
{
    return ENDIAN_UNKNOWN;
}

XBinary::MODE XJAR::getMode()
{
    return MODE_DATA;
}

QString XJAR::getArch()
{
    return tr("Universal");
}

qint32 XJAR::getType()
{
    return TYPE_PACKAGE;
}

QString XJAR::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_PACKAGE: sResult = tr("Package");
    }

    return sResult;
}

QString XJAR::_getJDKVersion(quint16 nMajor, quint16 nMinor)
{
    QString sResult;

    switch (nMajor) {
        case 0x2D: sResult = "JDK 1.1"; break;
        case 0x2E: sResult = "JDK 1.2"; break;
        case 0x2F: sResult = "JDK 1.3"; break;
        case 0x30: sResult = "JDK 1.4"; break;
        case 0x31: sResult = "Java SE 5.0"; break;
        case 0x32: sResult = "Java SE 6"; break;
        case 0x33: sResult = "Java SE 7"; break;
        case 0x34: sResult = "Java SE 8"; break;
        case 0x35: sResult = "Java SE 9"; break;
        case 0x36: sResult = "Java SE 10"; break;
        case 0x37: sResult = "Java SE 11"; break;
        case 0x38: sResult = "Java SE 12"; break;
        case 0x39: sResult = "Java SE 13"; break;
        case 0x3A: sResult = "Java SE 14"; break;
        case 0x3B: sResult = "Java SE 15"; break;
        case 0x3C: sResult = "Java SE 16"; break;
        case 0x3D: sResult = "Java SE 17"; break;
        case 0x3E: sResult = "Java SE 18"; break;
        case 0x3F: sResult = "Java SE 19"; break;
        case 0x40: sResult = "Java SE 20"; break;
        case 0x41: sResult = "Java SE 21"; break;
    }

    if ((sResult != "") && (nMinor)) {
        sResult += QString(".%1").arg(nMinor);
    }

    return sResult;
}
