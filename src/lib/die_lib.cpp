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

LIB_SOURCE_EXPORT char *DIE_ScanMemoryA(char *pMemory, int nMemorySize, unsigned int nFlags, char *pszDatabase)
{
    return DIE_lib().scanMemoryA(pMemory, nMemorySize, nFlags, pszDatabase);
}

LIB_SOURCE_EXPORT wchar_t *DIE_ScanMemoryW(char *pMemory, int nMemorySize, unsigned int nFlags, wchar_t *pwszDatabase)
{
    return DIE_lib().scanMemoryW(pMemory, nMemorySize, nFlags, pwszDatabase);
}

LIB_SOURCE_EXPORT bool DIE_LoadDatabaseA(char *pszDatabase)
{
    return DIE_lib().loadDatabaseA(pszDatabase);
}

LIB_SOURCE_EXPORT bool DIE_LoadDatabaseW(wchar_t *pwszDatabase)
{
    return DIE_lib().loadDatabaseW(pwszDatabase);
}

LIB_SOURCE_EXPORT char *DIE_ScanFileExA(char *pszFileName, unsigned int nFlags)
{
    return DIE_lib().scanFileExA(pszFileName, nFlags);
}

LIB_SOURCE_EXPORT wchar_t *DIE_ScanFileExW(wchar_t *pwszFileName, unsigned int nFlags)
{
    return DIE_lib().scanFileExW(pwszFileName, nFlags);
}

LIB_SOURCE_EXPORT char *DIE_ScanMemoryExA(char *pMemory, int nMemorySize, unsigned int nFlags)
{
    return DIE_lib().scanMemoryExA(pMemory, nMemorySize, nFlags);
}

LIB_SOURCE_EXPORT wchar_t *DIE_ScanMemoryExW(char *pMemory, int nMemorySize, unsigned int nFlags)
{
    return DIE_lib().scanMemoryExW(pMemory, nMemorySize, nFlags);
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
    g_pDieScript = new DiE_Script;
    //    QCoreAppDLL::pApp->exec();
}

DIE_lib::~DIE_lib()
{
    if (g_pDieScript) delete g_pDieScript;
    if (QCoreAppDLL::pApp) delete qApp;
}

char *DIE_lib::scanFileA(char *pszFileName, unsigned int nFlags, char *pszDatabase)
{
    QString sResult = _scanFile(pszFileName, nFlags, pszDatabase);

    QByteArray baResult = sResult.toUtf8();

    char *bBuffer = new char[baResult.size() + 1];

    XBinary::_copyMemory(bBuffer, baResult.data(), baResult.size());
    bBuffer[baResult.size()] = 0;

    return bBuffer;
}

wchar_t *DIE_lib::scanFileW(wchar_t *pwszFileName, unsigned int nFlags, wchar_t *pwszDatabase)
{
    QString sResult = _scanFile(XBinary::_fromWCharArray(pwszFileName, -1), nFlags, XBinary::_fromWCharArray(pwszDatabase, -1));

    wchar_t *bBuffer = new wchar_t[sResult.size() + 1];

    XBinary::_toWCharArray(sResult, bBuffer);

    return bBuffer;
}

char *DIE_lib::scanMemoryA(char *pMemory, int nMemorySize, unsigned int nFlags, char *pszDatabase)
{
    QString sResult = _scanMemory(pMemory, nMemorySize, nFlags, pszDatabase);

    QByteArray baResult = sResult.toUtf8();

    char *pBuffer = new char[baResult.size() + 1];

    XBinary::_copyMemory(pBuffer, baResult.data(), baResult.size());
    pBuffer[baResult.size()] = 0;

    return pBuffer;
}

wchar_t *DIE_lib::scanMemoryW(char *pMemory, int nMemorySize, unsigned int nFlags, wchar_t *pwszDatabase)
{
    QString sResult = _scanMemory(pMemory, nMemorySize, nFlags, XBinary::_fromWCharArray(pwszDatabase, -1));

    int nSize = (sResult.size() + 1) * 2;

    char *pBuffer = new char[nSize];

    sResult.toWCharArray((wchar_t *)pBuffer);

    return (wchar_t *)pBuffer;
}

bool DIE_lib::loadDatabaseA(char *pszDatabase)
{
    return _loadDatabase(pszDatabase);
}

