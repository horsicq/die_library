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
#include "die_script.h"

bool sort_signature_prio(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if ((sr1.sName == "_init") && (sr2.sName == "_init")) {
        return false;
    }

    if (sr1.sName == "_init") {
        return true;
    } else if (sr2.sName == "_init") {
        return false;
    }

    qint32 nPos1 = sr1.sName.count(".");
    qint32 nPos2 = sr2.sName.count(".");

    if ((nPos1 > 1) && (nPos2 > 1)) {
        QString sPrio1 = sr1.sName.section(".", nPos1 - 1, nPos1 - 1);
        QString sPrio2 = sr2.sName.section(".", nPos2 - 1, nPos2 - 1);

        if ((sPrio1 != "") && (sPrio2 != "")) {
            if (sPrio1 > sPrio2) {
                return false;
            } else if (sPrio1 < sPrio2) {
                return true;
            } else if (sPrio1 == sPrio2) {
                return (sr1.sName.section(".", nPos1 - 2, nPos1 - 2) < sr2.sName.section(".", nPos2 - 2, nPos2 - 2));
            }
        }
    }

    return (sr1.sName < sr2.sName);
}

bool sort_signature_name(const DiE_ScriptEngine::SIGNATURE_RECORD &sr1, const DiE_ScriptEngine::SIGNATURE_RECORD &sr2)
{
    if ((sr1.sName == "_init") && (sr2.sName == "_init")) {
        return false;
    }

    if (sr1.sName == "_init") {
        return true;
    } else if (sr2.sName == "_init") {
        return false;
    }

    return (sr1.sName < sr2.sName);
}

