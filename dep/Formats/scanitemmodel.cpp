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
#include "scanitemmodel.h"

ScanItemModel::ScanItemModel(QList<XBinary::SCANSTRUCT> *pListScanStructs, int nNumberOfColumns) : QAbstractItemModel(0)
{
    g_pRootItem = new ScanItem(tr("Result"), nullptr, nNumberOfColumns, true);
    XBinary::SCANSTRUCT emptySS = {};
    g_pRootItem->setScanStruct(emptySS);

    QMap<QString, ScanItem *> mapParents;

    qint32 nNumberOfDetects = pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfDetects; i++) {
        if (!mapParents.contains(pListScanStructs->at(i).id.sUuid)) {
            ScanItem *_pItemParent = nullptr;

            if (pListScanStructs->at(i).parentId.sUuid == "") {
                _pItemParent = g_pRootItem;
            } else {
                _pItemParent = mapParents.value(pListScanStructs->at(i).parentId.sUuid);
            }

            if (_pItemParent == nullptr) {
                _pItemParent = g_pRootItem;
            }

            QString sParent = XBinary::createTypeString(&pListScanStructs->at(i));

            ScanItem *pItemParent = new ScanItem(sParent, _pItemParent, nNumberOfColumns, true);
            XBinary::SCANSTRUCT ss = XBinary::createHeaderScanStruct(&pListScanStructs->at(i));
            pItemParent->setScanStruct(ss);
            _pItemParent->appendChild(pItemParent);

            mapParents.insert(pListScanStructs->at(i).id.sUuid, pItemParent);
        }

        if (pListScanStructs->at(i).sName != "") {
            ScanItem *pItemParent = mapParents.value(pListScanStructs->at(i).id.sUuid);

            QString sItem = XBinary::createResultString2(&pListScanStructs->at(i));
            ScanItem *pItem = new ScanItem(sItem, pItemParent, nNumberOfColumns, false);
            pItem->setScanStruct(pListScanStructs->at(i));
            pItemParent->appendChild(pItem);
        }
    }
}

ScanItemModel::~ScanItemModel()
{
    delete g_pRootItem;
}

QVariant ScanItemModel::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if ((orientation == Qt::Horizontal) && (nRole == Qt::DisplayRole)) {
        result = g_pRootItem->data(nSection);
    }

    return result;
}

QModelIndex ScanItemModel::index(int nRow, int nColumn, const QModelIndex &parent) const
{
    QModelIndex result;

    if (hasIndex(nRow, nColumn, parent)) {
        ScanItem *pParentItem = nullptr;

        if (!parent.isValid()) {
            pParentItem = g_pRootItem;
        } else {
            pParentItem = static_cast<ScanItem *>(parent.internalPointer());
        }

        ScanItem *pItemChild = pParentItem->child(nRow);

        if (pItemChild) {
            result = createIndex(nRow, nColumn, pItemChild);
        }
    }

    return result;
}

QModelIndex ScanItemModel::parent(const QModelIndex &index) const
{
    QModelIndex result;

    if (index.isValid()) {
        ScanItem *pChildItem = static_cast<ScanItem *>(index.internalPointer());
        ScanItem *pParentItem = pChildItem->getParentItem();

        if (pParentItem != g_pRootItem) {
            result = createIndex(pParentItem->row(), 0, pParentItem);
        }
    }

    return result;
}

int ScanItemModel::rowCount(const QModelIndex &parent) const
{
    int nResult = 0;

    if (parent.column() <= 0) {
        ScanItem *pParentItem = nullptr;

        if (!parent.isValid()) {
            pParentItem = g_pRootItem;
        } else {
            pParentItem = static_cast<ScanItem *>(parent.internalPointer());
        }

        nResult = pParentItem->childCount();
    }

    return nResult;
}

int ScanItemModel::columnCount(const QModelIndex &parent) const
{
    int nResult = 0;

    if (parent.isValid()) {
        nResult = static_cast<ScanItem *>(parent.internalPointer())->columnCount();
    } else {
        nResult = g_pRootItem->columnCount();
    }

    return nResult;
}

