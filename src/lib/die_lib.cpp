/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#include "die_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace QCoreAppDLL {
static int argc = 1;
static char arg0[] = "die.dll";
static char *argv[] = {arg0, nullptr};
static QCoreApplication *pApp = nullptr;
}  // namespace QCoreAppDLL

LIB_SOURCE_EXPORT char *DIE_ScanFileA(char *pszFileName, unsigned int nFlags, char *pszDatabase)
{
    return DIE_lib().scanFileA(pszFileName, nFlags, pszDatabase);
}

LIB_SOURCE_EXPORT wchar_t *DIE_ScanFileW(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase)
{
    return DIE_lib().scanFileW(pwszFileName, nFlags, pwszDatabase);
}

LIB_SOURCE_EXPORT void DIE_FreeMemoryA(char *pszString)
{
    DIE_lib().freeMemoryA(pszString);
}

LIB_SOURCE_EXPORT void DIE_FreeMemoryW(wchar_t *pwszString)
{
    DIE_lib().freeMemoryW(pwszString);
}
#ifdef Q_OS_WIN32
LIB_SOURCE_EXPORT int DIE_VB_ScanFile(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase, wchar_t *pwszBuffer, int nBufferSize)
{
    return DIE_lib().VB_ScanFile(pwszFileName, nFlags, pwszDatabase, pwszBuffer, nBufferSize);
}
#endif

#ifdef __cplusplus
}
#endif

DIE_lib::DIE_lib()
{
    QCoreAppDLL::pApp = new QCoreApplication(QCoreAppDLL::argc, QCoreAppDLL::argv);
    //    QCoreAppDLL::pApp->exec();
}

DIE_lib::~DIE_lib()
{
    if (QCoreAppDLL::pApp) delete qApp;
}

char *DIE_lib::scanFileA(char *pszFileName, unsigned int nFlags, char *pszDatabase)
{
    QString sResult = _scanFile(pszFileName, nFlags, pszDatabase);

    QByteArray baResult = sResult.toUtf8();

    char *pMemory = new char[baResult.size() + 1];

    XBinary::_copyMemory(pMemory, baResult.data(), baResult.size());
    pMemory[baResult.size()] = 0;

    return pMemory;
}

wchar_t *DIE_lib::scanFileW(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase)
{
    QString sResult = _scanFile(XBinary::_fromWCharArray(pwszFileName, -1), nFlags, XBinary::_fromWCharArray(pwszDatabase, -1));

    wchar_t *pMemory = new wchar_t[sResult.size() + 1];

    XBinary::_toWCharArray(sResult, pMemory);

    return pMemory;
}

void DIE_lib::freeMemoryA(char *pszString)
{
    delete[] pszString;
}

void DIE_lib::freeMemoryW(wchar_t *pwszString)
{
    delete[] pwszString;
}
#ifdef Q_OS_WIN32
int DIE_lib::VB_ScanFile(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase, wchar_t *pwszBuffer, int nBufferSize)
{
    int nResult = 0;

    QString sResult = _scanFile(XBinary::_fromWCharArray(pwszFileName, -1), nFlags, XBinary::_fromWCharArray(pwszDatabase, -1));

    if (sResult.size() < nBufferSize) {
        XBinary::_toWCharArray(sResult, pwszBuffer);
        nResult = sResult.size();
    }

    return nResult;
}
#endif
QString DIE_lib::_scanFile(QString sFileName, quint32 nFlags, QString sDatabase)
{
    QString sResult;

    XScanEngine::SCAN_OPTIONS scanOptions = {};

    scanOptions.bShowType = true;
    scanOptions.bShowVersion = true;
    scanOptions.bShowInfo = true;
    scanOptions.nBufferSize = 2 * 1024 * 1024;

    if (nFlags & SF_DEEPSCAN) {
        scanOptions.bIsDeepScan = true;
    }

    if (nFlags & SF_HEURISTICSCAN) {
        scanOptions.bIsHeuristicScan = true;
    }

    if (nFlags & SF_VERBOSE) {
        scanOptions.bIsVerbose = true;
    }

    if (nFlags & SF_ALLTYPESSCAN) {
        scanOptions.bIsAllTypesScan = true;
    }

    if (nFlags & SF_RECURSIVESCAN) {
        scanOptions.bIsRecursiveScan = true;
    }

    if (nFlags & SF_RESULTASJSON) {
        scanOptions.bResultAsJSON = true;
    }

    if (nFlags & SF_RESULTASXML) {
        scanOptions.bResultAsXML = true;
    }

    if (nFlags & SF_RESULTASTSV) {
        scanOptions.bResultAsTSV = true;
    }

    if (nFlags & SF_RESULTASCSV) {
        scanOptions.bResultAsCSV = true;
    }

    DiE_Script dieScript;

    if (sDatabase == "") {
        sDatabase = "$app/db";
    }

    dieScript.loadDatabase(sDatabase, DiE_ScriptEngine::DT_MAIN);  // TODO Check

    XScanEngine::SCAN_RESULT scanResult = dieScript.scanFile(sFileName, &scanOptions);

    ScanItemModel model(&scanOptions, &(scanResult.listRecords), 1);

    XBinary::FORMATTYPE formatType = XBinary::FORMATTYPE_TEXT;

    if (scanOptions.bResultAsCSV) formatType = XBinary::FORMATTYPE_CSV;
    else if (scanOptions.bResultAsJSON) formatType = XBinary::FORMATTYPE_JSON;
    else if (scanOptions.bResultAsTSV) formatType = XBinary::FORMATTYPE_TSV;
    else if (scanOptions.bResultAsXML) formatType = XBinary::FORMATTYPE_XML;

    sResult = model.toString(formatType);

    return sResult;
}