DiE_Script::DiE_Script(QObject *pParent) : QObject(pParent)
{
    g_databaseType = DBT_UNKNOWN;
    g_pPdStruct = nullptr;
    g_pDeviceProcess = nullptr;
    g_scanResultProcess = SCAN_RESULT();
#ifdef QT_SCRIPTTOOLS_LIB
    g_pDebugger = nullptr;
#endif
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabasePath(const QString &sDatabasePath, XBinary::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    QDir dir(sDatabasePath);

    QFileInfoList eil = dir.entryInfoList();

    qint32 nNumberOfFiles = eil.count();

    for (qint32 i = 0; i < nNumberOfFiles; i++) {
        if (eil.at(i).isFile()) {
            QString sSuffix = eil.at(i).suffix().toLower();

            if ((sSuffix == "sg") || (sSuffix == "")) {
                DiE_ScriptEngine::SIGNATURE_RECORD record = {};

                record.fileType = fileType;
                record.sName = eil.at(i).fileName();
                record.sText = XBinary::readFile(eil.at(i).absoluteFilePath());
                record.sFilePath = eil.at(i).absoluteFilePath();

                listResult.append(record);
            }
        }
    }

    std::sort(listResult.begin(), listResult.end(), sort_signature_prio);

    return listResult;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> DiE_Script::_loadDatabaseFromZip(XZip *pZip, QList<XArchive::RECORD> *pListRecords, const QString &sPrefix,
                                                                           XBinary::FT fileType)
{
    QList<DiE_ScriptEngine::SIGNATURE_RECORD> listResult;

    qint32 nNumberOfRecords = pListRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XArchive::RECORD zipRecord = pListRecords->at(i);

        if (((sPrefix == "") && (!zipRecord.sFileName.contains("/"))) ||
            ((zipRecord.sFileName.contains("/")) && (zipRecord.sFileName.section("/", 0, 0) == sPrefix) && (zipRecord.sFileName.section("/", 1, 1) != ""))) {
            QFileInfo fi(zipRecord.sFileName);

            DiE_ScriptEngine::SIGNATURE_RECORD record = {};

            record.fileType = fileType;
            record.sName = fi.fileName();
            record.sText = pZip->decompress(&zipRecord, false, nullptr);
            record.sFilePath = zipRecord.sFileName;
            record.bReadOnly = true;

            listResult.append(record);
        }
    }

    return listResult;
}

XBinary::SCANID DiE_Script::_processDetect(SCAN_RESULT *pScanResult, QIODevice *pDevice, const QString &sDetectFunction, XBinary::SCANID parentId, XBinary::FT fileType,
                                           OPTIONS *pOptions, const QString &sSignatureFilePath, qint64 nOffset, bool bAddUnknown, XBinary::PDSTRUCT *pPdStruct)
{
    XBinary::SCANID resultId = {};

    XBinary::_MEMORY_MAP memoryMap = XFormats::getMemoryMap(fileType, XBinary::MAPMODE_UNKNOWN, pDevice);

    resultId.fileType = fileType;
    resultId.sUuid = XBinary::generateUUID();
    resultId.sArch = memoryMap.sArch;
    resultId.mode = memoryMap.mode;
    resultId.endian = memoryMap.endian;
    resultId.sType = memoryMap.sType;
    resultId.nOffset = nOffset;
    resultId.nSize = pDevice->size();
    resultId.filePart = XBinary::FILEPART_HEADER;

    qint32 nNumberOfSignatures = g_listSignatures.count();

    DiE_ScriptEngine::SIGNATURE_RECORD srGlobalInit = {};
    DiE_ScriptEngine::SIGNATURE_RECORD srInit = {};

    bool bGlobalInit = false;
    bool bInit = false;

    for (qint32 i = 0; (i < nNumberOfSignatures) && (!(pPdStruct->bIsStop)); i++) {
        if (g_listSignatures.at(i).sName == "_init") {
            if (g_listSignatures.at(i).fileType == XBinary::FT_UNKNOWN) {
                srGlobalInit = g_listSignatures.at(i);
                bGlobalInit = true;
            }

            if (XBinary::checkFileType(g_listSignatures.at(i).fileType, fileType)) {
                srInit = g_listSignatures.at(i);
                bInit = true;
            }
        }

        if (bGlobalInit && bInit) {
            break;
        }
    }

    Binary_Script::OPTIONS _options = {};
    _options.bIsDeepScan = pOptions->bIsDeepScan;
    _options.bIsHeuristicScan = pOptions->bIsHeuristicScan;
    _options.bIsVerbose = pOptions->bIsVerbose;

    DiE_ScriptEngine scriptEngine(&g_listSignatures, &(pScanResult->listRecords), pDevice, fileType, &_options, pPdStruct);

    connect(&scriptEngine, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));
    connect(&scriptEngine, SIGNAL(infoMessage(QString)), this, SIGNAL(infoMessage(QString)));

#ifdef QT_SCRIPTTOOLS_LIB
    if (g_pDebugger) {
        g_pDebugger->attachTo(&scriptEngine);
    }
#endif

    if (nNumberOfSignatures) {
        if (bGlobalInit) {
            _handleError(&scriptEngine, scriptEngine.evaluate(srGlobalInit.sText, srGlobalInit.sFilePath), &srGlobalInit, pScanResult);
        }

        if (bInit) {
            _handleError(&scriptEngine, scriptEngine.evaluate(srInit.sText, srInit.sFilePath), &srInit, pScanResult);
        }
    }

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nNumberOfSignatures);

    for (qint32 i = 0; (i < nNumberOfSignatures) && (!(pPdStruct->bIsStop)); i++) {
        XBinary::setPdStructStatus(pPdStruct, _nFreeIndex, g_listSignatures.at(i).sName);

        bool bExec = false;

        if (XBinary::checkFileType(g_listSignatures.at(i).fileType, fileType)) {
            bExec = true;
        }

        if (pOptions->sSignatureName != "") {
            if (pOptions->sSignatureName != g_listSignatures.at(i).sName) {
                bExec = false;
            }
        }

        if (!pOptions->bIsDeepScan) {
            QString sPrefix = g_listSignatures.at(i).sName.section(".", 0, 0).toUpper();

            if ((sPrefix == "DS") || (sPrefix == "EP")) {
                bExec = false;
            }
        }

        if (!pOptions->bIsHeuristicScan) {
            QString sPrefix = g_listSignatures.at(i).sName.section(".", 0, 0).toUpper();

            if (sPrefix == "HEUR") {
                bExec = false;
            }
        }

        if (sSignatureFilePath != "")  // TODO Check!
        {
            bExec = (sSignatureFilePath == g_listSignatures.at(i).sFilePath);
        }

        if (bExec) {
            scriptEngine.clearListLocalResult();

            DiE_ScriptEngine::SIGNATURE_RECORD signatureRecord = g_listSignatures.at(i);

            QElapsedTimer scanTimer;

            if (pOptions->bDebug) {
                scanTimer.start();
            }

            XSCRIPTVALUE script = scriptEngine.evaluate(signatureRecord.sText, signatureRecord.sFilePath);

            if (_handleError(&scriptEngine, script, &signatureRecord, pScanResult)) {
#ifdef QT_SCRIPTTOOLS_LIB
                if (g_pDebugger) {
                    g_pDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
                }
#endif
                XSCRIPTVALUE _scriptValue = scriptEngine.globalObject().property(sDetectFunction);

                if (_handleError(&scriptEngine, _scriptValue, &signatureRecord, pScanResult)) {
                    XSCRIPTVALUELIST valuelist;

                    if (sDetectFunction == "detect") {
                        valuelist << pOptions->bShowType << pOptions->bShowVersion << pOptions->bShowOptions;
                    }

#ifdef QT_SCRIPT_LIB
                    QScriptValue result = _scriptValue.call(script, valuelist);
#else
                    QJSValue result = _scriptValue.callWithInstance(script, valuelist);
#endif

                    if (_handleError(&scriptEngine, result, &signatureRecord, pScanResult)) {
                        // TODO getResult
                        QString sResult = result.toString();

                        QList<DiE_ScriptEngine::RESULT> listLocalResult = scriptEngine.getListLocalResult();
                        qint32 nNumberOfDetects = listLocalResult.count();

                        if ((nNumberOfDetects == 0) && (sResult != "")) {
                            listLocalResult.append(DiE_ScriptEngine::stringToResult(sResult, pOptions->bShowType, pOptions->bShowVersion, pOptions->bShowOptions));
                        }

                        for (qint32 j = 0; j < nNumberOfDetects; j++) {
                            DiE_ScriptEngine::SCAN_STRUCT ssRecord = {};

                            //                            if(baseId.fileType==XBinary::FT_BINARY)
                            //                            {
                            //                                QString sPrefix=signatureRecord.sName.section(".",0,0).toUpper();

                            //                                if(sPrefix=="COM")
                            //                                {
                            //                                    baseId.fileType=XBinary::FT_COM;
                            //                                    baseId.sArch="8086";
                            //                                    baseId.sType="EXE";
                            //                                }
                            //                                else if(sPrefix=="TEXT") // mb TODO not set if COM
                            //                                {
                            //                                    baseId.fileType=XBinary::FT_TEXT;
                            //                                }
                            //                            }

                            // TODO IDs
                            ssRecord.id = resultId;
                            ssRecord.parentId = parentId;

                            ssRecord.sSignature = signatureRecord.sName;
                            ssRecord.sSignatureFileName = signatureRecord.sFilePath;
                            ssRecord.sType = listLocalResult.at(j).sType;
                            ssRecord.sName = listLocalResult.at(j).sName;
                            ssRecord.sVersion = listLocalResult.at(j).sVersion;
                            ssRecord.sOptions = listLocalResult.at(j).sOptions;
                            ssRecord.sFullString = QString("%1: %2(%3)[%4]").arg(ssRecord.sType, ssRecord.sName, ssRecord.sVersion, ssRecord.sOptions);
                            ssRecord.sResult = QString("%1(%2)[%3]").arg(ssRecord.sName, ssRecord.sVersion, ssRecord.sOptions);

                            pScanResult->listRecords.append(ssRecord);
                        }
                    }
                }
            }

            if (pOptions->bDebug) {
                DEBUG_RECORD debugRecord = {};
                debugRecord.sScript = signatureRecord.sName;
                debugRecord.nElapsedTime = scanTimer.elapsed();
#ifdef QT_DEBUG
                qDebug("%s: %lld msec", debugRecord.sScript.toLatin1().data(), debugRecord.nElapsedTime);
#endif
                pScanResult->listDebugRecords.append(debugRecord);
            }
        }

        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
    }

    if (bAddUnknown) {
        if (pScanResult->listRecords.count() == 0) {
            DiE_ScriptEngine::SCAN_STRUCT ssRecord = {};

            ssRecord.id = resultId;
            ssRecord.parentId = parentId;

            pScanResult->listRecords.append(ssRecord);
        }
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return resultId;
}

