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
#ifndef XMSDOS_DEF_H
#define XMSDOS_DEF_H

#include <QtGlobal>

namespace XMSDOS_DEF {
const quint16 S_IMAGE_DOS_SIGNATURE = 0x5A4D;     // MZ
const quint16 S_IMAGE_DOS_SIGNATURE_MZ = 0x5A4D;  // MZ
const quint16 S_IMAGE_DOS_SIGNATURE_ZM = 0x4D5A;  // ZM

struct IMAGE_DOS_HEADER {
    quint16 e_magic;    /* 00: MZ Header signature */
    quint16 e_cblp;     /* 02: Bytes on last page of file */
    quint16 e_cp;       /* 04: Pages in file */
    quint16 e_crlc;     /* 06: Relocations */
    quint16 e_cparhdr;  /* 08: Size of header in paragraphs */
    quint16 e_minalloc; /* 0a: Minimum extra paragraphs needed */
    quint16 e_maxalloc; /* 0c: Maximum extra paragraphs needed */
    quint16 e_ss;       /* 0e: Initial (relative) SS value */
    quint16 e_sp;       /* 10: Initial SP value */
    quint16 e_csum;     /* 12: Checksum */
    quint16 e_ip;       /* 14: Initial IP value */
    quint16 e_cs;       /* 16: Initial (relative) CS value */
    quint16 e_lfarlc;   /* 18: File address of relocation table */
    quint16 e_ovno;     /* 1a: Overlay number */
};

struct IMAGE_DOS_HEADEREX {
    quint16 e_magic;    /* 00: MZ Header signature */
    quint16 e_cblp;     /* 02: Bytes on last page of file */
    quint16 e_cp;       /* 04: Pages in file */
    quint16 e_crlc;     /* 06: Relocations */
    quint16 e_cparhdr;  /* 08: Size of header in paragraphs */
    quint16 e_minalloc; /* 0a: Minimum extra paragraphs needed */
    quint16 e_maxalloc; /* 0c: Maximum extra paragraphs needed */
    quint16 e_ss;       /* 0e: Initial (relative) SS value */
    quint16 e_sp;       /* 10: Initial SP value */
    quint16 e_csum;     /* 12: Checksum */
    quint16 e_ip;       /* 14: Initial IP value */
    quint16 e_cs;       /* 16: Initial (relative) CS value */
    quint16 e_lfarlc;   /* 18: File address of relocation table */
    quint16 e_ovno;     /* 1a: Overlay number */
    // Extra
    quint16 e_res[4];   /* 1c: Reserved words */
    quint16 e_oemid;    /* 24: OEM identifier (for e_oeminfo) */
    quint16 e_oeminfo;  /* 26: OEM information; e_oemid specific */
    quint16 e_res2[10]; /* 28: Reserved words */
    qint32 e_lfanew;    /* 3c: Offset to extended header */
};
}  // namespace XMSDOS_DEF
#endif  // XMSDOS_DEF_H
