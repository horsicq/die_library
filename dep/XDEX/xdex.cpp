/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#include "xdex.h"

XDEX::XDEX(QIODevice *pDevice) : XBinary(pDevice)
{
}

XBinary::MODE XDEX::getMode(QIODevice *pDevice)
{
    XDEX xdex(pDevice);

    return xdex.getMode();
}

bool XDEX::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    // TODO More checks(sizes,mb hashes)

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);
    bIsValid = compareSignature(&memoryMap, "'dex\n'......00", 0, pPdStruct);

    return bIsValid;
}

quint32 XDEX::_getVersion()
{
    quint32 nVersion = 0;

    QString sVersion = read_ansiString(4);

    nVersion = sVersion.toUInt();

    return nVersion;
}

QString XDEX::getVersion()
{
    return read_ansiString(4);  // mb Check
}

XBinary::ENDIAN XDEX::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint32 nData = read_uint32(offsetof(XDEX_DEF::HEADER, endian_tag));

    if (nData == 0x12345678) {
        result = ENDIAN_LITTLE;
    } else if (nData == 0x78563412) {
        result = ENDIAN_BIG;
    }

    return result;
}

XBinary::MODE XDEX::getMode()
{
    return MODE_32;
}

QString XDEX::getArch()
{
    return QString("Dalvik");  // TODO Check
}

XBinary::FT XDEX::getFileType()
{
    return FT_DEX;
}

qint32 XDEX::getType()
{
    // TODO more (main module,second module etc)
    return TYPE_MODULE;
}

QString XDEX::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_MODULE:
            sResult = QString("Module");  // mb translate
            break;
    }

    return sResult;
}

QList<XBinary::MAPMODE> XDEX::getMapModesList(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XDEX::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP result = {};

    qint64 nTotalSize = getSize();

    result.nModuleAddress = getModuleAddress();
    result.nBinarySize = nTotalSize;
    result.nImageSize = nTotalSize;
    result.fileType = FT_DEX;
    result.mode = getMode();
    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = getTypeAsString();

    qint32 nIndex = 0;

    XDEX_DEF::HEADER header = getHeader();

    _MEMORY_RECORD recordHeader = {};
    recordHeader.nAddress = -1;
    recordHeader.segment = ADDRESS_SEGMENT_FLAT;
    recordHeader.nOffset = 0;
    recordHeader.nSize = header.header_size;
    recordHeader.nIndex = nIndex++;
    recordHeader.type = MMT_HEADER;
    recordHeader.sName = tr("Header");

    result.listRecords.append(recordHeader);

    if (header.link_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.link_off;
        record.nSize = header.link_size;
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "link";

        result.listRecords.append(record);
    }

    if (header.string_ids_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.string_ids_off;
        record.nSize = (header.string_ids_size) * sizeof(XDEX_DEF::STRING_ITEM_ID);
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "string_ids";

        result.listRecords.append(record);
    }

    if (header.type_ids_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.type_ids_off;
        record.nSize = (header.type_ids_size) * sizeof(XDEX_DEF::TYPE_ITEM_ID);
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "type_ids";

        result.listRecords.append(record);
    }

    if (header.proto_ids_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.proto_ids_off;
        record.nSize = (header.proto_ids_size) * sizeof(XDEX_DEF::PROTO_ITEM_ID);
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "proto_ids";

        result.listRecords.append(record);
    }

    if (header.field_ids_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.field_ids_off;
        record.nSize = (header.field_ids_size) * sizeof(XDEX_DEF::FIELD_ITEM_ID);
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "field_ids";

        result.listRecords.append(record);
    }

    if (header.method_ids_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.method_ids_off;
        record.nSize = (header.method_ids_size) * sizeof(XDEX_DEF::METHOD_ITEM_ID);
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "method_ids";

        result.listRecords.append(record);
    }

    if (header.class_defs_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.class_defs_off;
        record.nSize = (header.class_defs_size) * sizeof(XDEX_DEF::CLASS_ITEM_DEF);
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "class_defs";

        result.listRecords.append(record);
    }

    if (header.data_size) {
        _MEMORY_RECORD record = {};
        record.nAddress = -1;
        record.segment = ADDRESS_SEGMENT_FLAT;
        record.nOffset = header.data_off;
        record.nSize = header.data_size;
        record.nIndex = nIndex++;
        record.type = MMT_FILESEGMENT;
        record.sName = "data";

        result.listRecords.append(record);
    }

    if ((header.data_off + header.data_size) < header.file_size) {
        _MEMORY_RECORD recordOverlay = {};
        recordOverlay.nAddress = -1;
        recordOverlay.segment = ADDRESS_SEGMENT_FLAT;
        recordOverlay.nOffset = (header.data_off + header.data_size);
        recordOverlay.nSize = nTotalSize - (header.data_off + header.data_size);
        recordOverlay.nIndex = nIndex++;
        recordOverlay.type = MMT_OVERLAY;
        recordOverlay.sName = tr("Overlay");

        result.listRecords.append(recordOverlay);
    }

    return result;
}