QVariant ScanItemModel::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (index.isValid()) {
        ScanItem *pItem = static_cast<ScanItem *>(index.internalPointer());

        if (nRole == Qt::DisplayRole) {
            result = pItem->data(index.column());
        } else if (nRole == Qt::UserRole + UD_FILETYPE) {
            result = pItem->scanStruct().id.fileType;
        } else if (nRole == Qt::UserRole + UD_NAME) {
            result = pItem->scanStruct().sName;
        } else if (nRole == Qt::UserRole + UD_INFO) {
            result = pItem->scanStruct().varInfo;
        } else if (nRole == Qt::UserRole + UD_INFO2) {
            result = pItem->scanStruct().varInfo2;
        } else if (nRole == Qt::UserRole + UD_UUID) {
            result = pItem->scanStruct().id.sUuid;
        }
#ifdef QT_GUI_LIB
        else if (nRole == Qt::ForegroundRole) {
            QColor colText;

            if (pItem->scanStruct().globalColor == Qt::transparent) {
                colText = QApplication::palette().text().color();
            } else {
                colText = QColor(pItem->scanStruct().globalColor);
            }

            result = QVariant(colText);
        }
#endif
    }

    return result;
}

Qt::ItemFlags ScanItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = Qt::NoItemFlags;

    if (index.isValid()) {
        result = QAbstractItemModel::flags(index);
    }

    return result;
}

QString ScanItemModel::toXML()
{
    QString sResult;
    QXmlStreamWriter xml(&sResult);

    xml.setAutoFormatting(true);

    _toXML(&xml, g_pRootItem, 0);

    return sResult;
}

QString ScanItemModel::toJSON()
{
    QString sResult;
#if (QT_VERSION_MAJOR > 4)
    QJsonObject jsonResult;

    _toJSON(&jsonResult, g_pRootItem, 0);

    QJsonDocument saveFormat(jsonResult);

    QByteArray baData = saveFormat.toJson(QJsonDocument::Indented);

    sResult = baData.data();
#endif

    return sResult;
}

QString ScanItemModel::toCSV()
{
    QString sResult;

    _toCSV(&sResult, g_pRootItem, 0);

    return sResult;
}

QString ScanItemModel::toTSV()
{
    QString sResult;

    _toTSV(&sResult, g_pRootItem, 0);

    return sResult;
}

QString ScanItemModel::toFormattedString()
{
    QString sResult;

    _toFormattedString(&sResult, g_pRootItem, 0);

    return sResult;
}

void ScanItemModel::coloredOutput()
{
    _coloredOutput(g_pRootItem, 0);
}

QString ScanItemModel::toString(XBinary::FORMATTYPE formatType)
{
    QString sResult;

    if (formatType == XBinary::FORMATTYPE_XML) {
        sResult = toXML();
    } else if (formatType == XBinary::FORMATTYPE_JSON) {
        sResult = toJSON();
    } else if (formatType == XBinary::FORMATTYPE_CSV) {
        sResult = toCSV();
    } else if (formatType == XBinary::FORMATTYPE_TSV) {
        sResult = toTSV();
    } else {
        sResult = toFormattedString();
    }

    return sResult;
}

ScanItem *ScanItemModel::rootItem()
{
    return this->g_pRootItem;
}