bool DiE_Script::_handleError(DiE_ScriptEngine *pScriptEngine, XSCRIPTVALUE scriptValue, DiE_ScriptEngine::SIGNATURE_RECORD *pSignatureRecord,
                              DiE_Script::SCAN_RESULT *pScanResult)
{
    bool bResult = false;

    QString sErrorString;
    if (pScriptEngine->handleError(scriptValue, &sErrorString)) {
        bResult = true;
    } else {
        ERROR_RECORD errorRecord = {};
        errorRecord.sScript = pSignatureRecord->sName;
        errorRecord.sErrorString = sErrorString;

        pScanResult->listErrors.append(errorRecord);
    }

    return bResult;
}

bool DiE_Script::loadDatabase(const QString &sDatabasePath, bool bInit)
{
    g_databaseType = DBT_UNKNOWN;

    if (bInit) {
        g_listSignatures.clear();
    }

    QString _sDatabasePath = XBinary::convertPathName(sDatabasePath);

    if (XBinary::isFileExists(_sDatabasePath))  // Load from zip
    {
        QFile file;
        file.setFileName(_sDatabasePath);

        if (file.open(QIODevice::ReadOnly)) {
            XZip zip(&file);

            if (zip.isValid()) {
                QList<XArchive::RECORD> listRecords = zip.getRecords(-1, g_pPdStruct);  // TODO Check

                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "", XBinary::FT_UNKNOWN));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "Binary", XBinary::FT_BINARY));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "COM", XBinary::FT_COM));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "ZIP", XBinary::FT_ZIP));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "JAR", XBinary::FT_JAR));  // TODO -> Archive
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "APK", XBinary::FT_APK));  // TODO -> Archive
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "IPA", XBinary::FT_IPA));  // TODO -> Archive
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "DEX", XBinary::FT_DEX));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "MSDOS", XBinary::FT_MSDOS));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "LE", XBinary::FT_LE));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "LX", XBinary::FT_LX));  // TODO Check
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "NE", XBinary::FT_NE));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "PE", XBinary::FT_PE));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "ELF", XBinary::FT_ELF));
                g_listSignatures.append(_loadDatabaseFromZip(&zip, &listRecords, "MACH", XBinary::FT_MACHO));

                g_databaseType = DBT_COMPRESSED;
            }

            file.close();
        }
    } else if (XBinary::isDirectoryExists(_sDatabasePath)) {
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath, XBinary::FT_UNKNOWN));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "Binary", XBinary::FT_BINARY));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "COM", XBinary::FT_COM));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "ZIP", XBinary::FT_ZIP));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "JAR", XBinary::FT_JAR));  // TODO -> Archive
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "APK", XBinary::FT_APK));  // TODO -> Archive
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "IPA", XBinary::FT_IPA));  // TODO -> Archive
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "DEX", XBinary::FT_DEX));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "MSDOS", XBinary::FT_MSDOS));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "LE", XBinary::FT_LE));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "LX", XBinary::FT_LX));  // TODO Check
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "NE", XBinary::FT_NE));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "PE", XBinary::FT_PE));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "ELF", XBinary::FT_ELF));
        g_listSignatures.append(_loadDatabasePath(_sDatabasePath + QDir::separator() + "MACH", XBinary::FT_MACHO));

        g_databaseType = DBT_FOLDER;
    } else {
        emit errorMessage(QString("%1: %2").arg(tr("Cannot load database"), sDatabasePath));
    }

    return g_listSignatures.count();
}

