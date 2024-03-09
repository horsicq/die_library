/* Copyright (c) 2017-2024 hors<horsicq@gmail.com>
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
#ifndef XDEX_DEF_H
#define XDEX_DEF_H

#include <QtGlobal>

namespace XDEX_DEF {
struct HEADER {
    quint32 magic;    // always LE
    quint32 version;  // always LE
    quint32 checksum;
    quint8 signature[20];
    quint32 file_size;
    quint32 header_size;
    quint32 endian_tag;  // always LE
    quint32 link_size;
    quint32 link_off;
    quint32 map_off;
    quint32 string_ids_size;
    quint32 string_ids_off;
    quint32 type_ids_size;
    quint32 type_ids_off;
    quint32 proto_ids_size;
    quint32 proto_ids_off;
    quint32 field_ids_size;
    quint32 field_ids_off;
    quint32 method_ids_size;
    quint32 method_ids_off;
    quint32 class_defs_size;
    quint32 class_defs_off;
    quint32 data_size;
    quint32 data_off;
};

struct MAP_ITEM {
    quint16 nType;
    quint32 nCount;
    quint32 nOffset;
};

struct STRING_ITEM_ID {
    quint32 string_data_off;
};

struct TYPE_ITEM_ID {
    quint32 descriptor_idx;
};

struct PROTO_ITEM_ID {
    quint32 shorty_idx;
    quint32 return_type_idx;
    quint32 parameters_off;
};

struct FIELD_ITEM_ID {
    quint16 class_idx;
    quint16 type_idx;
    quint32 name_idx;
};

struct METHOD_ITEM_ID {
    quint16 class_idx;
    quint16 proto_idx;
    quint32 name_idx;
};

struct CLASS_ITEM_DEF {
    quint32 class_idx;
    quint32 access_flags;
    quint32 superclass_idx;
    quint32 interfaces_off;
    quint32 source_file_idx;
    quint32 annotations_off;
    quint32 class_data_off;
    quint32 static_values_off;
};

const quint16 TYPE_HEADER_ITEM = 0x0000;
const quint16 TYPE_STRING_ID_ITEM = 0x0001;
const quint16 TYPE_TYPE_ID_ITEM = 0x0002;
const quint16 TYPE_PROTO_ID_ITEM = 0x0003;
const quint16 TYPE_FIELD_ID_ITEM = 0x0004;
const quint16 TYPE_METHOD_ID_ITEM = 0x0005;
const quint16 TYPE_CLASS_DEF_ITEM = 0x0006;
const quint16 TYPE_CALL_SITE_ID_ITEM = 0x0007;
const quint16 TYPE_METHOD_HANDLE_ITEM = 0x0008;
const quint16 TYPE_MAP_LIST = 0x1000;
const quint16 TYPE_TYPE_LIST = 0x1001;
const quint16 TYPE_ANNOTATION_SET_REF_LIST = 0x1002;
const quint16 TYPE_ANNOTATION_SET_ITEM = 0x1003;
const quint16 TYPE_CLASS_DATA_ITEM = 0x2000;
const quint16 TYPE_CODE_ITEM = 0x2001;
const quint16 TYPE_STRING_DATA_ITEM = 0x2002;
const quint16 TYPE_DEBUG_INFO_ITEM = 0x2003;
const quint16 TYPE_ANNOTATION_ITEM = 0x2004;
const quint16 TYPE_ENCODED_ARRAY_ITEM = 0x2005;
const quint16 TYPE_ANNOTATIONS_DIRECTORY_ITEM = 0x2006;
const quint16 TYPE_HIDDENAPI_CLASS_DATA_ITEM = 0xF000;
}  // namespace XDEX_DEF
#endif  // XDEX_DEF_H
