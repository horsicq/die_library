/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#ifndef MSDOS_SCRIPT_H
#define MSDOS_SCRIPT_H

#include "binary_script.h"
#include "xmsdos.h"

class MSDOS_Script : public Binary_Script {
    Q_OBJECT

public:
    explicit MSDOS_Script(XMSDOS *pMSDOS, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);
    ~MSDOS_Script();

public slots:
    bool isLE();
    bool isLX();
    bool isNE();
    bool isPE();
    qint64 getDosStubOffset();
    qint64 getDosStubSize();
    bool isDosStubPresent();
    bool isRichSignaturePresent();
    qint32 getNumberOfRichIDs();
    bool isRichVersionPresent(quint32 nVersion);
    quint32 getRichVersion(qint32 nPosition);
    quint32 getRichID(qint32 nPosition);
    quint32 getRichCount(qint32 nPosition);

private:
    XMSDOS *g_pMSDOS;
    QList<XMSDOS::MS_RICH_RECORD> listRich;
    qint32 nNumberOfRichIDs;
    qint64 nDosStubOffset;
    qint64 nDosStubSize;
    bool bIsDosStubPresent;
    bool bIisRichSignaturePresent;
    bool bIsLE;
    bool bIsLX;
    bool bIsNE;
    bool bIsPE;
};

#endif  // MSDOS_SCRIPT_H
