/* Copyright (c) 2022-2023 hors<horsicq@gmail.com>
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
#ifndef XPDF_H
#define XPDF_H

#include "xbinary.h"
#include "xpdf_def.h"

class XPDF : public XBinary {
    Q_OBJECT

    struct TRAILERRECORD {
        QString sName;
        QString sValue;
    };

public:
    struct STARTHREF {
        qint64 nXrefOffset;
        qint64 nFooterOffset;
        qint64 nFooterSize;
    };

    XPDF(QIODevice *pDevice);

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    virtual QString getVersion();
    virtual FT getFileType();
    virtual ENDIAN getEndian();
    virtual qint64 getFileFormatSize();
    virtual QString getFileFormatString();
    virtual QString getFileFormatExt();

    virtual QList<MAPMODE> getMapModesList(PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);

    STARTHREF findStartxref();
    QList<TRAILERRECORD> readTrailer();
    OS_STRING _readPDFString(qint64 nOffset);
    OS_STRING readPDFValue(qint64 nOffset);
    void getInfo();
    qint64 getObjectSize(qint64 nOffset);
};

#endif  // XPDF_H
