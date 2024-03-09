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
#include "xiodevice.h"

XIODevice::XIODevice(QObject *pParent) : QIODevice(pParent)
{
    g_nSize = 0;
    g_nInitLocation = 0;
}

void XIODevice::setSize(quint64 nSize)
{
    g_nSize = nSize;
}

void XIODevice::setInitLocation(quint64 nLocation)
{
    g_nInitLocation = nLocation;
}

quint64 XIODevice::getInitLocation()
{
    return g_nInitLocation;
}

qint64 XIODevice::size() const
{
    return g_nSize;
}

bool XIODevice::isSequential() const
{
    return false;
}

bool XIODevice::seek(qint64 nPos)
{
    bool bResult = false;

    if ((nPos < size()) && (nPos >= 0)) {
        bResult = QIODevice::seek(nPos);
    }

    return bResult;
}

bool XIODevice::reset()
{
    return seek(0);
}

bool XIODevice::open(OpenMode mode)
{
    setOpenMode(mode);

    return true;
}

bool XIODevice::atEnd() const
{
    return (bytesAvailable() == 0);
}

void XIODevice::close()
{
    setOpenMode(NotOpen);
}

qint64 XIODevice::pos() const
{
    return QIODevice::pos();
}

qint64 XIODevice::readData(char *pData, qint64 nMaxSize)
{
    Q_UNUSED(pData)
    Q_UNUSED(nMaxSize)

#ifdef QT_DEBUG
    qCritical("TODO: XIODevice::readData");
#endif

    return 0;
}

qint64 XIODevice::writeData(const char *pData, qint64 nMaxSize)
{
    Q_UNUSED(pData)
    Q_UNUSED(nMaxSize)

#ifdef QT_DEBUG
    qCritical("TODO: XIODevice::writeData");
#endif

    return 0;
}

void XIODevice::setErrorString(const QString &sString)
{
    QIODevice::setErrorString(sString);
}
