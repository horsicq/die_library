/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
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
#ifndef SCANITEM_H
#define SCANITEM_H

#include "xbinary.h"

class ScanItem {
public:
    ScanItem(const QString &sString, ScanItem *pItemParent = nullptr, qint32 nNumberOfColumns = 1, bool bIsParent = false);
    ~ScanItem();

    void appendChild(ScanItem *pItemChild);
    ScanItem *child(int nRow);
    int childCount() const;
    int columnCount() const;
    QVariant data(int nColumn) const;
    void setScanStruct(const XBinary::SCANSTRUCT &scanStruct);
    XBinary::SCANSTRUCT scanStruct() const;
    int row() const;
    ScanItem *getParentItem();

private:
    QList<ScanItem *> g_listChildItems;
    QString g_sString;
    ScanItem *g_pParentItem;
    qint32 g_nNumberOfColumns;
    bool g_bIsParent;
    XBinary::SCANSTRUCT g_scanStruct;
};

#endif  // SCANITEM_H
