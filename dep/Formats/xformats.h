/* Copyright (c) 2020-2023 hors<horsicq@gmail.com>
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
#ifndef XFORMATS_H
#define XFORMATS_H

#include "xbinary.h"
#include "xcom.h"
#include "xelf.h"
#include "xbmp.h"
#include "xgif.h"
#include "xicon.h"
#include "xjpeg.h"
#include "xle.h"
#include "xmach.h"
#include "xmsdos.h"
#include "xne.h"
#include "xpe.h"
#include "xpng.h"
#include "xtiff.h"
#include "xmp3.h"
#include "xmp4.h"
#include "xriff.h"
#ifdef USE_DEX
#include "xandroidbinary.h"
#include "xdex.h"
#endif
#ifdef USE_PDF
#include "xpdf.h"
#endif
#ifdef USE_ARCHIVE
#include "xarchives.h"
#endif
#ifdef QT_GUI_LIB
#include <QComboBox>  // TODO Check TESTLIB !!!
#include <QProgressBar>
#endif

class XFormats : public QObject {
    Q_OBJECT

public:
    explicit XFormats(QObject *pParent = nullptr);

    static XBinary::_MEMORY_MAP getMemoryMap(XBinary::FT fileType, XBinary::MAPMODE mapMode, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1,
                                             XBinary::PDSTRUCT *pPdStruct = nullptr);
    static XBinary::_MEMORY_MAP getMemoryMap(const QString &sFileName, XBinary::MAPMODE mapMode, bool bIsImage = false, XADDR nModuleAddress = -1,
                                             XBinary::PDSTRUCT *pPdStruct = nullptr);
    static QList<XBinary::MAPMODE> getMapModesList(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1,
                                                   XBinary::PDSTRUCT *pPdStruct = nullptr);
    static qint64 getEntryPointAddress(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static qint64 getEntryPointOffset(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static bool isBigEndian(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static QList<XBinary::HREGION> getHRegions(XBinary::FT fileType, QIODevice *pDevice, XBinary::_MEMORY_MAP *pMemoryMap, bool bIsImage = false,
                                               XADDR nModuleAddress = -1, XBinary::PDSTRUCT *pPdStruct = nullptr);
    static QList<XBinary::HREGION> getHighlights(XBinary::FT fileType, QIODevice *pDevice, XBinary::_MEMORY_MAP *pMemoryMap, bool bIsImage = false,
                                                 XADDR nModuleAddress = -1, XBinary::PDSTRUCT *pPdStruct = nullptr);
    static bool isSigned(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static bool isSigned(const QString &sFileName);
    static XBinary::OFFSETSIZE getSignOffsetSize(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static XBinary::OFFSETSIZE getSignOffsetSize(const QString &sFileName);
    static QList<XBinary::SYMBOL_RECORD> getSymbolRecords(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1,
                                                          XBinary::SYMBOL_TYPE symBolType = XBinary::SYMBOL_TYPE_ALL);
    static QSet<XBinary::FT> getFileTypes(QIODevice *pDevice, bool bExtra, XBinary::PDSTRUCT *pPdStruct = nullptr);
    static QSet<XBinary::FT> getFileTypes(QIODevice *pDevice, qint64 nOffset, qint64 nSize, bool bExtra = false);
    static QSet<XBinary::FT> getFileTypes(const QString &sFileName, bool bExtra = false);
    static QSet<XBinary::FT> getFileTypes(QByteArray *pbaData, bool bExtra = false);
    static XBinary::OSINFO getOsInfo(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static XBinary::FILEFORMATINFO getFileFormatInfo(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);

#ifdef USE_ARCHIVE
    static QSet<XBinary::FT> getFileTypes(QIODevice *pDevice, XArchive::RECORD *pRecord, bool bExtra = false);
    static QSet<XBinary::FT> getFileTypesZIP(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct);
#endif
#ifdef QT_GUI_LIB
    static XBinary::FT setFileTypeComboBox(XBinary::FT fileType, QIODevice *pDevice, QComboBox *pComboBox);
    static XBinary::FT setFileTypeComboBox(XBinary::FT fileType, const QString &sFileName, QComboBox *pComboBox);
    static bool setEndiannessComboBox(QComboBox *pComboBox, bool bIsBigEndian);
    static XBinary::MAPMODE setMapModeComboBox(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, QComboBox *pComboBox);
    static void setProgressBar(QProgressBar *pProgressBar, XBinary::PDRECORD pdRecord);
#endif
    static bool saveAllPEIconsToDirectory(QIODevice *pDevice, const QString &sDirectoryName);
    static bool saveAllPECursorsToDirectory(QIODevice *pDevice, const QString &sDirectoryName);
    static bool savePE_ICOToFile(QIODevice *pDevice, QList<XPE::RESOURCE_RECORD> *pListResourceRecords, XPE::RESOURCE_RECORD resourceRecord, const QString &sFileName);

    static Qt::GlobalColor typeToColor(const QString &sType);
    static qint32 typeToPrio(const QString &sType);
    static QString translateType(const QString &sType);
    static void sortRecords(QList<XBinary::SCANSTRUCT> *pListRecords);
    static QString getProtection(QList<XBinary::SCANSTRUCT> *pListRecords);
    static bool isProtection(const QString &sType);

private:
    static QSet<XBinary::FT> _getFileTypes(QIODevice *pDevice, bool bExtra, XBinary::PDSTRUCT *pPdStruct);
};

#endif  // XFORMATS_H
