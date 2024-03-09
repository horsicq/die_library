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
#ifndef DIE_SCRIPT_H
#define DIE_SCRIPT_H

#include <QDir>
#if (QT_VERSION_MAJOR > 4)  // TODO Check
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#endif
#include <QSemaphore>
#include <QTimer>
#include <QXmlStreamWriter>

#include "die_scriptengine.h"
#include "scanitemmodel.h"
#include "xformats.h"
#include "xarchives.h"
#include "xzip.h"
#ifdef QT_SCRIPTTOOLS_LIB
#include <QAction>
#include <QScriptEngineDebugger>
#endif

bool sort_signature_prio(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2);
bool sort_signature_name(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2);

class DiE_Script : public QObject {
    Q_OBJECT

public:
    struct ERROR_RECORD {
        QString sScript;
        QString sErrorString;
    };

    struct DEBUG_RECORD {
        QString sScript;
        qint64 nElapsedTime;
    };

    struct SCAN_RESULT {
        qint64 nScanTime;
        QString sFileName;
        qint64 nSize;
        //        XBinary::SCANID id;
        //        SCAN_HEADER scanHeader; // TODO set
        QList<DiE_ScriptEngine::SCAN_STRUCT> listRecords;
        QList<ERROR_RECORD> listErrors;
        QList<DEBUG_RECORD> listDebugRecords;
        //        bool bIsValidType;
    };

    struct OPTIONS {
        bool bIsDeepScan;
        bool bIsHeuristicScan;
        bool bIsVerbose;
        bool bAllTypesScan;
        bool bIsRecursiveScan;
        bool bDebug;
        bool bShowType;
        bool bShowVersion;
        bool bShowOptions;
        bool bShowEntropy;
        bool bShowExtraInfo;
        QString sSpecial;        // Special info
        XBinary::FT fileType;    // Optional
        QString sSignatureName;  // Optional
        bool bResultAsXML;
        bool bResultAsJSON;
        bool bResultAsCSV;
        bool bResultAsTSV;
        bool bSubdirectories;  // For directory scan
    };

    struct STATS {
        QMap<QString, qint32> mapTypes;
    };

    struct SIGNATURE_STATE {
        XBinary::FT fileType;
        qint32 nNumberOfSignatures;
    };

    enum DBT {
        DBT_UNKNOWN = 0,
        DBT_FOLDER,
        DBT_COMPRESSED
    };

    explicit DiE_Script(QObject *pParent = nullptr);
    bool loadDatabase(const QString &sDatabasePath, bool bInit);

    QList<SIGNATURE_STATE> getSignatureStates();
    qint32 getNumberOfSignatures(XBinary::FT fileType);
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> *getSignatures();
    SCAN_RESULT scanFile(const QString &sFileName, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct = nullptr);
    SCAN_RESULT scanDevice(QIODevice *pDevice, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct = nullptr);
    SCAN_RESULT processFile(const QString &sFileName, OPTIONS *pOptions, const QString &sFunction, XBinary::PDSTRUCT *pPdStruct = nullptr);
    SCAN_RESULT processDevice(QIODevice *pDevice, OPTIONS *pOptions, const QString &sFunction, XBinary::PDSTRUCT *pPdStruct = nullptr);
    void process(QIODevice *pDevice, const QString &sFunction, SCAN_RESULT *pScanResult, qint64 nOffset, qint64 nSize, XBinary::SCANID parentId, OPTIONS *pOptions,
                 bool bInit, XBinary::PDSTRUCT *pPdStruct);
    DiE_ScriptEngine::SIGNATURE_RECORD getSignatureByFilePath(const QString &sSignatureFilePath);
    bool updateSignature(const QString &sSignatureFilePath, const QString &sText);
    STATS getStats();
    DBT getDatabaseType();
    bool isSignaturesPresent(XBinary::FT fileType);

    static QString getErrorsString(SCAN_RESULT *pScanResult);
    static QList<QString> getErrorsStringList(SCAN_RESULT *pScanResult);
#ifdef QT_SCRIPTTOOLS_LIB
    void setDebugger(QScriptEngineDebugger *pDebugger);
    void removeDebugger();
#endif
    void setData(const QString &sDirectory, const OPTIONS &scanOptions, XBinary::PDSTRUCT *pPdStruct);
    void setData(QIODevice *pDevice, const OPTIONS &scanOptions, XBinary::PDSTRUCT *pPdStruct);

    static QList<XBinary::SCANSTRUCT> convert(QList<DiE_ScriptEngine::SCAN_STRUCT> *pListScanStructs);
    SCAN_RESULT getScanResultProcess();
    static bool isScanable(const QSet<XBinary::FT> &stFT);

public slots:
    void process();

private:
    static QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabasePath(const QString &sDatabasePath, XBinary::FT fileType);
    static QList<DiE_ScriptEngine::SIGNATURE_RECORD> _loadDatabaseFromZip(XZip *pZip, QList<XArchive::RECORD> *pListRecords, const QString &sPrefix,
                                                                          XBinary::FT fileType);
    XBinary::SCANID _processDetect(SCAN_RESULT *pScanResult, QIODevice *pDevice, const QString &sDetectFunction, XBinary::SCANID parentId, XBinary::FT fileType,
                                   OPTIONS *pOptions, const QString &sSignatureFilePath, qint64 nOffset, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct);
    bool _handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord, SCAN_RESULT *pScanResult);

signals:
    void scanCompleted(qint64 nTime);
    void directoryScanFileStarted(const QString &sFileName);
    void directoryScanResult(const DiE_Script::SCAN_RESULT &scanResult);
    void errorMessage(const QString &sErrorMessage);
    void infoMessage(const QString &sInfoMessage);

private:
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> g_listSignatures;
    DBT g_databaseType;
#ifdef QT_SCRIPTTOOLS_LIB
    QScriptEngineDebugger *g_pDebugger;
#endif
    QString g_sDirectoryProcess;
    QIODevice *g_pDeviceProcess;
    OPTIONS g_scanOptionsProcess;
    SCAN_RESULT g_scanResultProcess;
    XBinary::PDSTRUCT *g_pPdStruct;
    //    QMutex g_mutex;
    //    QSemaphore g_semaphore;
};

#endif  // DIE_SCRIPT_H