qint64 XDEX::getFileFormatSize()
{
    qint64 nResult = 0;

    nResult = getHeader_file_size();  // TODO check mn _getRawSize

    return nResult;
}

XBinary::OSINFO XDEX::getOsInfo()
{
    OSINFO result = {};

    result.osName = OSNAME_ANDROID;
    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    QString sDDEXVersion = getVersion();

    // https://source.android.com/devices/tech/dalvik/dex-format
    if (sDDEXVersion == "035") {
        result.sOsVersion = XBinary::getAndroidVersionFromApi(14);
    }
    //        else if (sDDEXVersion=="036")
    //        {
    //            // Due to a Dalvik bug present in older versions of Android, Dex version 036 has been skipped.
    //            // Dex version 036 is not valid for any version of Android and never will be.
    //        }
    else if (sDDEXVersion == "037") {
        result.sOsVersion = XBinary::getAndroidVersionFromApi(24);
    } else if (sDDEXVersion == "038") {
        result.sOsVersion = XBinary::getAndroidVersionFromApi(26);
    } else if (sDDEXVersion == "039") {
        result.sOsVersion = XBinary::getAndroidVersionFromApi(28);
    } else {
        result.sOsVersion = sDDEXVersion;
    }

    return result;
}

quint32 XDEX::getHeader_magic()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, magic), false);
}

quint32 XDEX::getHeader_version()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, version), false);
}

quint32 XDEX::getHeader_checksum()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, checksum), isBigEndian());
}

QByteArray XDEX::getHeader_signature()
{
    return read_array(offsetof(XDEX_DEF::HEADER, signature), 20);
}

quint32 XDEX::getHeader_file_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, file_size), isBigEndian());
}

quint32 XDEX::getHeader_header_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, header_size), isBigEndian());
}

quint32 XDEX::getHeader_endian_tag()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, endian_tag));
}

quint32 XDEX::getHeader_link_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, link_size), isBigEndian());
}

quint32 XDEX::getHeader_link_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, link_off), isBigEndian());
}

quint32 XDEX::getHeader_map_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, map_off), isBigEndian());
}

quint32 XDEX::getHeader_string_ids_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, string_ids_size), isBigEndian());
}

quint32 XDEX::getHeader_string_ids_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, string_ids_off), isBigEndian());
}

quint32 XDEX::getHeader_type_ids_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, type_ids_size), isBigEndian());
}

quint32 XDEX::getHeader_type_ids_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, type_ids_off), isBigEndian());
}

quint32 XDEX::getHeader_proto_ids_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, proto_ids_size), isBigEndian());
}

quint32 XDEX::getHeader_proto_ids_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, proto_ids_off), isBigEndian());
}

quint32 XDEX::getHeader_field_ids_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, field_ids_size), isBigEndian());
}

quint32 XDEX::getHeader_field_ids_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, field_ids_off), isBigEndian());
}

quint32 XDEX::getHeader_method_ids_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, method_ids_size), isBigEndian());
}

quint32 XDEX::getHeader_method_ids_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, method_ids_off), isBigEndian());
}

quint32 XDEX::getHeader_class_defs_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, class_defs_size), isBigEndian());
}

quint32 XDEX::getHeader_class_defs_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, class_defs_off), isBigEndian());
}

quint32 XDEX::getHeader_data_size()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, data_size), isBigEndian());
}

quint32 XDEX::getHeader_data_off()
{
    return read_uint32(offsetof(XDEX_DEF::HEADER, data_off), isBigEndian());
}

void XDEX::setHeader_magic(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, magic), value, false);
}

void XDEX::setHeader_version(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, version), value, false);
}

void XDEX::setHeader_checksum(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, checksum), value, isBigEndian());
}

void XDEX::setHeader_file_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, file_size), value, isBigEndian());
}

void XDEX::setHeader_header_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, header_size), value, isBigEndian());
}

void XDEX::setHeader_endian_tag(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, endian_tag), value);
}

void XDEX::setHeader_link_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, link_size), value, isBigEndian());
}

void XDEX::setHeader_link_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, link_off), value, isBigEndian());
}

void XDEX::setHeader_map_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, map_off), value, isBigEndian());
}

void XDEX::setHeader_string_ids_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, string_ids_size), value, isBigEndian());
}

void XDEX::setHeader_string_ids_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, string_ids_off), value, isBigEndian());
}

void XDEX::setHeader_type_ids_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, type_ids_size), value, isBigEndian());
}

void XDEX::setHeader_type_ids_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, type_ids_off), value, isBigEndian());
}

void XDEX::setHeader_proto_ids_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, proto_ids_size), value, isBigEndian());
}

void XDEX::setHeader_proto_ids_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, proto_ids_off), value, isBigEndian());
}

void XDEX::setHeader_field_ids_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, field_ids_size), value, isBigEndian());
}

void XDEX::setHeader_field_ids_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, field_ids_off), value, isBigEndian());
}

void XDEX::setHeader_method_ids_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, method_ids_size), value, isBigEndian());
}

void XDEX::setHeader_method_ids_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, method_ids_off), value, isBigEndian());
}

void XDEX::setHeader_class_defs_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, class_defs_size), value, isBigEndian());
}

