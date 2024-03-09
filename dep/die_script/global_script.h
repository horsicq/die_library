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
#ifndef GLOBAL_SCRIPT_H
#define GLOBAL_SCRIPT_H

// TODO rename to die_global_script
#include <QObject>
#include "xoptions.h"

class global_script : public QObject {
    Q_OBJECT

public:
    explicit global_script(QObject *pParent = nullptr);

public slots:
    void includeScript(const QString &sScript);
    void _log(const QString &sText);
    //    qint64 _min(qint64 nValue1,qint64 nValue2);
    //    qint64 _max(qint64 nValue1,qint64 nValue2);
    void _setResult(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions);
    bool _isResultPresent(const QString &sType, const QString &sName);
    qint32 _getNumberOfResults(const QString &sType);
    void _removeResult(const QString &sType, const QString &sName);
    bool _isStop();
    void _encodingList();

signals:
    void includeScriptSignal(const QString &sScript);
    void _logSignal(const QString &sText);
    void _setResultSignal(const QString &sType, const QString &sName, const QString &sVersion, const QString &sOptions);
    void _isResultPresentSignal(bool *pbResult, const QString &sType, const QString &sName);
    void _getNumberOfResultsSignal(qint32 *pnResult, const QString &sType);
    void _removeResultSignal(const QString &sType, const QString &sName);
    void _isStopSignal(bool *pResult);
    void _encodingListSignal();
};

#endif  // GLOBAL_SCRIPT_H
