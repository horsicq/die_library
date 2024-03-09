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
#include "xne.h"

XNE::XNE(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XMSDOS(pDevice, bIsImage, nModuleAddress)
{
}

bool XNE::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    quint16 magic = get_magic();

    if (magic == XMSDOS_DEF::S_IMAGE_DOS_SIGNATURE_MZ) {
        qint32 lfanew = get_lfanew();

        if (lfanew > 0) {
            quint32 signature = read_uint16(lfanew);

            if (signature == XNE_DEF::S_IMAGE_OS2_SIGNATURE) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XNE::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XNE xne(pDevice, bIsImage, nModuleAddress);

    return xne.isValid();
}

XBinary::MODE XNE::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XNE xne(pDevice, bIsImage, nModuleAddress);

    return xne.getMode();
}

qint64 XNE::getImageOS2HeaderOffset()
{
    qint64 nResult = get_lfanew();

    if (!_isOffsetValid(nResult)) {
        nResult = -1;
    }

    return nResult;
}

qint64 XNE::getImageOS2HeaderSize()
{
    return sizeof(XNE_DEF::IMAGE_OS2_HEADER);
}

XNE_DEF::IMAGE_OS2_HEADER XNE::getImageOS2Header()
{
    XNE_DEF::IMAGE_OS2_HEADER result = {};

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        result.ne_magic = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic));
        result.ne_ver = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver));
        result.ne_rev = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev));
        result.ne_enttab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab));
        result.ne_cbenttab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab));
        result.ne_crc = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc));
        result.ne_flags = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags));
        result.ne_autodata = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata));
        result.ne_heap = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap));
        result.ne_stack = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack));
        result.ne_csip = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip));
        result.ne_sssp = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp));
        result.ne_cseg = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg));
        result.ne_cmod = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod));
        result.ne_cbnrestab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab));
        result.ne_segtab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab));
        result.ne_rsrctab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab));
        result.ne_restab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab));
        result.ne_modtab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab));
        result.ne_imptab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab));
        result.ne_nrestab = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab));
        result.ne_cmovent = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent));
        result.ne_align = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align));
        result.ne_cres = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres));
        result.ne_exetyp = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp));
        result.ne_flagsothers = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers));
        result.ne_pretthunks = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks));
        result.ne_psegrefbytes = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes));
        result.ne_swaparea = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea));
        result.ne_expver = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver));
    }

    return result;
}

quint16 XNE::getImageOS2Header_magic()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_ver()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_rev()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_enttab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cbenttab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_crc()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_flags()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_autodata()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_heap()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_stack()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_csip()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_sssp()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cseg()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cmod()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cbnrestab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_segtab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_rsrctab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_restab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_modtab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_imptab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_nrestab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cmovent()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_align()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cres()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_exetyp()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_flagsothers()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_pretthunks()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_psegrefbytes()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_swaparea()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_expver()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver));
    }

    return nResult;
}

void XNE::setImageOS2Header_magic(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic), nValue);
    }
}

void XNE::setImageOS2Header_ver(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver), nValue);
    }
}

void XNE::setImageOS2Header_rev(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev), nValue);
    }
}

void XNE::setImageOS2Header_enttab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab), nValue);
    }
}

void XNE::setImageOS2Header_cbenttab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab), nValue);
    }
}

void XNE::setImageOS2Header_crc(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc), nValue);
    }
}

void XNE::setImageOS2Header_flags(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags), nValue);
    }
}

void XNE::setImageOS2Header_autodata(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata), nValue);
    }
}

void XNE::setImageOS2Header_heap(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap), nValue);
    }
}

void XNE::setImageOS2Header_stack(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack), nValue);
    }
}

void XNE::setImageOS2Header_csip(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip), nValue);
    }
}

void XNE::setImageOS2Header_sssp(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp), nValue);
    }
}

void XNE::setImageOS2Header_cseg(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg), nValue);
    }
}

void XNE::setImageOS2Header_cmod(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod), nValue);
    }
}

void XNE::setImageOS2Header_cbnrestab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab), nValue);
    }
}

void XNE::setImageOS2Header_segtab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab), nValue);
    }
}

void XNE::setImageOS2Header_rsrctab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab), nValue);
    }
}

void XNE::setImageOS2Header_restab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab), nValue);
    }
}

void XNE::setImageOS2Header_modtab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab), nValue);
    }
}

