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
#ifndef XDEX_H
#define XDEX_H

#include "xbinary.h"
#include "xdex_def.h"

class XDEX : public XBinary {
    Q_OBJECT

public:
    enum TYPE {
        TYPE_UNKNOWN = 0,
        TYPE_MODULE
        // TODO more
        // TODO main module !!! TODO Check
        // TODO the second module ...
    };

    XDEX(QIODevice *pDevice);

    static MODE getMode(QIODevice *pDevice);
    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    quint32 _getVersion();
    virtual QString getVersion();
    virtual ENDIAN getEndian();
    virtual MODE getMode();
    virtual QString getArch();
    virtual FT getFileType();
    virtual qint32 getType();
    virtual QString typeIdToString(qint32 nType);

    virtual QList<MAPMODE> getMapModesList(PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    virtual qint64 getFileFormatSize();

    virtual OSINFO getOsInfo();

    quint32 getHeader_magic();
    quint32 getHeader_version();
    quint32 getHeader_checksum();
    QByteArray getHeader_signature();
    quint32 getHeader_file_size();
    quint32 getHeader_header_size();
    quint32 getHeader_endian_tag();
    quint32 getHeader_link_size();
    quint32 getHeader_link_off();
    quint32 getHeader_map_off();
    quint32 getHeader_string_ids_size();
    quint32 getHeader_string_ids_off();
    quint32 getHeader_type_ids_size();
    quint32 getHeader_type_ids_off();
    quint32 getHeader_proto_ids_size();
    quint32 getHeader_proto_ids_off();
    quint32 getHeader_field_ids_size();
    quint32 getHeader_field_ids_off();
    quint32 getHeader_method_ids_size();
    quint32 getHeader_method_ids_off();
    quint32 getHeader_class_defs_size();
    quint32 getHeader_class_defs_off();
    quint32 getHeader_data_size();
    quint32 getHeader_data_off();

    void setHeader_magic(quint32 value);
    void setHeader_version(quint32 value);
    void setHeader_checksum(quint32 value);
    void setHeader_file_size(quint32 value);
    void setHeader_header_size(quint32 value);
    void setHeader_endian_tag(quint32 value);
    void setHeader_link_size(quint32 value);
    void setHeader_link_off(quint32 value);
    void setHeader_map_off(quint32 value);
    void setHeader_string_ids_size(quint32 value);
    void setHeader_string_ids_off(quint32 value);
    void setHeader_type_ids_size(quint32 value);
    void setHeader_type_ids_off(quint32 value);
    void setHeader_proto_ids_size(quint32 value);
    void setHeader_proto_ids_off(quint32 value);
    void setHeader_field_ids_size(quint32 value);
    void setHeader_field_ids_off(quint32 value);
    void setHeader_method_ids_size(quint32 value);
    void setHeader_method_ids_off(quint32 value);
    void setHeader_class_defs_size(quint32 value);
    void setHeader_class_defs_off(quint32 value);
    void setHeader_data_size(quint32 value);
    void setHeader_data_off(quint32 value);

    XDEX_DEF::HEADER getHeader();
    quint32 getHeaderSize();
    QList<XDEX_DEF::MAP_ITEM> getMapItems();

    static bool compareMapItems(QList<XDEX_DEF::MAP_ITEM> *pListMaps, QList<quint16> *pListIDs);
    quint64 getMapItemsHash();
    static bool isMapItemPresent(quint16 nType, QList<XDEX_DEF::MAP_ITEM> *pMapItems);

    static QMap<quint64, QString> getTypes();
    static QMap<quint64, QString> getTypesS();
    static XDEX_DEF::MAP_ITEM getMapItem(quint16 nType, QList<XDEX_DEF::MAP_ITEM> *pMapItems);

    QList<XDEX_DEF::STRING_ITEM_ID> getList_STRING_ITEM_ID();
    QList<XDEX_DEF::STRING_ITEM_ID> getList_STRING_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems);
    QList<XDEX_DEF::TYPE_ITEM_ID> getList_TYPE_ITEM_ID();
    QList<XDEX_DEF::TYPE_ITEM_ID> getList_TYPE_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems);
    QList<XDEX_DEF::PROTO_ITEM_ID> getList_PROTO_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems);
    QList<XDEX_DEF::FIELD_ITEM_ID> getList_FIELD_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems, PDSTRUCT *pPdStruct = nullptr);
    QList<XDEX_DEF::METHOD_ITEM_ID> getList_METHOD_ITEM_ID(QList<XDEX_DEF::MAP_ITEM> *pListMapItems, PDSTRUCT *pPdStruct = nullptr);
    QList<XDEX_DEF::CLASS_ITEM_DEF> getList_CLASS_ITEM_DEF(QList<XDEX_DEF::MAP_ITEM> *pListMapItems);

    QList<QString> getStrings(QList<XDEX_DEF::MAP_ITEM> *pMapItems, PDSTRUCT *pPdStruct = nullptr);
    QString _getString(XDEX_DEF::MAP_ITEM map_stringIdItem, quint32 nIndex, bool bIsBigEndian);
    QString _getString(XDEX_DEF::MAP_ITEM map_stringIdItem, quint32 nIndex, bool bIsBigEndian, char *pData, qint32 nDataSize, qint32 nDataOffset);
    QString _getTypeItemtString(XDEX_DEF::MAP_ITEM map_stringIdItem, XDEX_DEF::MAP_ITEM map_typeItemId, quint32 nIndex, bool bIsBigEndian);
    QList<quint32> _getTypeList(qint64 nOffset, bool bIsBigEndian);
    QList<QString> getTypeItemStrings(QList<XDEX_DEF::MAP_ITEM> *pMapItems, QList<QString> *pListStrings, PDSTRUCT *pPdStruct = nullptr);
    void getProtoIdItems(QList<XDEX_DEF::MAP_ITEM> *pMapItems);
    QString getStringItemIdString(XDEX_DEF::STRING_ITEM_ID stringItemId);
    QString getStringItemIdString(XDEX_DEF::STRING_ITEM_ID stringItemId, char *pData, qint32 nDataSize, qint32 nDataOffset);
    QString getStringItemIdString(QList<XDEX_DEF::STRING_ITEM_ID> *pList, qint32 nIndex, char *pData, qint32 nDataSize, qint32 nDataOffset);
    QString getTypeItemIdString(XDEX_DEF::TYPE_ITEM_ID typeItemId, XDEX_DEF::MAP_ITEM *pMapItemStrings);
    QString getTypeItemIdString(XDEX_DEF::TYPE_ITEM_ID typeItemId, XDEX_DEF::MAP_ITEM *pMapItemStrings, char *pData, qint32 nDataSize, qint32 nDataOffset);
    QString getTypeItemIdString(QList<XDEX_DEF::TYPE_ITEM_ID> *pList, qint32 nIndex, XDEX_DEF::MAP_ITEM *pMapItemStrings, char *pData, qint32 nDataSize,
                                qint32 nDataOffset);
    QString getProtoItemIdString(XDEX_DEF::PROTO_ITEM_ID protoItemId,
                                 XDEX_DEF::MAP_ITEM *pMapItemStrings);  // TODO

    static QMap<quint64, QString> getHeaderMagics();
    static QMap<quint64, QString> getHeaderVersions();
    static QMap<quint64, QString> getHeaderEndianTags();

    bool isStringPoolSorted();
    bool isStringPoolSorted(QList<XDEX_DEF::MAP_ITEM> *pMapItems);
    bool isFieldNamesUnicode(QList<XDEX_DEF::FIELD_ITEM_ID> *pListIDs, QList<QString> *pListStrings);
    bool isMethodNamesUnicode(QList<XDEX_DEF::METHOD_ITEM_ID> *pListIDs, QList<QString> *pListStrings);

    virtual QString getFileFormatExt();
    virtual QString getFileFormatString();
};

#endif  // XDEX_H
