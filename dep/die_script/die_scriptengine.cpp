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
#include "die_scriptengine.h"

DiE_ScriptEngine::DiE_ScriptEngine(QList<DiE_ScriptEngine::SIGNATURE_RECORD> *pSignaturesList, QList<SCAN_STRUCT> *pListScanStructs, QIODevice *pDevice,
                                   XBinary::FT fileType, Binary_Script::OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
    : XScriptEngine()
{
    g_pSignaturesList = pSignaturesList;
    g_pListScanStructs = pListScanStructs;
    g_pPdStruct = pPdStruct;

#ifdef QT_SCRIPT_LIB
    _addFunction(includeScript, "includeScript");
    _addFunction(_log, "_log");
    _addFunction(_setResult, "_setResult");
    _addFunction(_isResultPresent, "_isResultPresent");
    _addFunction(_getNumberOfResults, "_getNumberOfResults");
    _addFunction(_removeResult, "_removeResult");
    _addFunction(_isStop, "_isStop");
    _addFunction(_encodingList, "_encodingList");
#else
    connect(&g_globalScript, SIGNAL(includeScriptSignal(QString)), this, SLOT(includeScriptSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_logSignal(QString)), this, SLOT(_logSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_setResultSignal(QString, QString, QString, QString)), this, SLOT(_setResultSlot(QString, QString, QString, QString)),
            Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isResultPresentSignal(bool *, QString, QString)), this, SLOT(_isResultPresentSlot(bool *, QString, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_getNumberOfResultsSignal(qint32 *, QString)), this, SLOT(_getNumberOfResultsSlot(qint32 *, QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_removeResultSignal(QString)), this, SLOT(_removeResultSlot(QString)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_isStopSignal(bool *)), this, SLOT(_isStopSlot(bool *)), Qt::DirectConnection);
    connect(&g_globalScript, SIGNAL(_encodingListSignal()), this, SLOT(_encodingListSlot()), Qt::DirectConnection);

    QJSValue valueGlobalScript = newQObject(&g_globalScript);
    globalObject().setProperty("includeScript", valueGlobalScript.property("includeScript"));
    globalObject().setProperty("_log", valueGlobalScript.property("_log"));
    globalObject().setProperty("_setResult", valueGlobalScript.property("_setResult"));
    globalObject().setProperty("_isResultPresent", valueGlobalScript.property("_isResultPresent"));
    globalObject().setProperty("_getNumberOfResults", valueGlobalScript.property("_getNumberOfResults"));
    globalObject().setProperty("_removeResult", valueGlobalScript.property("_removeResult"));
    globalObject().setProperty("_isStop", valueGlobalScript.property("_isStop"));
    globalObject().setProperty("_encodingList", valueGlobalScript.property("_encodingList"));
#endif

    QSet<XBinary::FT> fileTypes = XBinary::getFileTypes(pDevice, true);

    XBinary *pBinary = nullptr;

    if (fileTypes.contains(XBinary::FT_JPEG)) {
        pBinary = new XJpeg(pDevice);
    } else {
        pBinary = new XBinary(pDevice);
    }

    g_listBinaries.append(pBinary);

    Binary_Script *pBinaryScript = new Binary_Script(pBinary, pOptions, pPdStruct);

    if (pBinaryScript) {
        connect(pBinaryScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    }

    g_listBinaryScripts.append(pBinaryScript);

    _addClass(pBinaryScript, "Binary");

    if (XBinary::checkFileType(XBinary::FT_COM, fileType)) {
        XCOM *pCOM = new XCOM(pDevice);
        COM_Script *pExtraScript = new COM_Script(pCOM, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "COM");
    } else if (XBinary::checkFileType(XBinary::FT_PE, fileType)) {
        XPE *pPE = new XPE(pDevice);
        PE_Script *pExtraScript = new PE_Script(pPE, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "PE");
    } else if (XBinary::checkFileType(XBinary::FT_ELF, fileType)) {
        XELF *pELF = new XELF(pDevice);
        ELF_Script *pExtraScript = new ELF_Script(pELF, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "ELF");
    } else if (XBinary::checkFileType(XBinary::FT_MACHO, fileType)) {
        XMACH *pMACH = new XMACH(pDevice);
        MACH_Script *pExtraScript = new MACH_Script(pMACH, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "MACH");
    } else if (XBinary::checkFileType(XBinary::FT_NE, fileType)) {
        XNE *pNE = new XNE(pDevice);
        NE_Script *pExtraScript = new NE_Script(pNE, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "NE");
    } else if (XBinary::checkFileType(XBinary::FT_LE, fileType)) {
        XLE *pLE = new XLE(pDevice);
        LE_Script *pExtraScript = new LE_Script(pLE, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "LE");
    } else if (XBinary::checkFileType(XBinary::FT_LX, fileType)) {
        XLE *pLE = new XLE(pDevice);
        LX_Script *pExtraScript = new LX_Script(pLE, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "LX");
    } else if (XBinary::checkFileType(XBinary::FT_MSDOS, fileType)) {
        XMSDOS *pXMSDOS = new XMSDOS(pDevice);
        MSDOS_Script *pExtraScript = new MSDOS_Script(pXMSDOS, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "MSDOS");
    } else if (XBinary::checkFileType(XBinary::FT_ZIP, fileType)) {
        XZip *pZIP = new XZip(pDevice);
        ZIP_Script *pExtraScript = new ZIP_Script(pZIP, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "ZIP");
    } else if (XBinary::checkFileType(XBinary::FT_JAR, fileType)) {
        XJAR *pJAR = new XJAR(pDevice);
        JAR_Script *pExtraScript = new JAR_Script(pJAR, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "JAR");
    } else if (XBinary::checkFileType(XBinary::FT_APK, fileType)) {
        XAPK *pAPK = new XAPK(pDevice);
        APK_Script *pExtraScript = new APK_Script(pAPK, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "APK");
    } else if (XBinary::checkFileType(XBinary::FT_IPA, fileType)) {
        XIPA *pIPA = new XIPA(pDevice);
        IPA_Script *pExtraScript = new IPA_Script(pIPA, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "IPA");
    } else if (XBinary::checkFileType(XBinary::FT_DEX, fileType)) {
        XDEX *pDEX = new XDEX(pDevice);
        DEX_Script *pExtraScript = new DEX_Script(pDEX, pOptions, pPdStruct);

        if (pExtraScript) {
            connect(pExtraScript, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
        }

        _addClass(pExtraScript, "DEX");
    }

    // TODO APKS
}

DiE_ScriptEngine::~DiE_ScriptEngine()
{
    {
        qint32 nNumberOfRecords = g_listBinaries.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            delete g_listBinaries.at(i);
        }
    }

    {
        qint32 nNumberOfRecords = g_listBinaryScripts.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            delete g_listBinaryScripts.at(i);
        }
    }
}

bool DiE_ScriptEngine::handleError(XSCRIPTVALUE value, QString *psErrorString)
{
    bool bResult = true;

    if (value.isError()) {
        // TODO Check more information
        *psErrorString = QString("%1: %2").arg(value.property("lineNumber").toString(), value.toString());

        bResult = false;
    }

    return bResult;
}

QList<DiE_ScriptEngine::RESULT> DiE_ScriptEngine::getListLocalResult()
{
    return g_listResult;
}

void DiE_ScriptEngine::clearListLocalResult()
{
    g_listResult.clear();
}

#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::includeScript(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        pEngine->currentContext()->setActivationObject(pEngine->currentContext()->parentContext()->activationObject());

        QString sScript = pContext->argument(0).toString();

        pScriptEngine->includeScriptSlot(sScript);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_log(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sText = pContext->argument(0).toString();

        pScriptEngine->_logSlot(sText);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_setResult(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        QString sName = pContext->argument(1).toString();
        QString sVersion = pContext->argument(2).toString();
        QString sOptions = pContext->argument(3).toString();

        pScriptEngine->_setResultSlot(sType, sName, sVersion, sOptions);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isResultPresent(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        QString sName = pContext->argument(1).toString();
        bool bResult = false;

        pScriptEngine->_isResultPresentSlot(&bResult, sType, sName);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_getNumberOfResults(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        qint32 nResult = 0;

        pScriptEngine->_getNumberOfResultsSlot(&nResult, sType);

        result = nResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_removeResult(QScriptContext *pContext, QScriptEngine *pEngine)
{
    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        QString sType = pContext->argument(0).toString();
        QString sName = pContext->argument(1).toString();

        pScriptEngine->_removeResultSlot(sType, sName);
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_isStop(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_isStopSlot(&bResult);

        result = bResult;
    }

    return result;
}
#endif
#ifdef QT_SCRIPT_LIB
QScriptValue DiE_ScriptEngine::_encodingList(QScriptContext *pContext, QScriptEngine *pEngine)
{
    Q_UNUSED(pContext)

    QScriptValue result;

    DiE_ScriptEngine *pScriptEngine = static_cast<DiE_ScriptEngine *>(pEngine);

    if (pScriptEngine) {
        bool bResult = false;

        pScriptEngine->_encodingListSlot();

        result = bResult;
    }

    return result;
}
#endif

void DiE_ScriptEngine::includeScriptSlot(const QString &sScript)
{
    // TODO error, cannot find signature
    qint32 nNumberOfSignatures = g_pSignaturesList->count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (g_pSignaturesList->at(i).fileType == XBinary::FT_UNKNOWN) {
            if (g_pSignaturesList->at(i).sName == sScript) {
                // TODO error
                evaluate(g_pSignaturesList->at(i).sText, sScript);

                break;
            }
        }
    }
}

void DiE_ScriptEngine::_logSlot(const QString &sText)
{
    emit infoMessage(sText);
}

void DiE_ScriptEngine::_setResultSlot(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions)
{
    RESULT record = {};
    record.sType = sType;
    record.sName = sName;
    record.sVersion = sVersion;
    record.sOptions = sOptions;

    g_listResult.append(record);
}

void DiE_ScriptEngine::_isResultPresentSlot(bool *pbResult, const QString &sType, const QString &sName)
{
    *pbResult = false;

    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) && ((g_pListScanStructs->at(i).sName.toUpper() == sName.toUpper()) || (sName == ""))) {
            *pbResult = true;
            break;
        }
    }
}

void DiE_ScriptEngine::_getNumberOfResultsSlot(qint32 *pnResult, const QString &sType)
{
    *pnResult = 0;

    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) || (sType == "")) {
            (*pnResult)++;
        }
    }
}

