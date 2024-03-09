/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#ifndef XANDROIDBINARY_H
#define XANDROIDBINARY_H

#include <QStack>
#include <QXmlStreamWriter>

#include "xandroidbinary_def.h"
#include "xbinary.h"

class XAndroidBinary : public XBinary {
    Q_OBJECT

public:
    struct RECORD {
        qint64 nOffset;
        XANDROIDBINARY_DEF::HEADER header;
        QList<RECORD> listChildren;
    };

    XAndroidBinary(QIODevice *pDevice);

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    virtual ENDIAN getEndian();
    virtual QString getVersion();
    XANDROIDBINARY_DEF::HEADER readHeader(qint64 nOffset);
    XANDROIDBINARY_DEF::HEADER_STRING_POOL readHeaderStringPool(qint64 nOffset);
    XANDROIDBINARY_DEF::HEADER_NAMESPACE readHeaderNamespace(qint64 nOffset);
    XANDROIDBINARY_DEF::HEADER_XML_START readHeaderXmlStart(qint64 nOffset);
    XANDROIDBINARY_DEF::HEADER_XML_ATTRIBUTE readHeaderXmlAttribute(qint64 nOffset);
    XANDROIDBINARY_DEF::HEADER_XML_END readHeaderXmlEnd(qint64 nOffset);
    QList<XANDROIDBINARY_DEF::HEADER> getHeaders();
    RECORD getRecord(qint64 nOffset);
    QString recordToString(RECORD *pRecord);
    static QString getDecoded(QIODevice *pDevice);
    static QString getDecoded(const QString &sFileName);
    static QString getDecoded(QByteArray *pbaData);
};

#endif  // XANDROIDBINARY_H
