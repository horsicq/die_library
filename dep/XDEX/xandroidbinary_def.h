/* Copyright (c) 2019-2023 hors<horsicq@gmail.com>
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
#ifndef XANDROIDBINARY_DEF_H
#define XANDROIDBINARY_DEF_H

#include <QtGlobal>

namespace XANDROIDBINARY_DEF {
struct HEADER {
    quint16 type;
    quint16 header_size;
    quint32 data_size;
};

struct HEADER_STRING_POOL {
    HEADER header;
    quint32 stringCount;
    quint32 styleCount;
    quint32 flags;
    quint32 stringsStart;
    quint32 stylesStart;
};

struct HEADER_NAMESPACE {
    HEADER header;
    quint32 lineNumber;
    quint32 comment;
    quint32 prefix;
    quint32 uri;
};

struct HEADER_XML_ATTRIBUTE {
    quint32 ns;
    quint32 name;
    quint32 rawValue;
    quint16 size;
    quint8 reserved;  // 0
    quint8 dataType;
    quint32 data;
};

struct HEADER_XML_START {
    HEADER header;
    quint32 lineNumber;
    quint32 comment;
    quint32 ns;
    quint32 name;
    quint16 attributeStart;
    quint16 attributeSize;
    quint16 attributeCount;
    quint16 idIndex;
    quint16 classIndex;
    quint16 styleIndex;
};

struct HEADER_XML_END {
    HEADER header;
    quint32 lineNumber;
    quint32 comment;
    quint32 ns;
    quint32 name;
};

enum {
    RES_NULL_TYPE = 0x0000,
    RES_STRING_POOL_TYPE = 0x0001,
    RES_TABLE_TYPE = 0x0002,
    RES_XML_TYPE = 0x0003,
    // Chunk types in RES_XML_TYPE
    RES_XML_FIRST_CHUNK_TYPE = 0x0100,
    RES_XML_START_NAMESPACE_TYPE = 0x0100,
    RES_XML_END_NAMESPACE_TYPE = 0x0101,
    RES_XML_START_ELEMENT_TYPE = 0x0102,
    RES_XML_END_ELEMENT_TYPE = 0x0103,
    RES_XML_CDATA_TYPE = 0x0104,
    RES_XML_LAST_CHUNK_TYPE = 0x017F,
    // This contains a uint32_t array mapping strings in the string
    // pool back to resource identifiers.  It is optional.
    RES_XML_RESOURCE_MAP_TYPE = 0x0180,
    // Chunk types in RES_TABLE_TYPE
    RES_TABLE_PACKAGE_TYPE = 0x0200,
    RES_TABLE_TYPE_TYPE = 0x0201,
    RES_TABLE_TYPE_SPEC_TYPE = 0x0202
};
}  // namespace XANDROIDBINARY_DEF

#endif  // XANDROIDBINARY_DEF_H
