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
#ifndef DIE_SCRIPTENGINE_H
#define DIE_SCRIPTENGINE_H

#include <QIODevice>

#include "binary_script.h"
#include "com_script.h"
#include "elf_script.h"
#include "le_script.h"
#include "lx_script.h"
#include "mach_script.h"
#include "msdos_script.h"
#include "ne_script.h"
#include "pe_script.h"
#include "xbinary.h"
#include "zip_script.h"
#include "jar_script.h"
#include "apk_script.h"
#include "ipa_script.h"
#include "dex_script.h"
#include "xscriptengine.h"

class DiE_ScriptEngine : public XScriptEngine {
    Q_OBJECT

public:
    struct SIGNATURE_RECORD {
        XBinary::FT fileType;
        QString sName;
        QString sFilePath;
        QString sText;
        bool bReadOnly;
    };

    struct RESULT {
        QString sType;
        QString sName;
        QString sVersion;
        QString sOptions;
    };

    //    struct SCAN_HEADER
    //    {
    //        XBinary::FT fileType;
    //        QString sArch;
    //        XBinary::MODE mode;
    //        bool bIsBigEndian;
    //        QString sType;
    //        qint64 nSize;
    //        qint64 nOffset;
    //        XBinary::FILEPART filePart;
    //    };

    struct SCAN_STRUCT {
        bool bIsHeuristic;
        XBinary::SCANID id;
        XBinary::SCANID parentId;
        //        SCAN_HEADER scanHeader;
        //        XBinary::FT fileType;
        QString sFullString;
        QString sType;
        QString sResult;
        QString sName;
        QString sVersion;
        QString sOptions;
        QString sSignature;
        QString sSignatureFileName;
    };

    DiE_ScriptEngine(QList<SIGNATURE_RECORD> *pSignaturesList, QList<SCAN_STRUCT> *pListScanStructs, QIODevice *pDevice, XBinary::FT fileType,
                     Binary_Script::OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct);
    ~DiE_ScriptEngine();

    bool handleError(XSCRIPTVALUE value, QString *psErrorString);
    QList<RESULT> getListLocalResult();
    void clearListLocalResult();
    static RESULT stringToResult(const QString &sString, bool bShowType, bool bShowVersion, bool bShowOptions);

private:
#ifdef QT_SCRIPT_LIB
    static QScriptValue includeScript(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _log(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _setResult(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isResultPresent(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _getNumberOfResults(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _removeResult(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _isStop(QScriptContext *pContext, QScriptEngine *pEngine);
    static QScriptValue _encodingList(QScriptContext *pContext, QScriptEngine *pEngine);
#endif

private slots:
    void includeScriptSlot(const QString &sScript);
    void _logSlot(const QString &sText);
    void _setResultSlot(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions);
    void _isResultPresentSlot(bool *pbResult, const QString &sType, const QString &sName);
    void _getNumberOfResultsSlot(qint32 *pnResult, const QString &sType);
    void _removeResultSlot(const QString &sType, const QString &sName);
    void _isStopSlot(bool *pResult);
    void _encodingListSlot();

private:
    QList<SIGNATURE_RECORD> *g_pSignaturesList;
    QList<SCAN_STRUCT> *g_pListScanStructs;
    QList<XBinary *> g_listBinaries;
    QList<Binary_Script *> g_listBinaryScripts;
    XBinary::PDSTRUCT *g_pPdStruct;

    QList<RESULT> g_listResult;
#ifndef QT_SCRIPT_LIB
    global_script g_globalScript;
#endif
};

#endif  // DIE_SCRIPTENGINE_H