void XNE::setImageOS2Header_imptab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab), nValue);
    }
}

void XNE::setImageOS2Header_nrestab(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab), nValue);
    }
}

void XNE::setImageOS2Header_cmovent(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent), nValue);
    }
}

void XNE::setImageOS2Header_align(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align), nValue);
    }
}

void XNE::setImageOS2Header_cres(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres), nValue);
    }
}

void XNE::setImageOS2Header_exetyp(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp), nValue);
    }
}

void XNE::setImageOS2Header_flagsothers(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers), nValue);
    }
}

void XNE::setImageOS2Header_pretthunks(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks), nValue);
    }
}

void XNE::setImageOS2Header_psegrefbytes(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes), nValue);
    }
}

void XNE::setImageOS2Header_swaparea(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea), nValue);
    }
}

void XNE::setImageOS2Header_expver(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver), nValue);
    }
}

qint64 XNE::getEntryTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_enttab();
}

qint64 XNE::getEntryTableSize()
{
    return getImageOS2Header_cbenttab();
}

qint64 XNE::getSegmentTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_segtab();
}

qint64 XNE::getResourceTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_rsrctab();
}

qint64 XNE::getResidentNameTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_restab();
}

qint64 XNE::getModuleReferenceTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_modtab();
}

qint64 XNE::getImportedNamesTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_imptab();
}

qint64 XNE::getNotResindentNameTableOffset()
{
    return getImageOS2Header_nrestab();
}

QList<XNE_DEF::NE_SEGMENT> XNE::getSegmentList()
{
    QList<XNE_DEF::NE_SEGMENT> listResult;

    qint64 nOffset = getSegmentTableOffset();
    qint32 nNumberOfSegments = getImageOS2Header_cseg();

    for (qint32 i = 0; i < nNumberOfSegments; i++) {
        XNE_DEF::NE_SEGMENT segment = _read_NE_SEGMENT(nOffset);

        listResult.append(segment);

        nOffset += sizeof(XNE_DEF::NE_SEGMENT);
    }

    return listResult;
}

XNE_DEF::NE_SEGMENT XNE::_read_NE_SEGMENT(qint64 nOffset)
{
    XNE_DEF::NE_SEGMENT result = {};

    result.dwFileOffset = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwFileOffset));
    result.dwFileSize = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwFileSize));
    result.dwFlags = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwFlags));
    result.dwMinAllocSize = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwMinAllocSize));

    return result;
}

XBinary::_MEMORY_MAP XNE::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP result = {};

    qint32 nIndex = 0;

    result.sArch = getArch();
    result.sType = getTypeAsString();

    result.fileType = FT_NE;
    result.mode = MODE_16SEG;
    result.nBinarySize = getSize();

    quint16 nShift = getImageOS2Header_align();
    QList<XNE_DEF::NE_SEGMENT> listSegments = getSegmentList();

    qint32 nNumberOfSegments = listSegments.count();

    result.nModuleAddress = getModuleAddress();       // TODO const
    result.nImageSize = nNumberOfSegments * 0x10000;  // TODO Check
    result.nEntryPointAddress = getImageOS2Header_csip();

    qint64 nMaxOffset = 0;

    for (qint32 i = 0; i < nNumberOfSegments; i++) {
        qint64 nFileSize = listSegments.at(i).dwFileSize;
        qint64 nFileOffset = listSegments.at(i).dwFileOffset << nShift;

        if (nFileSize == 0) {
            nFileSize = 0x10000;
        }

        nFileSize = S_ALIGN_UP(nFileSize, 0x200);  // TODO const

        if (nFileOffset)  // if offset = 0 no data
        {
            _MEMORY_RECORD record = {};
            record.nSize = nFileSize;
            record.nOffset = nFileOffset;
            record.nAddress = (i + 1) * 0x10000;  // TODO const
            record.segment = ADDRESS_SEGMENT_UNKNOWN;
            record.type = MMT_LOADSEGMENT;
            record.nIndex = nIndex++;

            result.listRecords.append(record);
        }

        if (0x10000 - nFileSize) {
            _MEMORY_RECORD record = {};
            record.nSize = 0x10000 - nFileSize;
            record.nOffset = -1;
            record.nAddress = (i + 1) * 0x10000 + nFileSize;  // TODO const
            record.segment = ADDRESS_SEGMENT_UNKNOWN;
            record.type = MMT_LOADSEGMENT;
            record.nIndex = nIndex++;
            record.bIsVirtual = true;

            result.listRecords.append(record);
        }

        if (nFileOffset) {
            nMaxOffset = qMax(nMaxOffset, nFileOffset + nFileSize);
        }
    }

    // TODO Overlay !!!
    // Check overlay!!
    //    qint64 nOverlaySize=result.nRawSize-nMaxOffset;

    //    if(nOverlaySize>0)
    //    {
    //        XBinary::_MEMORY_RECORD record={};

    //        record.type=MMT_OVERLAY;
    //        record.nAddress=-1;
    //        record.nSize=nOverlaySize;
    //        record.nOffset=nMaxOffset;
    //        record.nIndex=nIndex++;

    //        result.listRecords.append(record);
    //    }

    return result;
}