QList<DiE_Script::SIGNATURE_STATE> DiE_Script::getSignatureStates()
{
    QList<SIGNATURE_STATE> listResult;

    QList<XBinary::FT> listFT;

    listFT.append(XBinary::FT_BINARY);
    listFT.append(XBinary::FT_COM);
    listFT.append(XBinary::FT_MSDOS);
    listFT.append(XBinary::FT_NE);
    listFT.append(XBinary::FT_LE);
    listFT.append(XBinary::FT_LX);
    listFT.append(XBinary::FT_PE);
    listFT.append(XBinary::FT_ELF);
    listFT.append(XBinary::FT_MACHO);
    listFT.append(XBinary::FT_ZIP);
    listFT.append(XBinary::FT_JAR);
    listFT.append(XBinary::FT_APK);
    listFT.append(XBinary::FT_IPA);
    listFT.append(XBinary::FT_DEX);

    qint32 nNumberOfFileTypes = listFT.count();

    for (qint32 i = 0; i < nNumberOfFileTypes; i++) {
        SIGNATURE_STATE state = {};
        state.fileType = listFT.at(i);
        state.nNumberOfSignatures = getNumberOfSignatures(state.fileType);

        listResult.append(state);
    }

    return listResult;
}

qint32 DiE_Script::getNumberOfSignatures(XBinary::FT fileType)
{
    qint32 nResult = 0;

    qint32 nNumberOfSignatures = g_listSignatures.count();

    for (qint32 i = 0; (i < nNumberOfSignatures); i++) {
        if ((g_listSignatures.at(i).sName != "_init") && (XBinary::checkFileType(g_listSignatures.at(i).fileType, fileType))) {
            nResult++;
        }
    }

    return nResult;
}

QList<DiE_ScriptEngine::SIGNATURE_RECORD> *DiE_Script::getSignatures()
{
    return &g_listSignatures;
}