void XDEX::setHeader_class_defs_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, class_defs_off), value, isBigEndian());
}

void XDEX::setHeader_data_size(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, data_size), value, isBigEndian());
}

void XDEX::setHeader_data_off(quint32 value)
{
    write_uint32(offsetof(XDEX_DEF::HEADER, data_off), value, isBigEndian());
}

XDEX_DEF::HEADER XDEX::getHeader()
{
    XDEX_DEF::HEADER result = {};

    bool bIsBigEndian = isBigEndian();

    result.magic = read_uint32(offsetof(XDEX_DEF::HEADER, magic), false);
    result.version = read_uint32(offsetof(XDEX_DEF::HEADER, version), false);
    result.checksum = read_uint32(offsetof(XDEX_DEF::HEADER, checksum), bIsBigEndian);
    //    result.signature=getHeader_signature();
    result.file_size = read_uint32(offsetof(XDEX_DEF::HEADER, file_size), bIsBigEndian);
    result.header_size = read_uint32(offsetof(XDEX_DEF::HEADER, header_size), bIsBigEndian);
    result.endian_tag = read_uint32(offsetof(XDEX_DEF::HEADER, endian_tag), false);
    result.link_size = read_uint32(offsetof(XDEX_DEF::HEADER, link_size), bIsBigEndian);
    result.link_off = read_uint32(offsetof(XDEX_DEF::HEADER, link_off), bIsBigEndian);
    result.map_off = read_uint32(offsetof(XDEX_DEF::HEADER, map_off), bIsBigEndian);
    result.string_ids_size = read_uint32(offsetof(XDEX_DEF::HEADER, string_ids_size), bIsBigEndian);
    result.string_ids_off = read_uint32(offsetof(XDEX_DEF::HEADER, string_ids_off), bIsBigEndian);
    result.type_ids_size = read_uint32(offsetof(XDEX_DEF::HEADER, type_ids_size), bIsBigEndian);
    result.type_ids_off = read_uint32(offsetof(XDEX_DEF::HEADER, type_ids_off), bIsBigEndian);
    result.proto_ids_size = read_uint32(offsetof(XDEX_DEF::HEADER, proto_ids_size), bIsBigEndian);
    result.proto_ids_off = read_uint32(offsetof(XDEX_DEF::HEADER, proto_ids_off), bIsBigEndian);
    result.field_ids_size = read_uint32(offsetof(XDEX_DEF::HEADER, field_ids_size), bIsBigEndian);
    result.field_ids_off = read_uint32(offsetof(XDEX_DEF::HEADER, field_ids_off), bIsBigEndian);
    result.method_ids_size = read_uint32(offsetof(XDEX_DEF::HEADER, method_ids_size), bIsBigEndian);
    result.method_ids_off = read_uint32(offsetof(XDEX_DEF::HEADER, method_ids_off), bIsBigEndian);
    result.class_defs_size = read_uint32(offsetof(XDEX_DEF::HEADER, class_defs_size), bIsBigEndian);
    result.class_defs_off = read_uint32(offsetof(XDEX_DEF::HEADER, class_defs_off), bIsBigEndian);
    result.data_size = read_uint32(offsetof(XDEX_DEF::HEADER, data_size), bIsBigEndian);
    result.data_off = read_uint32(offsetof(XDEX_DEF::HEADER, data_off), bIsBigEndian);

    return result;
}

quint32 XDEX::getHeaderSize()
{
    return sizeof(XDEX_DEF::HEADER);
}

QList<XDEX_DEF::MAP_ITEM> XDEX::getMapItems()
{
    QList<XDEX_DEF::MAP_ITEM> listResult;

    qint64 nOffset = getHeader_map_off();

    bool bIsBigEndian = isBigEndian();

    quint32 nNumberOfItems = read_uint32(nOffset, bIsBigEndian);

    nOffset += 4;

    if (nNumberOfItems < 0x100) {
        for (quint32 i = 0; i < nNumberOfItems; i++) {
            XDEX_DEF::MAP_ITEM map_item = {};

            map_item.nType = read_uint16(nOffset, bIsBigEndian);
            map_item.nCount = read_uint32(nOffset + 4, bIsBigEndian);
            map_item.nOffset = read_uint32(nOffset + 8, bIsBigEndian);

            listResult.append(map_item);

            nOffset += 12;
        }
    }

    return listResult;
}

bool XDEX::compareMapItems(QList<XDEX_DEF::MAP_ITEM> *pListMaps, QList<quint16> *pListIDs)
{
    bool bResult = false;

    qint32 nNumberOfMapItems = pListMaps->count();
    qint32 nNumberOfIDs = pListIDs->count();

    qint32 nCurrentMapItem = 0;
    qint32 nCurrentID = 0;

    while ((nCurrentMapItem < nNumberOfMapItems) && (nCurrentID < nNumberOfIDs)) {
        bResult = false;

        if (pListMaps->at(nCurrentMapItem).nType == pListIDs->at(nCurrentID)) {
            bResult = true;
            nCurrentMapItem++;
            nCurrentID++;
        } else {
            nCurrentID++;
        }
    }

    bResult = (bResult) && (nCurrentMapItem == qMin(nNumberOfMapItems, nNumberOfIDs));

    return bResult;
}

