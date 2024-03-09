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
#ifndef X_AR_H
#define X_AR_H

#include "xarchive.h"

class X_Ar : public XArchive {
    Q_OBJECT

#pragma pack(push)
#pragma pack(1)
    struct FRECORD {
        char fileId[16];    // File identifier                          ASCII
        char fileMod[12];   // File modification timestamp (in seconds) Decimal
        char ownerId[6];    // Owner ID                                 Decimal
        char groupId[6];    // Group ID                                 Decimal
        char fileMode[8];   // File mode (type and permission)          Octal
        char fileSize[10];  // File size in bytes                       Decimal
        char endChar[2];    // Ending characters 0x60 0x0A
    };
#pragma pack(pop)

public:
    explicit X_Ar(QIODevice *pDevice = nullptr);

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice);
    virtual quint64 getNumberOfRecords(PDSTRUCT *pPdStruct);
    virtual QList<RECORD> getRecords(qint32 nLimit, PDSTRUCT *pPdStruct);
    virtual QString getFileFormatExt();
    virtual QList<MAPMODE> getMapModesList(PDSTRUCT *pPdStruct = nullptr);

private:
    FRECORD readFRECORD(qint64 nOffset);
};

#endif  // X_AR_H
