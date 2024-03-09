/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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
#ifndef SCANITEMMODEL_H
#define SCANITEMMODEL_H

#include <QAbstractItemModel>
#if (QT_VERSION_MAJOR > 4)
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#endif
#include <QXmlStreamWriter>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#ifdef QT_GUI_LIB
#include <QApplication>
#include <QColor>
#include <QPalette>
#endif
#include "scanitem.h"

class ScanItemModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum UD {
        UD_FILETYPE = 0,
        UD_NAME,
        UD_INFO,
        UD_INFO2,
        UD_UUID
    };

    explicit ScanItemModel(QList<XBinary::SCANSTRUCT> *pListScanStructs, int nNumberOfColumns = 1);
    ~ScanItemModel();

    QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const override;
    QModelIndex index(int nRow, int nColumn, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QString toXML();
    QString toJSON();
    QString toCSV();
    QString toTSV();
    QString toFormattedString();
    void coloredOutput();
    QString toString(XBinary::FORMATTYPE formatType);
    ScanItem *rootItem();

private:
    void _toXML(QXmlStreamWriter *pXml, ScanItem *pItem, qint32 nLevel);
#if (QT_VERSION_MAJOR > 4)
    void _toJSON(QJsonObject *pJsonObject, ScanItem *pItem, qint32 nLevel);
#endif
    void _toCSV(QString *pString, ScanItem *pItem, qint32 nLevel);
    void _toTSV(QString *pString, ScanItem *pItem, qint32 nLevel);
    void _toFormattedString(QString *pString, ScanItem *pItem, qint32 nLevel);
    void _coloredOutput(ScanItem *pItem, qint32 nLevel);
    void _coloredItem(ScanItem *pItem);

private:
    ScanItem *g_pRootItem;
};

#endif  // SCANITEMMODEL_H