quint64 XDEX::getMapItemsHash()
{
    quint64 nResult = 0;

    QList<XDEX_DEF::MAP_ITEM> listMapItems = getMapItems();

    qint32 nNumberOfMapItems = listMapItems.count();

    for (qint32 i = 0; i < nNumberOfMapItems; i++) {
        nResult += (quint64)i * getStringCustomCRC32(QString::number(listMapItems.at(i).nType));
    }

    return nResult;
}

bool XDEX::isMapItemPresent(quint16 nType, QList<XDEX_DEF::MAP_ITEM> *pMapItems)
{
    bool bResult = false;

    qint32 nNumberOfItems = pMapItems->count();

    for (qint32 i = 0; i < nNumberOfItems; i++) {
        if (pMapItems->at(i).nType == nType) {
            bResult = true;

            break;
        }
    }

    return bResult;
}

QMap<quint64, QString> XDEX::getTypes()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0000, "TYPE_HEADER_ITEM");
    mapResult.insert(0x0001, "TYPE_STRING_ID_ITEM");
    mapResult.insert(0x0002, "TYPE_TYPE_ID_ITEM");
    mapResult.insert(0x0003, "TYPE_PROTO_ID_ITEM");
    mapResult.insert(0x0004, "TYPE_FIELD_ID_ITEM");
    mapResult.insert(0x0005, "TYPE_METHOD_ID_ITEM");
    mapResult.insert(0x0006, "TYPE_CLASS_DEF_ITEM");
    mapResult.insert(0x0007, "TYPE_CALL_SITE_ID_ITEM");
    mapResult.insert(0x0008, "TYPE_METHOD_HANDLE_ITEM");
    mapResult.insert(0x1000, "TYPE_MAP_LIST");
    mapResult.insert(0x1001, "TYPE_TYPE_LIST");
    mapResult.insert(0x1002, "TYPE_ANNOTATION_SET_REF_LIST");
    mapResult.insert(0x1003, "TYPE_ANNOTATION_SET_ITEM");
    mapResult.insert(0x2000, "TYPE_CLASS_DATA_ITEM");
    mapResult.insert(0x2001, "TYPE_CODE_ITEM");
    mapResult.insert(0x2002, "TYPE_STRING_DATA_ITEM");
    mapResult.insert(0x2003, "TYPE_DEBUG_INFO_ITEM");
    mapResult.insert(0x2004, "TYPE_ANNOTATION_ITEM");
    mapResult.insert(0x2005, "TYPE_ENCODED_ARRAY_ITEM");
    mapResult.insert(0x2006, "TYPE_ANNOTATIONS_DIRECTORY_ITEM");
    mapResult.insert(0xF000, "TYPE_HIDDENAPI_CLASS_DATA_ITEM");

    return mapResult;
}

QMap<quint64, QString> XDEX::getTypesS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0000, "HEADER_ITEM");
    mapResult.insert(0x0001, "STRING_ID_ITEM");
    mapResult.insert(0x0002, "TYPE_ID_ITEM");
    mapResult.insert(0x0003, "PROTO_ID_ITEM");
    mapResult.insert(0x0004, "FIELD_ID_ITEM");
    mapResult.insert(0x0005, "METHOD_ID_ITEM");
    mapResult.insert(0x0006, "CLASS_DEF_ITEM");
    mapResult.insert(0x0007, "CALL_SITE_ID_ITEM");
    mapResult.insert(0x0008, "METHOD_HANDLE_ITEM");
    mapResult.insert(0x1000, "MAP_LIST");
    mapResult.insert(0x1001, "TYPE_LIST");
    mapResult.insert(0x1002, "ANNOTATION_SET_REF_LIST");
    mapResult.insert(0x1003, "ANNOTATION_SET_ITEM");
    mapResult.insert(0x2000, "CLASS_DATA_ITEM");
    mapResult.insert(0x2001, "CODE_ITEM");
    mapResult.insert(0x2002, "STRING_DATA_ITEM");
    mapResult.insert(0x2003, "DEBUG_INFO_ITEM");
    mapResult.insert(0x2004, "ANNOTATION_ITEM");
    mapResult.insert(0x2005, "ENCODED_ARRAY_ITEM");
    mapResult.insert(0x2006, "ANNOTATIONS_DIRECTORY_ITEM");
    mapResult.insert(0xF000, "HIDDENAPI_CLASS_DATA_ITEM");

    return mapResult;
}

XDEX_DEF::MAP_ITEM XDEX::getMapItem(quint16 nType, QList<XDEX_DEF::MAP_ITEM> *pMapItems)
{
    XDEX_DEF::MAP_ITEM result = {};

    qint32 nCount = pMapItems->count();

    for (qint32 i = 0; i < nCount; i++) {
        if (pMapItems->at(i).nType == nType) {
            result = pMapItems->at(i);

            break;
        }
    }

    return result;
}

QList<XDEX_DEF::STRING_ITEM_ID> XDEX::getList_STRING_ITEM_ID()
{
    QList<XDEX_DEF::MAP_ITEM> listMapItems = getMapItems();

    return getList_STRING_ITEM_ID(&listMapItems);
}