QMap<quint64, QString> XNE::getImageNEMagics()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x454E, "IMAGE_OS2_SIGNATURE");

    return mapResult;
}

QMap<quint64, QString> XNE::getImageNEMagicsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x454E, "OS2_SIGNATURE");

    return mapResult;
}

QMap<quint64, QString> XNE::getImageNEFlagsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0001, "single shared");
    mapResult.insert(0x0002, "multiple");
    mapResult.insert(0x0004, "Global initialization");
    mapResult.insert(0x0008, "Protected mode only");
    mapResult.insert(0x0010, "8086 instructions");
    mapResult.insert(0x0020, "80286 instructions");
    mapResult.insert(0x0040, "80386 instructions");
    mapResult.insert(0x0080, "80x87 instructions");
    mapResult.insert(0x0100, "Full screen");
    mapResult.insert(0x0200, "Compatible with Windows/P.M.");
    //    mapResult.insert(0x0400,""); // TODO Check !!!
    mapResult.insert(0x0800, "OS/2 family application");
    mapResult.insert(0x1000, "reserved?");
    mapResult.insert(0x2000, "Errors in image/executable");
    mapResult.insert(0x4000, "non-conforming program");
    mapResult.insert(0x8000, "DLL or driver");

    return mapResult;
}

QMap<quint64, QString> XNE::getImageNEExetypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0000, tr("Unknown"));
    mapResult.insert(0x0001, "OS/2");
    mapResult.insert(0x0002, "Windows");
    mapResult.insert(0x0003, "European MS-DOS 4.x");
    mapResult.insert(0x0004, "Windows 386");
    mapResult.insert(0x0005, "BOSS (Borland Operating System Services)");

    return mapResult;
}

QMap<quint64, QString> XNE::getImageNEFlagsothersS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0001, "Long filename support");
    mapResult.insert(0x0002, "2.x protected mode");
    mapResult.insert(0x0004, "2.x proportional fonts");
    mapResult.insert(0x0008, "Executable has gangload area");

    return mapResult;
}

QMap<quint64, QString> XNE::getImageSegmentTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0000, "CODE");
    mapResult.insert(0x0001, "DATA");

    return mapResult;
}

qint64 XNE::getModuleAddress()
{
    return 0x10000;
}

XBinary::MODE XNE::getMode()
{
    return MODE_16SEG;
}

QString XNE::getArch()
{
    QString sResult = QString("8086");

    // TODO

    return sResult;
}

XBinary::ENDIAN XNE::getEndian()
{
    return ENDIAN_LITTLE;
}

XBinary::FT XNE::getFileType()
{
    return FT_NE;
}

qint32 XNE::getType()
{
    return TYPE_EXE;  // TODO
}

XBinary::OSINFO XNE::getOsInfo()
{
    OSINFO result = {};

    result.osName = OSNAME_UNKNOWN;
    result.sOsVersion = "";

    quint16 nOS = getImageOS2Header_exetyp();

    if (nOS == 1) {
        result.osName = OSNAME_OS2;
    } else if (nOS == 2) {
        result.osName = OSNAME_WINDOWS;
    } else if (nOS == 3) {
        result.osName = OSNAME_MSDOS;
        result.sOsVersion = "4.X";
    } else if (nOS == 4) {
        result.osName = OSNAME_WINDOWS;
        result.sOsVersion = "386";
    } else if (nOS == 4) {
        result.osName = OSNAME_BORLANDOSSERVICES;
        result.sOsVersion = "386";
    }

    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    return result;
}

QString XNE::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_EXE: sResult = QString("EXE"); break;
        case TYPE_DLL: sResult = QString("DLL"); break;
        case TYPE_DRIVER: sResult = QString("Driver"); break;
    }

    return sResult;
}