void DiE_ScriptEngine::_removeResultSlot(const QString &sType, const QString &sName)
{
    qint32 nNumberOfResults = g_pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfResults; i++) {
        if ((g_pListScanStructs->at(i).sType.toUpper() == sType.toUpper()) && (g_pListScanStructs->at(i).sName.toUpper() == sName.toUpper())) {
            g_pListScanStructs->removeAt(i);
            break;
        }
    }
}

void DiE_ScriptEngine::_isStopSlot(bool *pResult)
{
    *pResult = (g_pPdStruct->bIsStop);
}

void DiE_ScriptEngine::_encodingListSlot()
{
#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
    QList<QString> listCodePages = XOptions::getCodePages(false);

    qint32 nNumberOfCodePages = listCodePages.count();

    for (qint32 i = 0; i < nNumberOfCodePages; i++) {
        emit infoMessage(listCodePages.at(i));
    }
#endif
}

DiE_ScriptEngine::RESULT DiE_ScriptEngine::stringToResult(const QString &sString, bool bShowType, bool bShowVersion, bool bShowOptions)
{
    QString sStringTmp = sString;
    RESULT result = {};

    if (bShowType) {
        result.sType = sStringTmp.section(": ", 0, 0);
        sStringTmp = sStringTmp.section(": ", 1, -1);
    }

    QString _sString = sStringTmp;

    if (bShowOptions) {
        if (_sString.count("[") == 1) {
            result.sName = _sString.section("[", 0, 0);
            result.sOptions = _sString.section("[", 1, -1).section("]", 0, 0);
            _sString = _sString.section("[", 0, 0);
        }
    }

    if (bShowVersion) {
        if (_sString.count("(") == 1) {
            result.sVersion = _sString.section("(", 1, -1).section(")", 0, 0);
            result.sName = _sString.section("(", 0, 0);
        }
    }

    return result;
}