QList<XDEX_DEF::STRING_ITEM_ID> XDEX::getList_STRING_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems)
{
    QList<XDEX_DEF::STRING_ITEM_ID> listResult;

    bool bIsBigEndian = isBigEndian();

    XDEX_DEF::MAP_ITEM mapItem = getMapItem(XDEX_DEF::TYPE_STRING_ID_ITEM, pListMapItems);

    QByteArray baData = read_array(mapItem.nOffset, mapItem.nCount * sizeof(XDEX_DEF::STRING_ITEM_ID));
    char *pData = baData.data();
    qint32 nSize = baData.size() / sizeof(XDEX_DEF::STRING_ITEM_ID);

    for (qint32 i = 0; i < nSize; i++) {
        qint64 nOffset = sizeof(XDEX_DEF::STRING_ITEM_ID) * i;

        XDEX_DEF::STRING_ITEM_ID record = {};

        record.string_data_off = _read_int32(pData + nOffset + offsetof(XDEX_DEF::STRING_ITEM_ID, string_data_off), bIsBigEndian);

        listResult.append(record);
    }

    return listResult;
}

QList<XDEX_DEF::TYPE_ITEM_ID> XDEX::getList_TYPE_ITEM_ID()
{
    QList<XDEX_DEF::MAP_ITEM> listMapItems = getMapItems();

    return getList_TYPE_ITEM_ID(&listMapItems);
}

QList<XDEX_DEF::TYPE_ITEM_ID> XDEX::getList_TYPE_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems)
{
    QList<XDEX_DEF::TYPE_ITEM_ID> listResult;

    XDEX_DEF::MAP_ITEM mapItem = getMapItem(XDEX_DEF::TYPE_TYPE_ID_ITEM, pListMapItems);
    bool bIsBigEndian = isBigEndian();

    QByteArray baData = read_array(mapItem.nOffset, mapItem.nCount * sizeof(XDEX_DEF::TYPE_ITEM_ID));
    char *pData = baData.data();
    qint32 nSize = baData.size() / sizeof(XDEX_DEF::TYPE_ITEM_ID);

    for (qint32 i = 0; i < nSize; i++) {
        qint64 nOffset = sizeof(XDEX_DEF::TYPE_ITEM_ID) * i;

        XDEX_DEF::TYPE_ITEM_ID record = {};

        record.descriptor_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::TYPE_ITEM_ID, descriptor_idx), bIsBigEndian);

        listResult.append(record);
    }

    return listResult;
}

QList<XDEX_DEF::PROTO_ITEM_ID> XDEX::getList_PROTO_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems)
{
    QList<XDEX_DEF::PROTO_ITEM_ID> listResult;

    XDEX_DEF::MAP_ITEM mapItem = getMapItem(XDEX_DEF::TYPE_PROTO_ID_ITEM, pListMapItems);
    bool bIsBigEndian = isBigEndian();

    QByteArray baData = read_array(mapItem.nOffset, mapItem.nCount * sizeof(XDEX_DEF::PROTO_ITEM_ID));
    char *pData = baData.data();
    qint32 nSize = baData.size() / sizeof(XDEX_DEF::PROTO_ITEM_ID);

    for (qint32 i = 0; i < nSize; i++) {
        qint64 nOffset = sizeof(XDEX_DEF::PROTO_ITEM_ID) * i;

        XDEX_DEF::PROTO_ITEM_ID record = {};

        record.shorty_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::PROTO_ITEM_ID, shorty_idx), bIsBigEndian);
        record.return_type_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::PROTO_ITEM_ID, return_type_idx), bIsBigEndian);
        record.parameters_off = _read_int32(pData + nOffset + offsetof(XDEX_DEF::PROTO_ITEM_ID, parameters_off), bIsBigEndian);

        listResult.append(record);
    }

    return listResult;
}