DiE_Script::SCAN_RESULT DiE_Script::scanFile(const QString &sFileName, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
{
    return processFile(sFileName, pOptions, "detect", pPdStruct);
}

DiE_Script::SCAN_RESULT DiE_Script::scanDevice(QIODevice *pDevice, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct)
{
    return processDevice(pDevice, pOptions, "detect", pPdStruct);
}

DiE_Script::SCAN_RESULT DiE_Script::processFile(const QString &sFileName, OPTIONS *pOptions, const QString &sFunction, XBinary::PDSTRUCT *pPdStruct)
{
    SCAN_RESULT scanResult = {};

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        scanResult = processDevice(&file, pOptions, sFunction, pPdStruct);
        file.close();
    }

    return scanResult;
}

DiE_Script::SCAN_RESULT DiE_Script::processDevice(QIODevice *pDevice, OPTIONS *pOptions, const QString &sFunction, XBinary::PDSTRUCT *pPdStruct)
{
    SCAN_RESULT scanResult = {};

    XBinary::SCANID parentId = {};
    parentId.fileType = XBinary::FT_UNKNOWN;
    parentId.filePart = XBinary::FILEPART_HEADER;

    process(pDevice, sFunction, &scanResult, 0, pDevice->size(), parentId, pOptions, true, pPdStruct);

    return scanResult;
}