bool DIE_lib::loadDatabaseW(wchar_t *pwszDatabase)
{
    return _loadDatabase(XBinary::_fromWCharArray(pwszDatabase, -1));
}

char *DIE_lib::scanFileExA(char *pszFileName, unsigned int nFlags)
{
    QString sResult = _scanFile(pszFileName, nFlags);

    QByteArray baResult = sResult.toUtf8();

    char *bBuffer = new char[baResult.size() + 1];

    XBinary::_copyMemory(bBuffer, baResult.data(), baResult.size());
    bBuffer[baResult.size()] = 0;

    return bBuffer;
}

wchar_t *DIE_lib::scanFileExW(wchar_t *pwszFileName, unsigned int nFlags)
{
    QString sResult = _scanFile(XBinary::_fromWCharArray(pwszFileName, -1), nFlags);

    wchar_t *bBuffer = new wchar_t[sResult.size() + 1];

    XBinary::_toWCharArray(sResult, bBuffer);

    return bBuffer;
}

char *DIE_lib::scanMemoryExA(char *pMemory, int nMemorySize, unsigned int nFlags)
{
    QString sResult = _scanMemory(pMemory, nMemorySize, nFlags);

    QByteArray baResult = sResult.toUtf8();

    char *pBuffer = new char[baResult.size() + 1];

    XBinary::_copyMemory(pBuffer, baResult.data(), baResult.size());
    pBuffer[baResult.size()] = 0;

    return pBuffer;
}

wchar_t *DIE_lib::scanMemoryExW(char *pMemory, int nMemorySize, unsigned int nFlags)
{
    QString sResult = _scanMemory(pMemory, nMemorySize, nFlags);

    int nSize = (sResult.size() + 1) * 2;

    char *pBuffer = new char[nSize];

    sResult.toWCharArray((wchar_t *)pBuffer);

    return (wchar_t *)pBuffer;
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
bool DIE_lib::_loadDatabase(QString sDatabase)
{
    bool bResult = false;

    if (g_pDieScript) {
        bResult = g_pDieScript->loadDatabase(sDatabase, DiE_ScriptEngine::DT_MAIN);
    }

    return bResult;
}

QString DIE_lib::_scanFile(QString sFileName, quint32 nFlags)
{
    XScanEngine::SCAN_OPTIONS scanOptions = XScanEngine::getDefaultOptions(nFlags);

    XScanEngine::SCAN_RESULT scanResult = g_pDieScript->scanFile(sFileName, &scanOptions);
    ScanItemModel model(&scanOptions, &(scanResult.listRecords), 1);

    return model.toString();
}

QString DIE_lib::_scanMemory(char *pMemory, int nMemorySize, quint32 nFlags)
{
    XScanEngine::SCAN_OPTIONS scanOptions = XScanEngine::getDefaultOptions(nFlags);

    XScanEngine::SCAN_RESULT scanResult = g_pDieScript->scanMemory(pMemory, nMemorySize, &scanOptions);
    ScanItemModel model(&scanOptions, &(scanResult.listRecords), 1);

    return model.toString();
}

QString DIE_lib::_scanFile(QString sFileName, quint32 nFlags, QString sDatabase)
{
    XScanEngine::SCAN_OPTIONS scanOptions = XScanEngine::getDefaultOptions(nFlags);
    DiE_Script dieScript;

    dieScript.loadDatabase(sDatabase, DiE_ScriptEngine::DT_MAIN);

    XScanEngine::SCAN_RESULT scanResult = dieScript.scanFile(sFileName, &scanOptions);
    ScanItemModel model(&scanOptions, &(scanResult.listRecords), 1);

    return model.toString();
}

QString DIE_lib::_scanMemory(char *pMemory, int nMemorySize, quint32 nFlags, QString sDatabase)
{
    XScanEngine::SCAN_OPTIONS scanOptions = XScanEngine::getDefaultOptions(nFlags);
    DiE_Script dieScript;

    if (sDatabase == "") {
        sDatabase = "$app/db";
    }

    dieScript.loadDatabase(sDatabase, DiE_ScriptEngine::DT_MAIN);

    XScanEngine::SCAN_RESULT scanResult = dieScript.scanMemory(pMemory, nMemorySize, &scanOptions);
    ScanItemModel model(&scanOptions, &(scanResult.listRecords), 1);

    return model.toString();
}