void ScanItemModel::_toXML(QXmlStreamWriter *pXml, ScanItem *pItem, qint32 nLevel)
{
    if (pItem->childCount()) {
        pXml->writeStartElement(pItem->data(0).toString());

        qint32 nNumberOfChildren = pItem->childCount();

        for (qint32 i = 0; i < nNumberOfChildren; i++) {
            _toXML(pXml, pItem->child(i), nLevel + 1);
        }

        pXml->writeEndElement();
    } else {
        XBinary::SCANSTRUCT ss = pItem->scanStruct();

        pXml->writeStartElement("detect");
        pXml->writeAttribute("type", ss.sType);
        pXml->writeAttribute("name", ss.sName);
        pXml->writeAttribute("version", ss.sVersion);
        pXml->writeAttribute("info", ss.sInfo);
        pXml->writeCharacters(pItem->data(0).toString());
        pXml->writeEndElement();
    }
}
#if (QT_VERSION_MAJOR > 4)
void ScanItemModel::_toJSON(QJsonObject *pJsonObject, ScanItem *pItem, qint32 nLevel)
{
    if (pItem->childCount()) {
        XBinary::SCANSTRUCT ss = pItem->scanStruct();

        QString sArrayName = "detects";

        if (ss.id.filePart != XBinary::FILEPART_UNKNOWN) {
            pJsonObject->insert("parentfilepart", XBinary::recordFilePartIdToString(ss.parentId.filePart));
            pJsonObject->insert("filetype", XBinary::fileTypeIdToString(ss.id.fileType));

            sArrayName = "values";
        }

        QJsonArray jsArray;

        qint32 nNumberOfChildren = pItem->childCount();

        for (qint32 i = 0; i < nNumberOfChildren; i++) {
            QJsonObject jsRecord;

            _toJSON(&jsRecord, pItem->child(i), nLevel + 1);

            jsArray.append(jsRecord);
        }

        pJsonObject->insert(sArrayName, jsArray);
    } else {
        XBinary::SCANSTRUCT ss = pItem->scanStruct();

        pJsonObject->insert("type", ss.sType);
        pJsonObject->insert("name", ss.sName);
        pJsonObject->insert("version", ss.sVersion);
        pJsonObject->insert("info", ss.sInfo);
        pJsonObject->insert("string", pItem->data(0).toString());
    }
}
#endif
void ScanItemModel::_toCSV(QString *pString, ScanItem *pItem, qint32 nLevel)
{
    if (pItem->childCount()) {
        qint32 nNumberOfChildren = pItem->childCount();

        for (qint32 i = 0; i < nNumberOfChildren; i++) {
            _toCSV(pString, pItem->child(i), nLevel + 1);
        }
    } else {
        XBinary::SCANSTRUCT ss = pItem->scanStruct();

        QString sResult = QString("%1;%2;%3;%4;%5\n").arg(ss.sType, ss.sName, ss.sVersion, ss.sInfo, pItem->data(0).toString());

        pString->append(sResult);
    }
}

void ScanItemModel::_toTSV(QString *pString, ScanItem *pItem, qint32 nLevel)
{
    if (pItem->childCount()) {
        qint32 nNumberOfChildren = pItem->childCount();

        for (qint32 i = 0; i < nNumberOfChildren; i++) {
            _toTSV(pString, pItem->child(i), nLevel + 1);
        }
    } else {
        XBinary::SCANSTRUCT ss = pItem->scanStruct();

        QString sResult = QString("%1\t%2\t%3\t%4\t%5\n").arg(ss.sType, ss.sName, ss.sVersion, ss.sInfo, pItem->data(0).toString());

        pString->append(sResult);
    }
}

void ScanItemModel::_toFormattedString(QString *pString, ScanItem *pItem, qint32 nLevel)
{
    if (nLevel) {
        QString sResult;
        sResult = sResult.leftJustified(4 * (nLevel - 1), ' ');  // TODO function !!!
        sResult.append(QString("%1\n").arg(pItem->data(0).toString()));
        pString->append(sResult);
    }

    qint32 nNumberOfChildren = pItem->childCount();

    for (qint32 i = 0; i < nNumberOfChildren; i++) {
        _toFormattedString(pString, pItem->child(i), nLevel + 1);
    }
}

void ScanItemModel::_coloredOutput(ScanItem *pItem, qint32 nLevel)
{
#ifdef QT_GUI_LIB
    Q_UNUSED(pItem)
    Q_UNUSED(nLevel)
#else
    if (nLevel) {
        QString sPrefix;
        sPrefix = sPrefix.leftJustified(4 * (nLevel - 1), ' ');
        printf("%s", sPrefix.toUtf8().data());
        _coloredItem(pItem);
        printf("\n");
    }

    qint32 nNumberOfChildren = pItem->childCount();

    for (qint32 i = 0; i < nNumberOfChildren; i++) {
        _coloredOutput(pItem->child(i), nLevel + 1);
    }
#endif
}