void DiE_Script::process(QIODevice *pDevice, const QString &sFunction, SCAN_RESULT *pScanResult, qint64 nOffset, qint64 nSize, XBinary::SCANID parentId,
                         OPTIONS *pOptions, bool bInit, XBinary::PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QElapsedTimer *pScanTimer = nullptr;

    if (bInit) {
        pScanTimer = new QElapsedTimer;
        pScanTimer->start();
        pScanResult->sFileName = XBinary::getDeviceFileName(pDevice);
    }

    QIODevice *_pDevice = nullptr;
    SubDevice *pSd = nullptr;

    if ((nOffset == 0) && (pDevice->size() == nSize)) {
        _pDevice = pDevice;
    } else {
        pSd = new SubDevice(pDevice, nOffset, nSize);
        pSd->open(QIODevice::ReadOnly);
        _pDevice = pSd;
    }

    QSet<XBinary::FT> stFT = XFormats::getFileTypes(_pDevice, true, pPdStruct);
    QSet<XBinary::FT> stFTOriginal = stFT;

    if (bInit) {
        if (pOptions->fileType != XBinary::FT_UNKNOWN) {
            XBinary::filterFileTypes(&stFT, pOptions->fileType);
        }
    }

    if (pOptions->bAllTypesScan) {
        if (stFT.contains(XBinary::FT_PE32) || stFT.contains(XBinary::FT_PE64) || stFT.contains(XBinary::FT_LE) || stFT.contains(XBinary::FT_LX) ||
            stFT.contains(XBinary::FT_NE)) {
            _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_MSDOS, pOptions, "", 0, true, pPdStruct);
        }

        if (stFT.contains(XBinary::FT_APK) || stFT.contains(XBinary::FT_IPA)) {
            _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_JAR, pOptions, "", 0, true, pPdStruct);
        }

        if (stFT.contains(XBinary::FT_JAR)) {
            _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_ZIP, pOptions, "", 0, true, pPdStruct);
        }
    }

    XBinary::SCANID scanIdMain = {};

    if (stFT.contains(XBinary::FT_PE32)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_PE32, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_PE64)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_PE64, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_ELF32)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_ELF32, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_ELF64)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_ELF64, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_MACHO32)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_MACHO32, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_MACHO64)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_MACHO64, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_LX)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_LX, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_LE)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_LE, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_NE)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_NE, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_MSDOS)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_MSDOS, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_APK)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_APK, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_IPA)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_IPA, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_JAR)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_JAR, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_ZIP)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_ZIP, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_DEX)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_DEX, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_MACHOFAT)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_MACHOFAT, pOptions, "", 0, true, pPdStruct);
    } else if (stFT.contains(XBinary::FT_COM) && (stFT.size() == 1)) {
        scanIdMain = _processDetect(pScanResult, _pDevice, sFunction, parentId, XBinary::FT_COM, pOptions, "", 0, true, pPdStruct);
    } else {
        SCAN_RESULT _scanResultCOM = {};

        // TODO optomize COM -> 3 times
        _processDetect(&_scanResultCOM, _pDevice, sFunction, parentId, XBinary::FT_COM, pOptions, "", 0, false, pPdStruct);

        bool bAddUnknown = (_scanResultCOM.listRecords.count() == 0);

        SCAN_RESULT _scanResultBinary = {};
        scanIdMain = _processDetect(&_scanResultBinary, _pDevice, sFunction, parentId, XBinary::FT_BINARY, pOptions, "", 0, bAddUnknown, pPdStruct);

        pScanResult->listRecords.append(_scanResultBinary.listRecords);
        pScanResult->listErrors.append(_scanResultBinary.listErrors);
        pScanResult->listDebugRecords.append(_scanResultBinary.listDebugRecords);

        pScanResult->listRecords.append(_scanResultCOM.listRecords);
        pScanResult->listErrors.append(_scanResultCOM.listErrors);
        pScanResult->listDebugRecords.append(_scanResultCOM.listDebugRecords);
    }

    if (pOptions->bIsRecursiveScan) {
        if (stFTOriginal.contains(XBinary::FT_ZIP)) {
            XZip xzip(pDevice);

            if (xzip.isValid(pPdStruct)) {
                QList<XArchive::RECORD> listRecords = xzip.getRecords(20000, pPdStruct);

                qint32 nNumberOfResources = listRecords.count();

                qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
                XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nNumberOfResources);

                for (qint32 i = 0; (i < nNumberOfResources) && (!(pPdStruct->bIsStop)); i++) {
                    QByteArray baRecordData = xzip.decompress(&(listRecords.at(i)), true, pPdStruct);

                    QSet<XBinary::FT> _stFT = XFormats::getFileTypes(&baRecordData, true);

                    if (isScanable(_stFT)) {
                        XBinary::SCANID scanIdResource = scanIdMain;
                        scanIdResource.filePart = XBinary::FILEPART_ARCHIVERECORD;
                        scanIdResource.sInfo = listRecords.at(i).sFileName;

                        qint64 _nUncompressedSize = listRecords.at(i).nUncompressedSize;
                        qint64 _nRecordDataSize = baRecordData.size();

                        if (_nUncompressedSize > _nRecordDataSize) {
                            QTemporaryFile fileTemp;

                            if (fileTemp.open()) {
                                QString sTempFileName = fileTemp.fileName();

                                if (xzip.decompressToFile(&(listRecords.at(i)), sTempFileName, pPdStruct)) {
                                    QFile file;
                                    file.setFileName(sTempFileName);

                                    if (file.open(QIODevice::ReadOnly)) {
                                        process(&file, sFunction, pScanResult, 0, file.size(), scanIdResource, pOptions, false, pPdStruct);
                                        file.close();
                                    }
                                }
                            }
                        } else {
                            QBuffer buffer(&baRecordData);

                            if (buffer.open(QIODevice::ReadOnly)) {
                                process(&buffer, sFunction, pScanResult, 0, buffer.size(), scanIdResource, pOptions, false, pPdStruct);

                                buffer.close();
                            }
                        }
                    }

                    XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
                    XBinary::setPdStructStatus(pPdStruct, _nFreeIndex, listRecords.at(i).sFileName);
                }

                XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
            }

        } else if (stFTOriginal.contains(XBinary::FT_ZLIB) || stFTOriginal.contains(XBinary::FT_LHA)) {
            QList<XArchive::RECORD> listRecords = XArchives::getRecords(_pDevice, -1, pPdStruct);

            if (listRecords.count() == 1) {
                QTemporaryFile fileTemp;

                if (fileTemp.open()) {
                    QString sTempFileName = fileTemp.fileName();

                    XArchive::RECORD record = listRecords.at(0);

                    // TODO
                    XBinary::SCANID scanIdArchiveRecord = scanIdMain;
                    scanIdArchiveRecord.filePart = XBinary::FILEPART_ARCHIVERECORD;
                    scanIdArchiveRecord.sInfo = record.sFileName;

                    if (XArchives::decompressToFile(_pDevice, &record, sTempFileName, pPdStruct)) {
                        QFile file;
                        file.setFileName(sTempFileName);

                        if (file.open(QIODevice::ReadOnly)) {
                            process(&file, sFunction, pScanResult, 0, file.size(), scanIdArchiveRecord, pOptions, false, pPdStruct);
                            file.close();
                        }
                    }
                }
            }
        } else if (stFT.contains(XBinary::FT_PE32) || stFT.contains(XBinary::FT_PE64)) {
            XPE pe(_pDevice);

            if (pe.isValid()) {
                if (pe.isResourcesPresent()) {
                    QList<XPE::RESOURCE_RECORD> listResources = pe.getResources();

                    qint32 nNumberOfResources = listResources.count();

                    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
                    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nNumberOfResources);

                    for (qint32 i = 0; (i < nNumberOfResources) && (!(pPdStruct->bIsStop)); i++) {
                        qint64 nResourceOffset = listResources.at(i).nOffset;
                        qint64 nResourceSize = listResources.at(i).nSize;

                        if (pe.checkOffsetSize(nResourceOffset, nResourceSize)) {
                            QSet<XBinary::FT> _stFT = XFormats::getFileTypes(pDevice, nResourceOffset, nResourceSize);

                            if (isScanable(_stFT)) {
                                XBinary::SCANID scanIdResource = scanIdMain;
                                scanIdResource.filePart = XBinary::FILEPART_RESOURCE;
                                scanIdResource.sInfo = XBinary::valueToHexEx(nResourceOffset);

                                process(_pDevice, sFunction, pScanResult, nResourceOffset, nResourceSize, scanIdResource, pOptions, false, pPdStruct);
                            }
                        }

                        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
                    }

                    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
                }

                if (pe.isOverlayPresent()) {
                    XBinary::SCANID scanIdOverlay = scanIdMain;
                    scanIdOverlay.filePart = XBinary::FILEPART_OVERLAY;

                    process(_pDevice, sFunction, pScanResult, pe.getOverlayOffset(), pe.getOverlaySize(), scanIdOverlay, pOptions, false, pPdStruct);
                }
            }
        }
    }

    if (pSd) {
        pSd->close();

        delete pSd;
    }

    if (pScanTimer) {
        pScanResult->nScanTime = pScanTimer->elapsed();

        delete pScanTimer;
    }
}

