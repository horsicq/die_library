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
#include "global_script.h"
global_script::global_script(QObject *pParent) : QObject(pParent)
{
}

void global_script::includeScript(const QString &sScript)
{
    emit includeScriptSignal(sScript);
}

void global_script::_log(const QString &sText)
{
    emit _logSignal(sText);
}

// qint64 global_script::_min(qint64 nValue1,qint64 nValue2)
//{
//     return qMin(nValue1,nValue2);
// }

// qint64 global_script::_max(qint64 nValue1,qint64 nValue2)
//{
//     return qMax(nValue1,nValue2);
// }

void global_script::_setResult(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions)
{
    emit _setResultSignal(sType, sName, sVersion, sOptions);
}

bool global_script::_isResultPresent(const QString &sType, const QString &sName)
{
    bool bResult = false;
    emit _isResultPresentSignal(&bResult, sType, sName);

    return bResult;
}

qint32 global_script::_getNumberOfResults(const QString &sType)
{
    qint32 nResult = 0;
    emit _getNumberOfResultsSignal(&nResult, sType);

    return nResult;
}

void global_script::_removeResult(const QString &sType, const QString &sName)
{
    emit _removeResultSignal(sType, sName);
}

bool global_script::_isStop()
{
    bool bResult = false;
    emit _isStopSignal(&bResult);

    return bResult;
}

void global_script::_encodingList()
{
    emit _encodingListSignal();
}