QList<XDEX_DEF::FIELD_ITEM_ID> XDEX::getList_FIELD_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<XDEX_DEF::FIELD_ITEM_ID> listResult;

    XDEX_DEF::MAP_ITEM mapItem = getMapItem(XDEX_DEF::TYPE_FIELD_ID_ITEM, pListMapItems);
    bool bIsBigEndian = isBigEndian();

    QByteArray baData = read_array(mapItem.nOffset, mapItem.nCount * sizeof(XDEX_DEF::FIELD_ITEM_ID));
    char *pData = baData.data();
    qint32 nSize = baData.size() / sizeof(XDEX_DEF::FIELD_ITEM_ID);

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    for (qint32 i = 0; (i < nSize) && (!(pPdStruct->bIsStop)); i++) {
        qint64 nOffset = sizeof(XDEX_DEF::FIELD_ITEM_ID) * i;

        XDEX_DEF::FIELD_ITEM_ID record = {};

        record.class_idx = _read_int16(pData + nOffset + offsetof(XDEX_DEF::FIELD_ITEM_ID, class_idx), bIsBigEndian);
        record.type_idx = _read_int16(pData + nOffset + offsetof(XDEX_DEF::FIELD_ITEM_ID, type_idx), bIsBigEndian);
        record.name_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::FIELD_ITEM_ID, name_idx), bIsBigEndian);

        listResult.append(record);

        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QList<XDEX_DEF::METHOD_ITEM_ID> XDEX::getList_METHOD_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<XDEX_DEF::METHOD_ITEM_ID> listResult;

    XDEX_DEF::MAP_ITEM mapItem = getMapItem(XDEX_DEF::TYPE_METHOD_ID_ITEM, pListMapItems);
    bool bIsBigEndian = isBigEndian();

    QByteArray baData = read_array(mapItem.nOffset, mapItem.nCount * sizeof(XDEX_DEF::METHOD_ITEM_ID));
    char *pData = baData.data();
    qint32 nSize = baData.size() / sizeof(XDEX_DEF::METHOD_ITEM_ID);

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    for (qint32 i = 0; (i < nSize) && (!(pPdStruct->bIsStop)); i++) {
        qint64 nOffset = sizeof(XDEX_DEF::METHOD_ITEM_ID) * i;

        XDEX_DEF::METHOD_ITEM_ID record = {};

        record.class_idx = _read_int16(pData + nOffset + offsetof(XDEX_DEF::METHOD_ITEM_ID, class_idx), bIsBigEndian);
        record.proto_idx = _read_int16(pData + nOffset + offsetof(XDEX_DEF::METHOD_ITEM_ID, proto_idx), bIsBigEndian);
        record.name_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::METHOD_ITEM_ID, name_idx), bIsBigEndian);

        listResult.append(record);

        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QList<XDEX_DEF::CLASS_ITEM_DEF> XDEX::getList_CLASS_ITEM_DEF(QList<XDEX_DEF::MAP_ITEM> *pListMapItems)
{
    QList<XDEX_DEF::CLASS_ITEM_DEF> listResult;

    XDEX_DEF::MAP_ITEM mapItem = getMapItem(XDEX_DEF::TYPE_CLASS_DEF_ITEM, pListMapItems);
    bool bIsBigEndian = isBigEndian();

    QByteArray baData = read_array(mapItem.nOffset, mapItem.nCount * sizeof(XDEX_DEF::CLASS_ITEM_DEF));
    char *pData = baData.data();
    qint32 nSize = baData.size() / sizeof(XDEX_DEF::CLASS_ITEM_DEF);

    for (qint32 i = 0; i < nSize; i++) {
        qint64 nOffset = sizeof(XDEX_DEF::CLASS_ITEM_DEF) * i;

        XDEX_DEF::CLASS_ITEM_DEF record = {};

        record.class_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, class_idx), bIsBigEndian);
        record.access_flags = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, access_flags), bIsBigEndian);
        record.superclass_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, superclass_idx), bIsBigEndian);
        record.interfaces_off = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, interfaces_off), bIsBigEndian);
        record.source_file_idx = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, source_file_idx), bIsBigEndian);
        record.annotations_off = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, annotations_off), bIsBigEndian);
        record.class_data_off = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, class_data_off), bIsBigEndian);
        record.static_values_off = _read_int32(pData + nOffset + offsetof(XDEX_DEF::CLASS_ITEM_DEF, static_values_off), bIsBigEndian);

        listResult.append(record);
    }

    return listResult;
}

