/* Copyright (c) 2022-2024 hors<horsicq@gmail.com>
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
#include "xpdf.h"

XPDF::XPDF(QIODevice *pDevice) : XBinary(pDevice)
{
}

bool XPDF::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    // TODO more checks !!!
    // 1.0-2.0
    if (getSize() > 4) {
        if (read_uint32(0) == 0x46445025)  // '%PDF'
        {
            bResult = true;
        }
    }

    return bResult;
}

QString XPDF::getVersion()
{
    QString sResult;

    sResult = read_ansiString(5, 4).remove("\r").remove("\n");

    return sResult;
}

XBinary::FT XPDF::getFileType()
{
    return FT_PDF;
}

XBinary::ENDIAN XPDF::getEndian()
{
    return ENDIAN_UNKNOWN;
}

qint64 XPDF::getFileFormatSize()
{
    // TODO Check if 2 PDFs or PDF in PDF
    // TODO Check edited PDF
    qint64 nResult = 0;
    qint64 nOffset = 0;

    while (true) {
        qint64 nCurrent = find_signature(nOffset, -1, "'startxref'");  // TODO PDStruct

        if (nCurrent != -1) {
            OS_STRING osStartXref = _readPDFString(nCurrent);

            nCurrent += osStartXref.nSize;

            OS_STRING osOffset = _readPDFString(nCurrent);

            qint64 _nOffset = osOffset.sString.toLongLong();

            if ((_nOffset > 0) && (_nOffset < nCurrent)) {
                nCurrent += osOffset.nSize;

                OS_STRING osEnd = _readPDFString(nCurrent);

                if (osEnd.sString == "%%EOF") {
                    nCurrent += osEnd.nSize;
                    nResult = nCurrent;

                    break;
                }
            }
        } else {
            nResult = 0;
            break;
        }

        nOffset = nCurrent + 10;
    }

    return nResult;
}

QString XPDF::getFileFormatString()
{
    QString sResult;

    sResult = QString("PDF(%1)").arg(getVersion());
    // TODO more info

    return sResult;
}

QString XPDF::getFileFormatExt()
{
    return "pdf";
}

QList<XBinary::MAPMODE> XPDF::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XPDF::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)
    // TODO Check streams
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};

    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    {
        OS_STRING osHeader = _readPDFString(0);

        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.type = MMT_HEADER;
        record.nOffset = 0;
        record.nSize = osHeader.nSize + 1;
        record.nAddress = -1;
        record.sName = tr("Header");

        result.listRecords.append(record);
    }

    STARTHREF startxref = findStartxref();

    OS_STRING osHref = _readPDFString(startxref.nXrefOffset);

    {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.type = MMT_DATA;
        record.nOffset = startxref.nXrefOffset;
        record.nSize = 10;
        record.nAddress = -1;

        result.listRecords.append(record);
    }

    if (osHref.sString == "xref") {
        qint64 nCurrentOffset = startxref.nXrefOffset + osHref.nSize;

        while (!pPdStruct->bIsStop) {
            OS_STRING osSection = _readPDFString(nCurrentOffset);

            quint64 nID = osSection.sString.section(" ", 0, 0).toULongLong();
            quint64 nNumberOfObjects = osSection.sString.section(" ", 1, 1).toULongLong();

            nCurrentOffset += osSection.nSize;

            if (nNumberOfObjects) {
                for (quint64 i = 0; i < nNumberOfObjects; i++) {
                    OS_STRING osObject = _readPDFString(nCurrentOffset);

                    if (i > 0) {
                        qint64 nObjectOffset = osObject.sString.section(" ", 0, 0).toULongLong();

                        {
                            _MEMORY_RECORD record = {};

                            record.nIndex = nIndex++;
                            record.type = MMT_OBJECT;
                            record.nOffset = nObjectOffset;
                            record.nSize = getObjectSize(nObjectOffset);
                            record.nAddress = -1;
                            record.nID = nID + i;
                            record.sName = QString("%1 %2").arg(tr("Object"), QString::number(record.nID));

                            result.listRecords.append(record);
                        }
                    }

                    nCurrentOffset += osObject.nSize;
                }
            } else {
                break;
            }
        }
    }

    {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.type = MMT_FOOTER;
        record.nOffset = startxref.nFooterOffset;
        record.nSize = startxref.nFooterSize;
        record.nAddress = -1;
        record.sName = tr("Footer");

        result.listRecords.append(record);
    }

    return result;
}

XPDF::STARTHREF XPDF::findStartxref()
{
    STARTHREF result = {};

    qint64 nSize = getSize();

    qint64 nOffset = qMax((qint64)0, nSize - 0x1000);  // TODO const

    while (true) {
        qint64 nStartXref = find_signature(nOffset, -1, "'startxref'");  // \n \r

        if (nStartXref != -1) {
            qint64 nCurrent = nStartXref;

            OS_STRING osStartXref = _readPDFString(nCurrent);

            nCurrent += osStartXref.nSize;

            OS_STRING osOffset = _readPDFString(nCurrent);

            qint64 _nOffset = osOffset.sString.toLongLong();

            if ((_nOffset > 0) && (_nOffset < nCurrent)) {
                nCurrent += osOffset.nSize;

                OS_STRING osEnd = _readPDFString(nCurrent);

                if (osEnd.sString == "%%EOF") {
                    nCurrent += osEnd.nSize;

                    result.nXrefOffset = _nOffset;
                    result.nFooterOffset = nStartXref;
                    result.nFooterSize = nCurrent - nStartXref;

                    break;
                }
            }
        } else {
            break;
        }

        nOffset = nStartXref + 10;  // Get the last
    }

    return result;
}

QList<XPDF::TRAILERRECORD> XPDF::readTrailer()
{
    QList<XPDF::TRAILERRECORD> listResult;

    qint64 nSize = getSize();

    qint64 nOffset = qMax((qint64)0, nSize - 0x1000);  // TODO const

    bool bFound = false;

    while (true) {
        qint64 nCurrent = find_signature(nOffset, -1, "'trailer'");

        if (nCurrent == -1) {
            break;
        }

        bFound = true;

        nOffset = nCurrent + 8;  // Get the last
    }

    if (bFound) {
        bool bValid = false;

        while (true) {
            OS_STRING osString = _readPDFString(nOffset);

            if (osString.sString == "<<") {
                bValid = true;
            } else if (bValid && XBinary::isRegExpPresent("^\\/", osString.sString)) {
                QString _sRecord = osString.sString.section("/", 1, -1);

                TRAILERRECORD record = {};

                record.sName = _sRecord.section(" ", 0, 0);
                record.sValue = _sRecord.section(" ", 1, -1);

                listResult.append(record);
            } else if ((osString.sString == "") || (osString.sString == ">>")) {
                break;
            }

            nOffset += osString.nSize;
        }
    }

    return listResult;
}

XBinary::OS_STRING XPDF::_readPDFString(qint64 nOffset)
{
    OS_STRING result = {};

    result.nOffset = nOffset;

    // TODO optimize
    for (qint32 i = 0; i < 65535; i++) {
        QString sSymbol = read_ansiString(nOffset + i, 1);

        if (sSymbol != "") {
            result.nSize++;
        }

        if ((sSymbol == "") || (sSymbol == "\r") || (sSymbol == "\n"))  // TODO more checks
        {
            break;
        }

        result.sString.append(sSymbol);

        if (sSymbol == "(") {
            OS_STRING _unicode = readPDFValue(nOffset + i + 1);

            result.sString.append(_unicode.sString);
            i += _unicode.nSize;
            result.nSize += _unicode.nSize;
        }
    }

    return result;
}

XBinary::OS_STRING XPDF::readPDFValue(qint64 nOffset)
{
    OS_STRING result = {};

    result.nOffset = nOffset;

    quint16 nBOF = read_uint16(nOffset);

    if (nBOF == 0xFFFE) {
        nOffset += 2;

        qint64 _nOffset = nOffset;

        while (true) {
            quint16 nWord = read_uint16(_nOffset, true);
            if ((nWord == 0) || (nWord == 0x290a)) {
                break;
            }

            _nOffset += 2;
        }

        qint32 nSize = (_nOffset - nOffset) / 2;

        result.sString = read_unicodeString(nOffset, nSize, true);
        result.nSize = 2 + result.sString.size() * 2;
    }

    return result;
}

void XPDF::getInfo()
{
    //    // TODO all streams
    //    QList<TRAILERRECORD> listRecords = readTrailer();
    //    qint64 nStartxref = findStartxref();

    //    QList<XPDF_DEF::OBJECT> listObjects;

    //    if (nStartxref != -1) {
    //        // TODO "xref"
    //        qint64 nOffset = nStartxref;

    //        OS_STRING osRecord = readPDFString(nOffset);

    //        if (osRecord.sString == "xref") {
    //            // Cross-Reference Table

    //        } else {
    //            // The cross-reference stream object
    //        }

    //        bool bValid = false;

    //        while (true)  // TODO size from trailer
    //        {
    //            OS_STRING osRecord = readPDFString(nOffset);

    //            if (osRecord.sString == "") {
    //                break;
    //            }

    //            if (osRecord.sString == "xref") {
    //                bValid = true;
    //            } else {
    //                QString sID = osRecord.sString.section(" ", 0, 0);
    //                qint32 nNumberOfObjects = osRecord.sString.section(" ", 1, 1).toUInt();

    //                bool bLast = false;

    //                //                if(sID==listRecords.value("Size"))
    //                //                {
    //                //                    bLast=true;
    //                //                }

    //                nOffset += osRecord.nSize;

    //                for (qint32 i = 0; i < nNumberOfObjects; i++) {
    //                    OS_STRING osString = readPDFString(nOffset);

    //                    XPDF_DEF::OBJECT record = {};

    //                    record.nOffset = osString.sString.section(" ", 0, 0).toULongLong();
    //                    record.nID = sID.toULongLong() + osString.sString.section(" ", 1, 1).toULongLong();

    //                    QString sStatus = osString.sString.section(" ", 2, 2);

    //                    if (sStatus == "f") {
    //                        record.bIsFree = true;
    //                    }

    //                    record.nSize = getObjectSize(record.nOffset);

    //                    listObjects.append(record);

    //                    qDebug("%s", osString.sString.toLatin1().data());

    //                    nOffset += osString.nSize;
    //                }

    //                if (bLast) {
    //                    break;
    //                }
    //            }

    //            qDebug("%s", osRecord.sString.toLatin1().data());

    //            nOffset += osRecord.nSize;  // Check
    //        }
    //    }

    //    qint32 nNumberOfRecords = listObjects.count();

    //    for (qint32 i = 0; i < nNumberOfRecords; i++) {
    //        QString sRecord = QString("%1 %2 %3").arg(QString::number(i), QString::number(listObjects.at(i).nOffset), QString::number(listObjects.at(i).nSize));

    //        qDebug("%s", sRecord.toLatin1().data());
    //    }
}

qint64 XPDF::getObjectSize(qint64 nOffset)
{
    qint64 _nOffset = nOffset;

    while (true) {
        // TODO Read Object
        OS_STRING osString = _readPDFString(_nOffset);
        _nOffset += osString.nSize;

        if (osString.sString == "") {
            break;
        }
        // TODO XXX XXX "obj"
        if ((osString.sString == "endobj")) {
            break;
        }
    }

    return _nOffset - nOffset;
}
