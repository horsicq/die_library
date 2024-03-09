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
#include "xdataconvertor.h"

XDataConvertor::XDataConvertor(QObject *pParent) : QObject(pParent)
{
    g_options = {};
}

void XDataConvertor::setData(QIODevice *pDeviceIn, QIODevice *pDeviceOut, CMETHOD method, const OPTIONS &options, XBinary::PDSTRUCT *pPdStruct)
{
    this->g_pDeviceIn = pDeviceIn;
    this->g_pDeviceOut = pDeviceOut;
    this->g_method = method;
    this->g_options = options;
    this->g_pPdStruct = pPdStruct;
}

void XDataConvertor::process()
{
    // mb TODO set selected processed

    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();
    XBinary::PDSTRUCT *pPdStruct = g_pPdStruct;

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QElapsedTimer scanTimer;
    scanTimer.start();

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, 0);

    qint64 nOutSize = 0;
    qint64 nBufferSize = 0;
    qint64 nInSize = g_pDeviceIn->size();
    quint64 nKey = 0;

    // mb TODO adjust
    if ((g_method == CMETHOD_XOR_BYTE) || (g_method == CMETHOD_ADD_BYTE) || (g_method == CMETHOD_SUB_BYTE)) {
        nOutSize = nInSize;
        nBufferSize = 0x1000;
        nKey = (quint8)g_options.varKey.toULongLong();
    } else if ((g_method == CMETHOD_XOR_WORD) || (g_method == CMETHOD_ADD_WORD) || (g_method == CMETHOD_SUB_WORD)) {
        nOutSize = S_ALIGN_DOWN64(nInSize, 2);
        nBufferSize = 0x1000;
        nKey = (quint16)g_options.varKey.toULongLong();
    } else if ((g_method == CMETHOD_XOR_DWORD) || (g_method == CMETHOD_ADD_DWORD) || (g_method == CMETHOD_SUB_DWORD)) {
        nOutSize = S_ALIGN_DOWN64(nInSize, 4);
        nBufferSize = 0x1000;
        nKey = (quint32)g_options.varKey.toULongLong();
    } else if ((g_method == CMETHOD_XOR_QWORD) || (g_method == CMETHOD_ADD_QWORD) || (g_method == CMETHOD_SUB_QWORD)) {
        nOutSize = S_ALIGN_DOWN64(nInSize, 8);
        nBufferSize = 0x1000;
        nKey = (quint64)g_options.varKey.toULongLong();
    } else if (g_method == CMETHOD_BASE64_ENCODE) {
        nOutSize = (nInSize / 3) * 4;
        if (nInSize % 3) {
            nOutSize += 4;
        }
        nBufferSize = 0x1000;
    }

    char *pBuffer = new char[nBufferSize];

    if (XBinary::resize(g_pDeviceOut, nOutSize)) {
        for (qint32 i = 0; i < nInSize;) {
            qint64 _nBufferSize = qMin(nBufferSize, nInSize - i);
            qint64 nProcessedSize = 0;

            bool bInvalidSize = false;

            if ((g_method == CMETHOD_XOR_WORD) || (g_method == CMETHOD_ADD_WORD) || (g_method == CMETHOD_SUB_WORD)) {
                if (_nBufferSize % 2) {
                    bInvalidSize = true;
                }
            } else if ((g_method == CMETHOD_XOR_DWORD) || (g_method == CMETHOD_ADD_DWORD) || (g_method == CMETHOD_SUB_DWORD)) {
                if (_nBufferSize % 4) {
                    bInvalidSize = true;
                }
            } else if ((g_method == CMETHOD_XOR_QWORD) || (g_method == CMETHOD_ADD_QWORD) || (g_method == CMETHOD_SUB_QWORD)) {
                if (_nBufferSize % 8) {
                    bInvalidSize = true;
                }
            }

            g_pDeviceIn->seek(i);
            g_pDeviceOut->seek(i);

            if ((g_pDeviceIn->read(pBuffer, _nBufferSize) != _nBufferSize) || (_nBufferSize == 0)) {
                pPdStruct->sInfoString = tr("Read error");
                break;
            }

            if (g_method == CMETHOD_XOR_BYTE) {
                for (qint32 j = 0; j < _nBufferSize; j++) {
                    pBuffer[j] = pBuffer[j] ^ (quint8)nKey;
                    nProcessedSize++;
                }
            } else if (g_method == CMETHOD_XOR_WORD) {
                for (qint32 j = 0; j < _nBufferSize / 2; j++) {
                    ((quint16 *)pBuffer)[j] = ((quint16 *)pBuffer)[j] ^ (quint16)nKey;
                    nProcessedSize += 2;
                }
            } else if (g_method == CMETHOD_XOR_DWORD) {
                for (qint32 j = 0; j < _nBufferSize / 4; j++) {
                    ((quint32 *)pBuffer)[j] = ((quint32 *)pBuffer)[j] ^ (quint32)nKey;
                    nProcessedSize += 4;
                }
            } else if (g_method == CMETHOD_XOR_QWORD) {
                for (qint32 j = 0; j < _nBufferSize / 8; j++) {
                    ((quint64 *)pBuffer)[j] = ((quint64 *)pBuffer)[j] ^ (quint64)nKey;
                    nProcessedSize += 8;
                }
            } else if (g_method == CMETHOD_ADD_BYTE) {
                for (qint32 j = 0; j < _nBufferSize; j++) {
                    pBuffer[j] = pBuffer[j] + (quint8)nKey;
                    nProcessedSize++;
                }
            } else if (g_method == CMETHOD_ADD_WORD) {
                for (qint32 j = 0; j < _nBufferSize / 2; j++) {
                    ((quint16 *)pBuffer)[j] = ((quint16 *)pBuffer)[j] + (quint16)nKey;
                    nProcessedSize += 2;
                }
            } else if (g_method == CMETHOD_ADD_DWORD) {
                for (qint32 j = 0; j < _nBufferSize / 4; j++) {
                    ((quint32 *)pBuffer)[j] = ((quint32 *)pBuffer)[j] + (quint32)nKey;
                    nProcessedSize += 4;
                }
            } else if (g_method == CMETHOD_ADD_QWORD) {
                for (qint32 j = 0; j < _nBufferSize / 8; j++) {
                    ((quint64 *)pBuffer)[j] = ((quint64 *)pBuffer)[j] + (quint64)nKey;
                    nProcessedSize += 8;
                }
            } else if (g_method == CMETHOD_SUB_BYTE) {
                for (qint32 j = 0; j < _nBufferSize; j++) {
                    pBuffer[j] = pBuffer[j] - (quint8)nKey;
                    nProcessedSize++;
                }
            } else if (g_method == CMETHOD_SUB_WORD) {
                for (qint32 j = 0; j < _nBufferSize / 2; j++) {
                    ((quint16 *)pBuffer)[j] = ((quint16 *)pBuffer)[j] - (quint16)nKey;
                    nProcessedSize += 2;
                }
            } else if (g_method == CMETHOD_SUB_DWORD) {
                for (qint32 j = 0; j < _nBufferSize / 4; j++) {
                    ((quint32 *)pBuffer)[j] = ((quint32 *)pBuffer)[j] - (quint32)nKey;
                    nProcessedSize += 4;
                }
            } else if (g_method == CMETHOD_SUB_QWORD) {
                for (qint32 j = 0; j < _nBufferSize / 8; j++) {
                    ((quint64 *)pBuffer)[j] = ((quint64 *)pBuffer)[j] - (quint64)nKey;
                    nProcessedSize += 8;
                }
            }

            if (g_pDeviceOut->write(pBuffer, nProcessedSize) != nProcessedSize) {
                pPdStruct->sInfoString = tr("Write error");
                break;
            }

            if (bInvalidSize) {
                pPdStruct->sInfoString = tr("Invalid size");
                break;
            }

            i += _nBufferSize;
        }
    } else {
        pPdStruct->sInfoString = tr("Cannot resize");
    }

    delete[] pBuffer;

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    emit completed(scanTimer.elapsed());
}