QList<QString> XDEX::getStrings(QList<XDEX_DEF::MAP_ITEM> *pMapItems, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<QString> listResult;

    bool bIsBigEndian = isBigEndian();

    XDEX_DEF::MAP_ITEM map_strings = getMapItem(XDEX_DEF::TYPE_STRING_ID_ITEM, pMapItems);

    QByteArray baData = read_array(getHeader_data_off(), getHeader_data_size());

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, map_strings.nCount);

    for (quint32 i = 0; (i < map_strings.nCount) && (!(pPdStruct->bIsStop)); i++) {
        QString sString = _getString(map_strings, i, bIsBigEndian, baData.data(), baData.size(), getHeader_data_off());

        listResult.append(sString);
        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QString XDEX::_getString(XDEX_DEF::MAP_ITEM map_stringIdItem, quint32 nIndex, bool bIsBigEndian)
{
    QString sResult;

    if (nIndex < map_stringIdItem.nCount) {
        qint64 nOffset = map_stringIdItem.nOffset + sizeof(quint32) * nIndex;

        quint32 nStringsOffset = read_uint32(nOffset, bIsBigEndian);

        sResult = XBinary::_read_utf8String(nStringsOffset);
    }

    return sResult;
}

QString XDEX::_getString(XDEX_DEF::MAP_ITEM map_stringIdItem, quint32 nIndex, bool bIsBigEndian, char *pData, qint32 nDataSize, qint32 nDataOffset)
{
    QString sResult;

    if (nIndex < map_stringIdItem.nCount) {
        qint64 nOffset = map_stringIdItem.nOffset + sizeof(quint32) * nIndex;

        qint32 nStringsOffset = (qint32)read_uint32(nOffset, bIsBigEndian);

        sResult = XBinary::_read_utf8String(nStringsOffset, pData, nDataSize, nDataOffset);
    }

    return sResult;
}

QString XDEX::_getTypeItemtString(XDEX_DEF::MAP_ITEM map_stringIdItem, XDEX_DEF::MAP_ITEM map_typeItemId, quint32 nIndex, bool bIsBigEndian)
{
    QString sResult;

    if (nIndex < map_typeItemId.nCount) {
        quint32 nID = read_uint32(map_typeItemId.nOffset + sizeof(quint32) * nIndex, bIsBigEndian);

        sResult = _getString(map_stringIdItem, nID, bIsBigEndian);
    }

    return sResult;
}

QList<quint32> XDEX::_getTypeList(qint64 nOffset, bool bIsBigEndian)
{
    QList<quint32> listResult;

    if (nOffset) {
        quint32 nCount = read_uint32(nOffset, bIsBigEndian);

        for (quint32 i = 0; i < nCount; i++) {
            quint32 nType = read_uint32(nOffset + sizeof(quint32) * (1 + i), bIsBigEndian);
            listResult.append(nType);
        }
    }

    return listResult;
}

QList<QString> XDEX::getTypeItemStrings(QList<XDEX_DEF::MAP_ITEM> *pMapItems, QList<QString> *pListStrings, PDSTRUCT *pPdStruct)
{
    QList<QString> listResult;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    bool bIsBigEndian = isBigEndian();

    qint32 nStringsCount = pListStrings->count();

    XDEX_DEF::MAP_ITEM map_items = getMapItem(XDEX_DEF::TYPE_TYPE_ID_ITEM, pMapItems);

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, map_items.nCount);

    for (quint32 i = 0; (i < map_items.nCount) && (!(pPdStruct->bIsStop)); i++) {
        quint32 nOffset = map_items.nOffset + sizeof(quint32) * i;

        quint32 nItem = read_uint32(nOffset, bIsBigEndian);

        if (((qint32)nItem > 0) && ((qint32)nItem < nStringsCount)) {
            QString sString = pListStrings->at(nItem);

            listResult.append(sString);
        }
        //        else
        //        {
        //            qDebug("Error");
        //        }

        XBinary::setPdStructCurrentIncrement(pPdStruct, _nFreeIndex);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

void XDEX::getProtoIdItems(QList<XDEX_DEF::MAP_ITEM> *pMapItems)
{
    bool bIsBigEndian = isBigEndian();

    XDEX_DEF::MAP_ITEM map_protoIdItem = getMapItem(XDEX_DEF::TYPE_PROTO_ID_ITEM, pMapItems);
    XDEX_DEF::MAP_ITEM map_typeIdItem = getMapItem(XDEX_DEF::TYPE_TYPE_ID_ITEM, pMapItems);
    XDEX_DEF::MAP_ITEM map_stringIdItem = getMapItem(XDEX_DEF::TYPE_STRING_ID_ITEM, pMapItems);

    for (quint32 i = 0; i < map_protoIdItem.nCount; i++) {
        quint32 nOffset = map_protoIdItem.nOffset + sizeof(XDEX_DEF::PROTO_ITEM_ID) * i;

        XDEX_DEF::PROTO_ITEM_ID record = {};

        record.shorty_idx = read_uint32(nOffset + offsetof(XDEX_DEF::PROTO_ITEM_ID, shorty_idx), bIsBigEndian);
        record.return_type_idx = read_uint32(nOffset + offsetof(XDEX_DEF::PROTO_ITEM_ID, return_type_idx), bIsBigEndian);
        record.parameters_off = read_uint32(nOffset + offsetof(XDEX_DEF::PROTO_ITEM_ID, parameters_off), bIsBigEndian);

        QString sProto = _getString(map_stringIdItem, record.shorty_idx, bIsBigEndian);
        QString sRet = _getTypeItemtString(map_stringIdItem, map_typeIdItem, record.return_type_idx, bIsBigEndian);

        QList<quint32> listParams = _getTypeList(record.parameters_off, bIsBigEndian);

        //        QString sDebugString=QString("%1 %2").arg(sRet,sProto);

        //        qDebug("%s",sDebugString.toLatin1().data());
    }
}

QString XDEX::getStringItemIdString(XDEX_DEF::STRING_ITEM_ID stringItemId)
{
    QString sResult;

    sResult = _read_utf8String(stringItemId.string_data_off);

    return sResult;
}

QString XDEX::getStringItemIdString(XDEX_DEF::STRING_ITEM_ID stringItemId, char *pData, qint32 nDataSize, qint32 nDataOffset)
{
    QString sResult;

    sResult = XBinary::_read_utf8String(stringItemId.string_data_off, pData, nDataSize, nDataOffset);

    return sResult;
}

QString XDEX::getStringItemIdString(QList<XDEX_DEF::STRING_ITEM_ID> *pList, qint32 nIndex, char *pData, qint32 nDataSize, qint32 nDataOffset)
{
    QString sResult;

    if ((nIndex > 0) && (nIndex < pList->count())) {
        sResult = getStringItemIdString(pList->at(nIndex), pData, nDataSize, nDataOffset);
    }

    return sResult;
}

QString XDEX::getTypeItemIdString(XDEX_DEF::TYPE_ITEM_ID typeItemId, XDEX_DEF::MAP_ITEM *pMapItemStrings)
{
    QString sResult;

    bool bIsBigEndian = isBigEndian();

    sResult = _read_utf8String(read_uint32(pMapItemStrings->nOffset + sizeof(quint32) * typeItemId.descriptor_idx, bIsBigEndian));

    return sResult;
}

QString XDEX::getTypeItemIdString(XDEX_DEF::TYPE_ITEM_ID typeItemId, XDEX_DEF::MAP_ITEM *pMapItemStrings, char *pData, qint32 nDataSize, qint32 nDataOffset)
{
    QString sResult;

    bool bIsBigEndian = isBigEndian();

    sResult = XBinary::_read_utf8String(read_uint32(pMapItemStrings->nOffset + sizeof(quint32) * typeItemId.descriptor_idx, bIsBigEndian), pData, nDataSize, nDataOffset);

    return sResult;
}

QString XDEX::getTypeItemIdString(QList<XDEX_DEF::TYPE_ITEM_ID> *pList, qint32 nIndex, XDEX_DEF::MAP_ITEM *pMapItemStrings, char *pData, qint32 nDataSize,
                                  qint32 nDataOffset)
{
    QString sResult;

    if ((nIndex > 0) && (nIndex < pList->count())) {
        sResult = getTypeItemIdString(pList->at(nIndex), pMapItemStrings, pData, nDataSize, nDataOffset);
    }

    return sResult;
}

QString XDEX::getProtoItemIdString(XDEX_DEF::PROTO_ITEM_ID protoItemId, XDEX_DEF::MAP_ITEM *pMapItemStrings)
{
    QString sResult;

    bool bIsBigEndian = isBigEndian();

    QString sPrototype = _read_utf8String(read_uint32(pMapItemStrings->nOffset + sizeof(quint32) * protoItemId.shorty_idx, bIsBigEndian));
    QString sReturn = _read_utf8String(read_uint32(pMapItemStrings->nOffset + sizeof(quint32) * protoItemId.return_type_idx, bIsBigEndian));
    sResult = QString("%1 %2()").arg(sReturn, sPrototype);

    return sResult;
}

QMap<quint64, QString> XDEX::getHeaderMagics()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x0A786564, "Magic");

    return mapResult;
}