DiE_ScriptEngine::SIGNATURE_RECORD DiE_Script::getSignatureByFilePath(const QString &sSignatureFilePath)
{
    DiE_ScriptEngine::SIGNATURE_RECORD result = {};

    qint32 nNumberOfSignatures = g_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (g_listSignatures.at(i).sFilePath == sSignatureFilePath) {
            result = g_listSignatures.at(i);

            break;
        }
    }

    return result;
}

bool DiE_Script::updateSignature(const QString &sSignatureFilePath, const QString &sText)
{
    bool bResult = false;

    qint32 nNumberOfSignatures = g_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (g_listSignatures.at(i).sFilePath == sSignatureFilePath) {
            if (XBinary::writeToFile(sSignatureFilePath, QByteArray().append(sText.toUtf8()))) {
                g_listSignatures[i].sText = sText;
                bResult = true;
            }

            break;
        }
    }

    return bResult;
}

void DiE_Script::process()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    if (g_sDirectoryProcess != "") {
        QList<QString> listFileNames;

        XBinary::findFiles(g_sDirectoryProcess, &listFileNames, g_scanOptionsProcess.bSubdirectories, 0, g_pPdStruct);

        qint32 nTotal = listFileNames.count();

        qint32 _nFreeIndex = XBinary::getFreeIndex(g_pPdStruct);
        XBinary::setPdStructInit(g_pPdStruct, _nFreeIndex, nTotal);

        for (qint32 i = 0; (i < nTotal) && (!(g_pPdStruct->bIsStop)); i++) {
            QString sFileName = listFileNames.at(i);
            //            g_mutex.lock();

            XBinary::setPdStructCurrent(g_pPdStruct, _nFreeIndex, i);
            XBinary::setPdStructStatus(g_pPdStruct, _nFreeIndex, sFileName);

            emit directoryScanFileStarted(sFileName);

            g_scanResultProcess = scanFile(sFileName, &g_scanOptionsProcess, g_pPdStruct);

            emit directoryScanResult(g_scanResultProcess);

            ////            g_mutex.unlock();

            //            // TODO a function
            //            // TODO remove
            //            QEventLoop loop;
            //            QTimer t;
            //            t.connect(&t,&QTimer::timeout,&loop,&QEventLoop::quit);
            //            t.start(1);
            //            loop.exec();
        }

        XBinary::setPdStructFinished(g_pPdStruct, _nFreeIndex);
    } else if (g_pDeviceProcess) {
        g_scanResultProcess = scanDevice(g_pDeviceProcess, &g_scanOptionsProcess, g_pPdStruct);
    }

    emit scanCompleted(elapsedTimer.elapsed());
}