void ScanItemModel::_coloredItem(ScanItem *pItem)
{
#ifdef QT_GUI_LIB
    Q_UNUSED(pItem)
#else
#ifdef Q_OS_WIN
    HANDLE hConsole = 0;
    WORD wOldAttribute = 0;

    if (pItem->scanStruct().globalColor != Qt::transparent) {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO csbi = {};

        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            wOldAttribute = csbi.wAttributes;
        }

        WORD wAttribute = 0;

        if (pItem->scanStruct().globalColor == Qt::blue) {
            wAttribute = FOREGROUND_BLUE;
        } else if (pItem->scanStruct().globalColor == Qt::red) {
            wAttribute = FOREGROUND_RED;
        } else if (pItem->scanStruct().globalColor == Qt::green) {
            wAttribute = FOREGROUND_GREEN;
        } else if (pItem->scanStruct().globalColor == Qt::yellow) {
            wAttribute = FOREGROUND_RED | FOREGROUND_GREEN;
        } else if (pItem->scanStruct().globalColor == Qt::magenta) {
            wAttribute = FOREGROUND_RED | FOREGROUND_BLUE;
        } else if (pItem->scanStruct().globalColor == Qt::cyan) {
            wAttribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
        } else if (pItem->scanStruct().globalColor == Qt::darkBlue) {
            wAttribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        } else if (pItem->scanStruct().globalColor == Qt::darkRed) {
            wAttribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
        } else if (pItem->scanStruct().globalColor == Qt::darkGreen) {
            wAttribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        } else if (pItem->scanStruct().globalColor == Qt::darkYellow) {
            wAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        } else if (pItem->scanStruct().globalColor == Qt::darkMagenta) {
            wAttribute = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        } else if (pItem->scanStruct().globalColor == Qt::darkCyan) {
            wAttribute = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        }

        if (wAttribute) {
            SetConsoleTextAttribute(hConsole, wAttribute);
        }
    }
#else
    if (pItem->scanStruct().globalColor != Qt::transparent) {
        if (pItem->scanStruct().globalColor == Qt::blue) {
            printf("\033[0;34m");
        } else if (pItem->scanStruct().globalColor == Qt::red) {
            printf("\033[0;31m");
        } else if (pItem->scanStruct().globalColor == Qt::green) {
            printf("\033[0;32m");
        } else if (pItem->scanStruct().globalColor == Qt::yellow) {
            printf("\033[0;33m");
        } else if (pItem->scanStruct().globalColor == Qt::magenta) {
            printf("\033[0;35m");
        } else if (pItem->scanStruct().globalColor == Qt::cyan) {
            printf("\033[0;36m");
        } else if (pItem->scanStruct().globalColor == Qt::darkBlue) {
            printf("\033[1;34m");
        } else if (pItem->scanStruct().globalColor == Qt::darkRed) {
            printf("\033[1;31m");
        } else if (pItem->scanStruct().globalColor == Qt::darkGreen) {
            printf("\033[1;32m");
        } else if (pItem->scanStruct().globalColor == Qt::darkYellow) {
            printf("\033[1;33m");
        } else if (pItem->scanStruct().globalColor == Qt::darkMagenta) {
            printf("\033[1;35m");
        } else if (pItem->scanStruct().globalColor == Qt::darkCyan) {
            printf("\033[1;36m");
        }
    }
#endif

    printf("%s", pItem->data(0).toString().toUtf8().data());

#ifdef Q_OS_WIN
    if (pItem->scanStruct().globalColor != Qt::transparent) {
        if (wOldAttribute) {
            SetConsoleTextAttribute(hConsole, wOldAttribute);
        }
    }
#else
    if (pItem->scanStruct().globalColor != Qt::transparent) {
        printf("\033[0m");
    }
#endif
#endif
}