QMap<quint64, QString> XDEX::getHeaderVersions()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00353330, "035");
    // 036 invalid
    mapResult.insert(0x00373330, "037");
    mapResult.insert(0x00383330, "038");
    mapResult.insert(0x00393330, "039");

    return mapResult;
}

QMap<quint64, QString> XDEX::getHeaderEndianTags()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x12345678, "Little endian");
    mapResult.insert(0x78563412, "Big endian");

    return mapResult;
}

bool XDEX::isStringPoolSorted(QList<XDEX_DEF::MAP_ITEM> *pMapItems)
{
    bool bResult = true;

    bool bIsBigEndian = isBigEndian();

    XDEX_DEF::MAP_ITEM map_strings = getMapItem(XDEX_DEF::TYPE_STRING_ID_ITEM, pMapItems);

    qint32 nPrevStringOffset = 0;

    for (quint32 i = 0; i < map_strings.nCount; i++) {
        qint64 nOffset = map_strings.nOffset + sizeof(quint32) * i;

        qint32 nStringOffset = (qint32)read_uint32(nOffset, bIsBigEndian);

        if (nStringOffset < nPrevStringOffset) {
            bResult = false;

            break;
        }

        nPrevStringOffset = nStringOffset;
    }

    return bResult;
}

bool XDEX::isFieldNamesUnicode(QList<XDEX_DEF::FIELD_ITEM_ID> *pListIDs, QList<QString> *pListStrings)
{
    bool bResult = false;

    qint32 nNumberOfIds = pListIDs->count();
    qint32 nNumberOfStrings = pListStrings->count();

    for (qint32 i = 0; i < nNumberOfIds; i++) {
        QString sString = getStringByIndex(pListStrings, pListIDs->at(i).name_idx, nNumberOfStrings);

        if (XBinary::isStringUnicode(sString)) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

bool XDEX::isMethodNamesUnicode(QList<XDEX_DEF::METHOD_ITEM_ID> *pListIDs, QList<QString> *pListStrings)
{
    bool bResult = false;

    qint32 nNumberOfIds = pListIDs->count();
    qint32 nNumberOfStrings = pListStrings->count();

    for (qint32 i = 0; i < nNumberOfIds; i++) {
        QString sString = getStringByIndex(pListStrings, pListIDs->at(i).name_idx, nNumberOfStrings);

        if (XBinary::isStringUnicode(sString)) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

QString XDEX::getFileFormatExt()
{
    return "dex";
}

QString XDEX::getFileFormatString()
{
    QString sResult;

    sResult = QString("DEX(%1)").arg(getVersion());
    // TODO more info

    return sResult;
}

bool XDEX::isStringPoolSorted()
{
    QList<XDEX_DEF::MAP_ITEM> mapItems = getMapItems();

    return isStringPoolSorted(&mapItems);
}