DiE_Script::STATS DiE_Script::getStats()
{
    STATS result = {};

    qint32 nNumberOfSignatures = g_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        QString sText = g_listSignatures.at(i).sText;

        QString sType = XBinary::regExp("init\\(\"(.*?)\",", sText, 1);

        if (sType != "") {
            result.mapTypes.insert(sType, result.mapTypes.value(sType, 0) + 1);
        }
    }

    return result;
}

DiE_Script::DBT DiE_Script::getDatabaseType()
{
    return g_databaseType;
}

bool DiE_Script::isSignaturesPresent(XBinary::FT fileType)
{
    bool bResult = false;

    qint32 nNumberOfSignatures = g_listSignatures.count();

    for (qint32 i = 0; i < nNumberOfSignatures; i++) {
        if (g_listSignatures.at(i).fileType == fileType) {
            bResult = true;

            break;
        }
    }

    return bResult;
}

QString DiE_Script::getErrorsString(DiE_Script::SCAN_RESULT *pScanResult)
{
    QString sResult;

    qint32 nNumberOfErrors = pScanResult->listErrors.count();

    for (qint32 i = 0; i < nNumberOfErrors; i++) {
        sResult += QString("%1: %2\n").arg(pScanResult->listErrors.at(i).sScript, pScanResult->listErrors.at(i).sErrorString);
    }

    return sResult;
}

QList<QString> DiE_Script::getErrorsStringList(SCAN_RESULT *pScanResult)
{
    QList<QString> listResult;

    qint32 nNumberOfErrors = pScanResult->listErrors.count();

    for (qint32 i = 0; i < nNumberOfErrors; i++) {
        listResult.append(QString("%1: %2").arg(pScanResult->listErrors.at(i).sScript, pScanResult->listErrors.at(i).sErrorString));
    }

    return listResult;
}

void DiE_Script::setData(const QString &sDirectory, const OPTIONS &scanOptions, XBinary::PDSTRUCT *pPdStruct)
{
    g_sDirectoryProcess = sDirectory;
    g_scanOptionsProcess = scanOptions;
    g_pPdStruct = pPdStruct;
}

void DiE_Script::setData(QIODevice *pDevice, const OPTIONS &scanOptions, XBinary::PDSTRUCT *pPdStruct)
{
    g_pDeviceProcess = pDevice;
    g_scanOptionsProcess = scanOptions;
    g_pPdStruct = pPdStruct;
}

QList<XBinary::SCANSTRUCT> DiE_Script::convert(QList<DiE_ScriptEngine::SCAN_STRUCT> *pListScanStructs)
{
    QList<XBinary::SCANSTRUCT> listResult;

    qint32 nNumberOfRecords = pListScanStructs->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XBinary::SCANSTRUCT record = {};

        record.bIsHeuristic = pListScanStructs->at(i).bIsHeuristic;
        record.id = pListScanStructs->at(i).id;
        record.parentId = pListScanStructs->at(i).parentId;
        record.sType = pListScanStructs->at(i).sType;
        record.sName = pListScanStructs->at(i).sName;
        record.sVersion = pListScanStructs->at(i).sVersion;
        record.sInfo = pListScanStructs->at(i).sOptions;
        record.varInfo = pListScanStructs->at(i).sSignature;
        record.varInfo2 = pListScanStructs->at(i).sSignatureFileName;

        record.globalColor = XFormats::typeToColor(record.sType);
        record.nPrio = XFormats::typeToPrio(record.sType);
        record.bIsProtection = XFormats::isProtection(record.sType);
        record.sType = XFormats::translateType(record.sType);

        listResult.append(record);
    }

    XFormats::sortRecords(&listResult);

    return listResult;
}

DiE_Script::SCAN_RESULT DiE_Script::getScanResultProcess()
{
    return g_scanResultProcess;
}

bool DiE_Script::isScanable(const QSet<XBinary::FT> &stFT)
{
    return (stFT.contains(XBinary::FT_MSDOS) || stFT.contains(XBinary::FT_NE) || stFT.contains(XBinary::FT_LE) || stFT.contains(XBinary::FT_LX) ||
            stFT.contains(XBinary::FT_PE) || stFT.contains(XBinary::FT_ELF) || stFT.contains(XBinary::FT_MACHO) || stFT.contains(XBinary::FT_DEX) ||
            stFT.contains(XBinary::FT_ARCHIVE));
}

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::setDebugger(QScriptEngineDebugger *pDebugger)
{
    this->g_pDebugger = pDebugger;
}
#endif

#ifdef QT_SCRIPTTOOLS_LIB
void DiE_Script::removeDebugger()
{
    this->g_pDebugger = nullptr;
}
#endif
