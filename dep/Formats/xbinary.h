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
#ifndef XBINARY_H
#define XBINARY_H

#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
#include <QDirIterator>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QMap>
#include <QMutex>
#include <QSet>
#include <QTemporaryFile>
#include <QTextStream>
#include <QUuid>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QtEndian>
#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
#include <QTextCodec>  // Qt5 Compat
#endif
#if (QT_VERSION_MAJOR < 5)  // TODO Check
#include <QRegExp>
#else
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#endif
#ifdef QT_DEBUG
#include <QDebug>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#elif (QT_VERSION_MAJOR >= 6)  // TODO Check
#include <QRandomGenerator>
#endif

#ifdef QT_GUI_LIB
#include <QColor>
#endif

#include <math.h>

#include "subdevice.h"
#include "xbinary_def.h"
#include "xelf_def.h"
#include "xle_def.h"
#include "xmach_def.h"
#include "xmsdos_def.h"
#include "xne_def.h"
#include "xpe_def.h"

// TODO mb Functions
#define S_ALIGN_DOWN32(value, align) (((quint32)value) & ~((quint32)align - 1))
#define S_ALIGN_UP32(value, align) ((((quint32)value) & ((quint32)align - 1)) ? (S_ALIGN_DOWN32((quint32)value, (quint32)align) + (quint32)align) : ((quint32)value))

#define S_ALIGN_DOWN64(value, align) (((quint64)value) & ~((quint64)align - 1))
#define S_ALIGN_UP64(value, align) ((((quint64)value) & ((quint64)align - 1)) ? (S_ALIGN_DOWN64((quint64)value, (quint64)align) + (quint64)align) : ((quint64)value))

#define S_ALIGN_DOWN(value, align) ((value) & ~(align - 1))
#define S_ALIGN_UP(value, align) (((value) & (align - 1)) ? S_ALIGN_DOWN(value, align) + align : value)

#define S_LOWORD(value) ((quint16)((quint32)(value)&0xFFFF))
#define S_HIWORD(value) ((quint16)((quint32)(value) >> 16))
#define S_FULL_VERSION(value1, value2, value3) ((quint32)((((quint16)value1) << 16) | (((quint8)value2) << 8) | ((quint8)value3)))

typedef quint64 XADDR;
// #define XADDR_ERROR (XADDR)-1

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>  // Check
#endif

class XBinary : public QObject {
    Q_OBJECT

    static const double D_ENTROPY_THRESHOLD;  // 6.5 TODO set get

public:
    struct DATASET {
        qint64 nOffset;
        XADDR nAddress;
        qint64 nSize;
        QString sName;
        quint32 nType;
        // Optional
        qint64 nStringTableOffset;
        qint64 nStringTableSize;
    };

    struct BYTE_COUNTS {
        qint64 nSize;
        qint64 nCount[256];  // TODO const
    };

    struct OS_STRING {
        qint64 nOffset;
        qint64 nSize;
        QString sString;
    };

    struct OFFSETSIZE {
        qint64 nOffset;
        qint64 nSize;
    };

    struct ADDRESSSIZE {
        XADDR nAddress;
        qint64 nSize;
    };

    struct RELADDRESSSIZE {
        qint64 nAddress;
        qint64 nSize;
    };

    enum ADDRESS_SEGMENT {
        ADDRESS_SEGMENT_UNKNOWN = -1,
        ADDRESS_SEGMENT_FLAT = 0,
        ADDRESS_SEGMENT_CODE,
        //        ADDRESS_SEGMENT_DATA
    };

    enum FILEPART {
        FILEPART_UNKNOWN = 0,
        FILEPART_ARCHIVERECORD,
        FILEPART_HEADER,
        FILEPART_OVERLAY,
        FILEPART_RESOURCE
    };

    enum MMT {
        MMT_UNKNOWN = 0,
        MMT_HEADER,
        MMT_FOOTER,
        MMT_LOADSEGMENT,  // Section in PE; LoadProgram in ELF; Segments in MACH
        MMT_NOLOADABLE,   // For ELF TODO Check
        MMT_FILESEGMENT,
        MMT_OVERLAY,
        MMT_DATA,
        MMT_OBJECT,
        MMT_TABLE
    };

    struct _MEMORY_RECORD {
        qint64 nOffset;
        XADDR nAddress;
        ADDRESS_SEGMENT segment;
        qint64 nSize;
        MMT type;
        qint32 nLoadSectionNumber;
        QString sName;
        qint32 nIndex;
        bool bIsVirtual;
        quint64 nID;
    };

    enum FORMATTYPE {
        FORMATTYPE_TEXT = 0,
        FORMATTYPE_XML,
        FORMATTYPE_JSON,
        FORMATTYPE_CSV,
        FORMATTYPE_TSV
    };

    enum FT {
        FT_UNKNOWN = 0,
        FT_REGION,  // For Memory regions
        FT_PROCESS,
        FT_BINARY,
        FT_BINARY16,
        FT_BINARY32,
        FT_BINARY64,
        FT_COM,
        FT_MSDOS,
        FT_NE,
        FT_LE,
        FT_LX,
        FT_PE,
        FT_PE32,
        FT_PE64,
        FT_ELF,
        FT_ELF32,
        FT_ELF64,
        FT_MACHO,
        FT_MACHO32,
        FT_MACHO64,
        // Extra
        FT_7Z,
        FT_ANDROIDASRC,
        FT_ANDROIDXML,
        FT_APK,
        FT_APKS,
        FT_AR,
        FT_ARCHIVE,
        FT_CAB,
        FT_DEX,
        FT_DOCUMENT,
        FT_GIF,
        FT_BMP,
        FT_IMAGE,
        FT_VIDEO,
        FT_AUDIO,
        FT_IPA,
        FT_JAR,
        FT_JPEG,
        FT_MACHOFAT,
        FT_PDF,
        FT_PLAINTEXT,
        FT_PNG,
        FT_RAR,
        FT_TEXT,
        FT_TIFF,
        FT_UNICODE,
        FT_UNICODE_BE,
        FT_UNICODE_LE,
        FT_UTF8,
        FT_ZIP,
        FT_GZIP,
        FT_ZLIB,
        FT_LHA,
        FT_ICO,
        FT_CUR,
        FT_MP3,
        FT_MP4,
        FT_RIFF,
        FT_AVI,
        FT_WEBP,
        FT_SIGNATURE
        // TODO more
    };

    enum MODE {
        MODE_UNKNOWN = 0,
        MODE_DATA,  // Raw data
        MODE_BIT,   // 1/0
        MODE_8,
        MODE_16,
        MODE_16SEG,
        MODE_32,
        MODE_64,
        MODE_128,
        MODE_256,
        MODE_FREG
        // TODO more
    };

    enum DMFAMILY {
        DMFAMILY_UNKNOWN,
        DMFAMILY_X86,
        DMFAMILY_ARM,
        DMFAMILY_ARM64,
        DMFAMILY_MIPS,
        DMFAMILY_PPC,
        DMFAMILY_SPARC,
        DMFAMILY_SYSZ,
        DMFAMILY_XCORE,
        DMFAMILY_M68K,
        DMFAMILY_M68OK,
        DMFAMILY_RISCV,
        DMFAMILY_EVM,
        DMFAMILY_MOS65XX,
        DMFAMILY_WASM,
        DMFAMILY_BPF
    };

    enum DM {
        DM_UNKNOWN = 0,
        DM_X86_16,
        DM_X86_32,
        DM_X86_64,
        DM_ARM_LE,
        DM_ARM_BE,
        DM_ARM64_LE,
        DM_ARM64_BE,
        DM_CORTEXM,
        DM_THUMB_LE,
        DM_THUMB_BE,
        DM_MIPS_LE,
        DM_MIPS_BE,
        DM_MIPS64_LE,
        DM_MIPS64_BE,
        DM_PPC_LE,
        DM_PPC_BE,
        DM_PPC64_LE,
        DM_PPC64_BE,
        DM_SPARC,
        DM_S390X,
        DM_XCORE,
        DM_M68K,
        DM_M68K40,
        DM_TMS320C64X,
        DM_M6800,
        DM_M6801,
        DM_M6805,
        DM_M6808,
        DM_M6809,
        DM_M6811,
        DM_CPU12,
        DM_HD6301,
        DM_HD6309,
        DM_HCS08,
        DM_EVM,
        DM_RISKV32,
        DM_RISKV64,
        DM_RISKVC,
        DM_MOS65XX,
        DM_WASM,
        DM_BPF_LE,
        DM_BPF_BE
    };

    enum SYNTAX {
        SYNTAX_DEFAULT = 0,
        SYNTAX_INTEL,
        SYNTAX_ATT,
        SYNTAX_MASM,
        SYNTAX_MOTOROLA
    };

    enum TYPE {
        TYPE_UNKNOWN = 0,
        // TODO more
    };

    // TODO reactOS
    // TODO FreeDOS
    enum OSNAME {
        OSNAME_UNKNOWN = 0,
        OSNAME_AIX,
        OSNAME_ALPINELINUX,
        OSNAME_ANDROID,
        OSNAME_AROS,
        OSNAME_ASPLINUX,
        OSNAME_BORLANDOSSERVICES,
        OSNAME_BRIDGEOS,
        OSNAME_DEBIANLINUX,
        OSNAME_FENIXOS,
        OSNAME_FREEBSD,
        OSNAME_GENTOOLINUX,
        OSNAME_HANCOMLINUX,
        OSNAME_HPUX,
        OSNAME_IOS,
        OSNAME_IPADOS,
        OSNAME_IPHONEOS,
        OSNAME_IRIX,
        OSNAME_LINUX,
        OSNAME_MACOS,
        OSNAME_MAC_OS,
        OSNAME_MAC_OS_X,
        OSNAME_MANDRAKELINUX,
        OSNAME_MCLINUX,
        OSNAME_MINIX,
        OSNAME_MODESTO,
        OSNAME_MSDOS,
        OSNAME_NETBSD,
        OSNAME_NSK,
        OSNAME_OPENBSD,
        OSNAME_OPENVMS,
        OSNAME_OPENVOS,
        OSNAME_OS2,
        OSNAME_OS_X,
        OSNAME_POSIX,
        OSNAME_QNX,
        OSNAME_REDHATLINUX,
        OSNAME_SOLARIS,
        OSNAME_STARTOSLINUX,
        OSNAME_SUNOS,
        OSNAME_SUSELINUX,
        OSNAME_SYLLABLE,
        OSNAME_TRU64,
        OSNAME_TURBOLINUX,
        OSNAME_TVOS,
        OSNAME_UBUNTULINUX,
        OSNAME_UEFI,
        OSNAME_UNIX,
        OSNAME_VINELINUX,
        OSNAME_WATCHOS,
        OSNAME_WINDOWS,
        OSNAME_WINDOWSCE,
        OSNAME_WINDRIVERLINUX,
        OSNAME_XBOX,
        OSNAME_JVM,
        // TODO more
    };

    enum ENDIAN {
        ENDIAN_UNKNOWN = 0,
        ENDIAN_LITTLE,
        ENDIAN_BIG
    };

    struct OSINFO {
        OSNAME osName;
        QString sOsVersion;
        QString sBuild;
        QString sArch;
        MODE mode;
        QString sType;
        ENDIAN endian;
        bool bIsVM;
    };

    struct FILEFORMATINFO {
        bool bIsValid;
        qint64 nSize;
        FT fileType;
        QString sString;
        QString sExt;
        QString sVersion;
    };

    struct _MEMORY_MAP {
        XADDR nModuleAddress;
        qint64 nImageSize;
        qint64 nBinarySize;
        XADDR nEntryPointAddress;
        qint64 nSegmentBase;  // For MSDOS
        FT fileType;
        MODE mode;
        ENDIAN endian;
        QString sArch;
        QString sType;
        QList<_MEMORY_RECORD> listRecords;
    };

    enum SYMBOL_TYPE {
        SYMBOL_TYPE_UNKNOWN,
        SYMBOL_TYPE_EXPORT = 0x00000001,
        SYMBOL_TYPE_IMPORT = 0x00000002,
        SYMBOL_TYPE_LABEL = 0x00000004,  // DATA
        SYMBOL_TYPE_ANSISTRING = 0x00000008,
        SYMBOL_TYPE_UNICODESTRING = 0x00000010,
        SYMBOL_TYPE_ALL = 0xFFFFFFFF
    };

    struct SYMBOL_RECORD {
        XADDR nAddress;
        qint64 nSize;
        XADDR nModuleAddress;
        SYMBOL_TYPE symbolType;
        qint32 nOrdinal;  // For Windows OS;
        QString sName;
        QString sFunction;
    };

    enum HASH {
        HASH_MD4 = 0,
        HASH_MD5,
        HASH_SHA1,
#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
#if (QT_VERSION_MAJOR > 4)
        HASH_SHA224,
        HASH_SHA256,
        HASH_SHA384,
        HASH_SHA512,
#endif
        // TODO Check more
        // TODO Check Qt versions!
//        HASH_KECCAK_224,
//        HASH_KECCAK_256,
//        HASH_KECCAK_384,
//        HASH_KECCAK_512
#endif
    };

    enum MS_RECORD_TYPE {
        MS_RECORD_TYPE_UNKNOWN = 0,
        MS_RECORD_TYPE_STRING_ANSI,
        MS_RECORD_TYPE_STRING_UTF8,
        MS_RECORD_TYPE_STRING_UNICODE,
        MS_RECORD_TYPE_SIGNATURE,
        MS_RECORD_TYPE_VALUE
        // TODO more PASCAL(A/U)
    };

    struct MS_RECORD {
        qint64 nOffset;
        XADDR nAddress;
        QString sRegion;
        qint64 nSize;
        MS_RECORD_TYPE recordType;
        QString sString;
        QString sInfo;
    };

    struct OPCODE {
        XADDR nAddress;
        qint64 nSize;
        QString sName;
    };

    struct MEMORY_REPLACE  // For debuggers&breakpoints
    {
        XADDR nAddress;
        qint64 nOffset;
        qint64 nSize;
        QByteArray baOriginal;
    };

    //    struct XUINT128 {
    //        quint64 low;
    //        quint64 high;
    //    };

    struct XVARIANT {
        MODE mode;
        bool bIsBigEndian;
        union DUMMYUNION {
            bool v_bool;
            quint8 v_uint8;
            quint16 v_uint16;
            quint32 v_uint32;
            quint64 v_uint64;
            quint64 v_uint128[2];
            quint64 v_uint256[4];
            quint8 v_freg[10];
            // mb TODO 256/512
        } var;
    };

    struct SCANID {
        bool bVirtual;
        QString sUuid;
        XBinary::FT fileType;
        XBinary::FILEPART filePart;
        QString sArch;
        QString sVersion;
        QString sInfo;

        XBinary::MODE mode;
        XBinary::ENDIAN endian;
        QString sType;
        qint64 nSize;
        qint64 nOffset;
    };

    struct SCANSTRUCT {
        bool bIsHeuristic;
        XBinary::SCANID id;
        XBinary::SCANID parentId;
        QString sType;
        QString sName;
        QString sVersion;
        QString sInfo;
        QString varInfo;   // Signature in die scripts
        QString varInfo2;  // Signature File in die scripts
        Qt::GlobalColor globalColor;
        qint32 nPrio;
        bool bIsProtection;
    };

    struct PDRECORD {
        qint64 nCurrent;
        qint64 nTotal;
        QString sStatus;
        //        bool bSuccess;
        //        bool bFinished;
        bool bIsValid;
    };

    const static qint32 N_NUMBER_PDRECORDS = 5;

    struct PDSTRUCT {
        PDRECORD _pdRecord[N_NUMBER_PDRECORDS];
        bool bIsStop;
        quint64 nFinished;
        //        bool bIsDisable;
        //        QString sStatus;
        //        bool bErrors;
        //        bool bSuccess; // TODO important
        QString sInfoString;
        bool bCriticalError;  // TODO !!!
    };

private:
    enum ST {
        ST_COMPAREBYTES = 0,
        ST_NOTNULL,
        ST_ANSI,
        ST_NOTANSI,
        ST_NOTANSIANDNULL,
        ST_FINDBYTES,
        ST_SKIP,
        ST_RELOFFSET,
        ST_ADDRESS
    };

    struct SIGNATURE_RECORD {
        XADDR nBaseAddress;
        ST st;
        QByteArray baData;
        quint32 nSizeOfAddr;
        qint64 nFindDelta;
        qint32 nSize;
    };

public:
    explicit XBinary(QIODevice *pDevice = nullptr, bool bIsImage = false,
                     XADDR nModuleAddress = -1);  // mb TODO parent for signals/slot
    XBinary(const QString &sFileName);
    ~XBinary();

    void setData(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    void setDevice(QIODevice *pDevice);
    void setReadWriteMutex(QMutex *pReadWriteMutex);
    qint64 safeReadData(QIODevice *pDevice, qint64 nPos, char *pData, qint64 nMaxLen);
    qint64 safeWriteData(QIODevice *pDevice, qint64 nPos, const char *pData, qint64 nLen);
    qint64 getSize();
    static qint64 getSize(QIODevice *pDevice);
    static qint64 getSize(const QString &sFileName);

    void setMode(MODE mode);
    virtual MODE getMode();

    void setType(qint32 nType);
    virtual qint32 getType();
    virtual QString typeIdToString(qint32 nType);
    QString getTypeAsString();

    void setFileType(FT fileType);
    virtual FT getFileType();

    static QString modeIdToString(MODE mode);
    static QString endianToString(ENDIAN endian);

    void setArch(const QString &sArch);
    virtual QString getArch();

    void setFileFormatName(const QString &sFileFormatString);
    virtual QString getFileFormatString();

    void setFileFormatExt(const QString &sFileFormatExt);
    virtual QString getFileFormatExt();

    void setFileFormatSize(qint64 nFileFormatSize);
    virtual qint64 getFileFormatSize();

    virtual bool isSigned();
    virtual OFFSETSIZE getSignOffsetSize();  // TODO rename

    void setOsType(OSNAME osName);
    void setOsVersion(const QString &sOsVersion);
    virtual OSINFO getOsInfo();
    virtual FILEFORMATINFO getFileFormatInfo();

    void setEndian(ENDIAN endian);
    virtual ENDIAN getEndian();

    static bool isPacked(double dEntropy);

    static quint8 random8();
    static quint16 random16();
    static quint32 random32();
    static quint64 random64();
    static quint64 random(quint64 nLimit);

    static QString fileTypeIdToString(FT fileType);
    static QString fileTypeIdToExts(FT fileType);

    static QString convertFileName(const QString &sFileName);
    static QString convertPathName(const QString &sPathName);

    OS_STRING getOsAnsiString(qint64 nOffset, qint64 nSize);

    struct FFOPTIONS {
        QList<QString> *pListFileNames;
        bool bSubdirectories;
        bool *pbIsStop;
        qint32 *pnNumberOfFiles;
        // TODO filter
    };

    static void findFiles(const QString &sDirectoryName, FFOPTIONS *pFFOption, qint32 nLevel = 0);
    static void findFiles(const QString &sDirectoryName, QList<QString> *pListFileNames);
    static void findFiles(const QString &sDirectoryName, QList<QString> *pListFileNames, bool bSubDirectories, qint32 nLevel, PDSTRUCT *pPdStruct = nullptr);

    static QString regExp(const QString &sRegExp, const QString &sString, qint32 nIndex);
    static bool isRegExpPresent(const QString &sRegExp, const QString &sString);
    static qint32 getRegExpCount(const QString &sRegExp, const QString &sString);  // TODO Check!
    static QString getRegExpSection(const QString &sRegExp, const QString &sString, qint32 nStart, qint32 nEnd);
    qint64 read_array(qint64 nOffset, char *pBuffer, qint64 nMaxSize);
    QByteArray read_array(qint64 nOffset, qint64 nSize);
    qint64 write_array(qint64 nOffset, char *pBuffer, qint64 nSize);
    qint64 write_array(qint64 nOffset, QByteArray baData);

    static QByteArray read_array(QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    static qint64 read_array(QIODevice *pDevice, qint64 nOffset, char *pBuffer, qint64 nSize);
    static qint64 write_array(QIODevice *pDevice, qint64 nOffset, char *pBuffer, qint64 nSize);
    static qint64 write_array(QIODevice *pDevice, qint64 nOffset, QByteArray baData);

    quint8 read_uint8(qint64 nOffset);
    qint8 read_int8(qint64 nOffset);
    quint16 read_uint16(qint64 nOffset, bool bIsBigEndian = false);
    qint16 read_int16(qint64 nOffset, bool bIsBigEndian = false);
    quint32 read_uint32(qint64 nOffset, bool bIsBigEndian = false);
    qint32 read_int32(qint64 nOffset, bool bIsBigEndian = false);
    quint64 read_uint64(qint64 nOffset, bool bIsBigEndian = false);
    qint64 read_int64(qint64 nOffset, bool bIsBigEndian = false);
    float read_float16(qint64 nOffset, bool bIsBigEndian = false);  // TODO Check
    float read_float(qint64 nOffset, bool bIsBigEndian = false);    // TODO Check
    double read_double(qint64 nOffset, bool bIsBigEndian = false);  // TODO Check

    quint32 read_uint24(qint64 nOffset,
                        bool bIsBigEndian = false);  // Uses UPX in header
    qint32 read_int24(qint64 nOffset, bool bIsBigEndian = false);

    qint64 write_ansiString(qint64 nOffset, const QString &sString);
    void write_ansiStringFix(qint64 nOffset, qint64 nSize, const QString &sString);
    // TODO write unicodestring

    QString read_ansiString(qint64 nOffset, qint64 nMaxSize = 256);
    QString read_unicodeString(qint64 nOffset, qint64 nMaxSize = 256, bool bIsBigEndian = false);
    QString read_ucsdString(qint64 nOffset);
    QString read_utf8String(qint64 nOffset, qint64 nMaxSize = 256);
    QString _read_utf8String(qint64 nOffset, qint64 nMaxSize = 256);
    QString _read_utf8String(char *pData, qint64 nMaxSize);
    QString _read_utf8String(qint64 nOffset, char *pData, qint32 nDataSize, qint32 nDataOffset);

    QString read_codePageString(qint64 nOffset, qint64 nMaxByteSize = 256, const QString &sCodePage = "System");

    bool isUnicodeStringLatin(qint64 nOffset, qint64 nMaxSize = 256, bool bIsBigEndian = false);

    void write_uint8(qint64 nOffset, quint8 nValue);
    void write_int8(qint64 nOffset, qint8 nValue);
    void write_uint16(qint64 nOffset, quint16 nValue, bool bIsBigEndian = false);
    void write_int16(qint64 nOffset, qint16 nValue, bool bIsBigEndian = false);
    void write_uint32(qint64 nOffset, quint32 nValue, bool bIsBigEndian = false);
    void write_int32(qint64 nOffset, qint32 nValue, bool bIsBigEndian = false);
    void write_uint64(qint64 nOffset, quint64 nValue, bool bIsBigEndian = false);
    void write_int64(qint64 nOffset, qint64 nValue, bool bIsBigEndian = false);
    void write_float16(qint64 nOffset, float fValue,
                       bool bIsBigEndian = false);  // TODO Check
    void write_float(qint64 nOffset, float fValue,
                     bool bIsBigEndian = false);  // TODO Check
    void write_double(qint64 nOffset, double dValue,
                      bool bIsBigEndian = false);  // TODO Check

    QString read_UUID_bytes(qint64 nOffset);                       // uuid [16]
    void write_UUID_bytes(qint64 nOffset, const QString &sValue);  // uuid [16]

    QString read_UUID(qint64 nOffset, bool bIsBigEndian = false);

    static quint8 _read_uint8(char *pData);
    static qint8 _read_int8(char *pData);
    static quint16 _read_uint16(char *pData, bool bIsBigEndian = false);
    static qint16 _read_int16(char *pData, bool bIsBigEndian = false);
    static quint32 _read_uint32(char *pData, bool bIsBigEndian = false);
    static qint32 _read_int32(char *pData, bool bIsBigEndian = false);
    static quint64 _read_uint64(char *pData, bool bIsBigEndian = false);
    static qint64 _read_int64(char *pData, bool bIsBigEndian = false);
    static QString _read_ansiString(char *pData, qint32 nMaxSize = 50);
    static QByteArray _read_byteArray(char *pData, qint32 nSize);
    static float _read_float(char *pData,
                             bool bIsBigEndian = false);  // TODO Check
    static double _read_double(char *pData,
                               bool bIsBigEndian = false);  // TODO Check

    static quint64 _read_value(MODE mode, char *pData, bool bIsBigEndian = false);
    // TODO read uin64, freg

    static void _write_uint8(char *pData, quint8 nValue);
    static void _write_int8(char *pData, qint8 nValue);
    static void _write_uint16(char *pData, quint16 nValue, bool bIsBigEndian = false);
    static void _write_int16(char *pData, qint16 nValue, bool bIsBigEndian = false);
    static void _write_uint32(char *pData, quint32 nValue, bool bIsBigEndian = false);
    static void _write_int32(char *pData, qint32 nValue, bool bIsBigEndian = false);
    static void _write_uint64(char *pData, quint64 nValue, bool bIsBigEndian = false);
    static void _write_int64(char *pData, qint64 nValue, bool bIsBigEndian = false);
    static void _write_float(char *pData, float fValue,
                             bool bIsBigEndian = false);  // TODO Check
    static void _write_double(char *pData, double dValue,
                              bool bIsBigEndian = false);  // TODO Check

    static void _write_value(MODE mode, char *pData, quint64 nValue, bool bIsBigEndian = false);
    // TODO write uin64, freg

    quint8 read_bcd_uint8(qint64 nOffset);
    quint16 read_bcd_uint16(qint64 nOffset, bool bIsBigEndian = false);
    quint16 read_bcd_uint32(qint64 nOffset, bool bIsBigEndian = false);
    quint16 read_bcd_uint64(qint64 nOffset, bool bIsBigEndian = false);

    quint8 _bcd_decimal(quint8 nValue);

    qint64 find_array(qint64 nOffset, qint64 nSize, const char *pArray, qint64 nArraySize, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_byteArray(qint64 nOffset, qint64 nSize, QByteArray baData, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint8(qint64 nOffset, qint64 nSize, quint8 nValue, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int8(qint64 nOffset, qint64 nSize, qint8 nValue, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint16(qint64 nOffset, qint64 nSize, quint16 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int16(qint64 nOffset, qint64 nSize, qint16 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint32(qint64 nOffset, qint64 nSize, quint32 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int32(qint64 nOffset, qint64 nSize, qint32 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint64(qint64 nOffset, qint64 nSize, quint64 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int64(qint64 nOffset, qint64 nSize, qint64 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_float(qint64 nOffset, qint64 nSize, float fValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_double(qint64 nOffset, qint64 nSize, double dValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);

    static void endian_float(float *pValue, bool bIsBigEndian);
    static void endian_double(double *pValue, bool bIsBigEndian);

    qint64 find_ansiString(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_unicodeString(qint64 nOffset, qint64 nSize, const QString &sString, bool bIsBigEndian, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_utf8String(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_signature(qint64 nOffset, qint64 nSize, const QString &sSignature, qint64 *pnResultSize = 0, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_signature(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, const QString &sSignature, qint64 *pnResultSize = nullptr,
                          PDSTRUCT *pPdStruct = nullptr);
    qint64 find_ansiStringI(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_unicodeStringI(qint64 nOffset, qint64 nSize, const QString &sString, bool bIsBigEndian, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_utf8StringI(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    // TODO find_codePageString
    // TODO find_codePageStringI

    static quint8 getBits_uint8(quint8 nValue, qint32 nBitOffset, qint32 nBitSize);
    static quint16 getBits_uint16(quint16 nValue, qint32 nBitOffset, qint32 nBitSize);
    static quint32 getBits_uint32(quint32 nValue, qint32 nBitOffset, qint32 nBitSize);
    static quint64 getBits_uint64(quint64 nValue, qint32 nBitOffset, qint32 nBitSize);

    struct STRINGSEARCH_OPTIONS {
        // TODO more
        qint32 nLimit;
        qint64 nMinLenght;
        qint64 nMaxLenght;
        bool bAnsi;
        bool bUTF8;
        bool bUnicode;
        bool bCStrings;
        QString sExpFilter;
        QString sANSICodec;
        bool bLinks;
    };

    enum VT {
        VT_UNKNOWN = 0,
        VT_ANSISTRING,
        VT_ANSISTRING_I,
        VT_UNICODESTRING,
        VT_UNICODESTRING_I,
        VT_UTF8STRING,
        VT_UTF8STRING_I,
        VT_SIGNATURE,
        VT_BYTE,
        VT_WORD,
        VT_DWORD,
        VT_QWORD,
        VT_CHAR,
        VT_UCHAR,
        VT_SHORT,
        VT_USHORT,
        VT_INT,
        VT_UINT,
        VT_INT64,
        VT_UINT64,
        VT_DOUBLE,
        VT_FLOAT,
        // TODO UTF8
        // TODO pascal strings(A/U)
    };

    enum SF {
        SF_BEGIN = 0,
        SF_CURRENTOFFSET
    };

    struct SEARCHDATA {
        qint64 nResultOffset;
        qint64 nResultSize;
        qint64 nCurrentOffset;
        SF startFrom;
        QVariant varValue;
        VT valueType;
        bool bIsBigEndian;
        bool bInit;
    };

    bool _addMultiSearchStringRecord(QList<MS_RECORD> *pList, MS_RECORD *pRecord, STRINGSEARCH_OPTIONS *pSsOptions);

    QList<MS_RECORD> multiSearch_allStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct = nullptr);
    QList<MS_RECORD> multiSearch_signature(qint64 nOffset, qint64 nSize, qint32 nLimit, const QString &sSignature, const QString &sInfo = "",
                                           PDSTRUCT *pPdStruct = nullptr);
    QList<MS_RECORD> multiSearch_signature(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, qint32 nLimit, const QString &sSignature, const QString &sInfo = "",
                                           PDSTRUCT *pPdStruct = nullptr);
    QList<MS_RECORD> multiSearch_value(qint64 nOffset, qint64 nSize, qint32 nLimit, QVariant varValue, VT valueType, bool bIsBigEndian, PDSTRUCT *pPdStruct = nullptr);
    QList<MS_RECORD> multiSearch_value(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, qint32 nLimit, QVariant varValue, VT valueType, bool bIsBigEndian,
                                       PDSTRUCT *pPdStruct = nullptr);

    qint64 find_value(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, QVariant varValue, VT valueType, bool bIsBigEndian, qint64 *pnResultSize,
                      PDSTRUCT *pPdStruct = nullptr);

    static QString msRecordTypeIdToString(MS_RECORD_TYPE msRecordTypeId);

    static QString valueTypeToString(VT valueType);
    static QString getValueString(QVariant varValue, VT valueType);
    static qint32 getValueSize(QVariant varValue, VT valueType);
    static VT getValueType(quint64 nValue);

    static QByteArray getUnicodeString(const QString &sString, bool bIsBigEndian);
    static QByteArray getStringData(MS_RECORD_TYPE msRecordTypeId, const QString &sString, bool bAddNull);

    bool isSignaturePresent(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);
    static bool isSignatureValid(const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);

    static bool createFile(const QString &sFileName, qint64 nFileSize = 0);
    static bool isFileExists(const QString &sFileName, bool bTryToOpen = false);
    static bool removeFile(const QString &sFileName);
    static bool copyFile(const QString &sSrcFileName, const QString &sDestFileName);
    static bool moveFile(const QString &sSrcFileName, const QString &sDestFileName);
    static bool moveFileToDirectory(const QString &sSrcFileName, const QString &sDestDirectory);
    static QString convertFileNameSymbols(const QString &sFileName);
    static QString getBaseFileName(const QString &sFileName);
    static bool createDirectory(const QString &sDirectoryName);
    static bool isDirectoryExists(const QString &sDirectoryName);
    static bool removeDirectory(const QString &sDirectoryName);
    static bool isDirectoryEmpty(const QString &sDirectoryName);

    static QByteArray readFile(const QString &sFileName);

    static void _copyMemory(char *pDest, char *pSource, qint64 nSize);
    static void _zeroMemory(char *pDest, qint64 nSize);
    static bool _isMemoryZeroFilled(char *pSource, qint64 nSize);
    static bool _isMemoryNotNull(char *pSource, qint64 nSize);
    static bool _isMemoryAnsi(char *pSource, qint64 nSize);
    static bool _isMemoryNotAnsi(char *pSource, qint64 nSize);
    static bool _isMemoryNotAnsiAndNull(char *pSource, qint64 nSize);
    static bool copyDeviceMemory(QIODevice *pSourceDevice, qint64 nSourceOffset, QIODevice *pDestDevice, qint64 nDestOffset, qint64 nSize, quint32 nBufferSize = 0x1000);
    bool copyMemory(qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize, quint32 nBufferSize = 1, bool bReverse = false);
    bool zeroFill(qint64 nOffset, qint64 nSize);
    static bool compareMemory(char *pMemory1, const char *pMemory2, qint64 nSize);
    // For strings compare
    static bool compareMemoryByteI(quint8 *pMemory, const quint8 *pMemoryU, const quint8 *pMemoryL,
                                   qint64 nSize);  // Ansi
    static bool compareMemoryWordI(quint16 *pMemory, const quint16 *pMemoryU, const quint16 *pMemoryL,
                                   qint64 nSize);  // Unicode

    bool isOffsetValid(qint64 nOffset);
    bool isAddressValid(XADDR nAddress);
    bool isRelAddressValid(qint64 nRelAddress);
    bool isAddressPhysical(XADDR nAddress);

    XADDR offsetToAddress(qint64 nOffset);
    qint64 addressToOffset(XADDR nAddress);
    XADDR offsetToRelAddress(qint64 nOffset);
    qint64 relAddressToOffset(qint64 nRelAddress);

    static bool isOffsetValid(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static bool isOffsetAndSizeValid(_MEMORY_MAP *pMemoryMap, OFFSETSIZE *pOsRegion);
    bool isOffsetAndSizeValid(qint64 nOffset, qint64 nSize);
    static bool isOffsetAndSizeValid(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize);
    static bool isOffsetAndSizeValid(QIODevice *pDevice, qint64 nOffset, qint64 nSize);

    static bool isAddressValid(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static bool isRelAddressValid(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);

    static bool isAddressPhysical(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static bool isRelAddressPhysical(_MEMORY_MAP *pMemoryMap, XADDR nAddress);

    static XADDR offsetToAddress(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static qint64 addressToOffset(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static qint64 offsetToRelAddress(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static qint64 relAddressToOffset(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static qint64 relAddressToAddress(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static qint64 addressToRelAddress(_MEMORY_MAP *pMemoryMap, XADDR nAddress);

    static _MEMORY_RECORD getMemoryRecordByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static _MEMORY_RECORD getMemoryRecordByAddress(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static _MEMORY_RECORD getMemoryRecordByRelAddress(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);

    static qint32 addressToLoadSection(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static qint32 relAddressToLoadSection(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static bool isAddressInHeader(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static bool isRelAddressInHeader(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);

    static QString getLoadSectionNameByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);

    static bool isSolidAddressRange(_MEMORY_MAP *pMemoryMap, XADDR nAddress, qint64 nSize);

    QString getMemoryRecordInfoByOffset(qint64 nOffset);
    QString getMemoryRecordInfoByAddress(XADDR nAddress);
    QString getMemoryRecordInfoByRelAddress(qint64 nRelAddress);
    static QString getMemoryRecordInfoByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static QString getMemoryRecordInfoByAddress(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static QString getMemoryRecordInfoByRelAddress(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static QString getMemoryRecordName(_MEMORY_RECORD *pMemoryRecord);

    enum MAPMODE {
        MAPMODE_UNKNOWN = 0,
        MAPMODE_REGIONS,
        MAPMODE_SEGMENTS,
        MAPMODE_SECTIONS
    };

    static QString mapModeToString(MAPMODE mapMode);

    virtual QList<MAPMODE> getMapModesList(PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);

    static qint32 getNumberOfPhysicalRecords(_MEMORY_MAP *pMemoryMap);
    static qint32 getNumberOfVirtualRecords(_MEMORY_MAP *pMemoryMap);
    static qint64 getRecordsTotalRowSize(_MEMORY_MAP *pMemoryMap);

    virtual XADDR getBaseAddress();
    virtual void setBaseAddress(XADDR nBaseAddress);
    virtual qint64 getImageSize();

    qint64 _getEntryPointOffset();
    virtual qint64 getEntryPointOffset(_MEMORY_MAP *pMemoryMap);

    virtual void setEntryPointOffset(qint64 nEntryPointOffset);
    XADDR getEntryPointAddress();
    XADDR getEntryPointAddress(_MEMORY_MAP *pMemoryMap);

    qint64 getEntryPointRVA();
    qint64 getEntryPointRVA(_MEMORY_MAP *pMemoryMap);

    static XADDR getLowestAddress(_MEMORY_MAP *pMemoryMap);
    static qint64 getTotalVirtualSize(_MEMORY_MAP *pMemoryMap);
    static XADDR positionToVirtualAddress(_MEMORY_MAP *pMemoryMap, qint64 nPosition);

    void setModuleAddress(XADDR nValue);
    XADDR getModuleAddress();

    bool isImage();
    void setIsImage(bool bValue);

    void setMultiSearchCallbackState(bool bState);

    static qint64 getPhysSize(char *pBuffer, qint64 nSize);  // TODO Check!
    static bool isEmptyData(char *pBuffer, qint64 nSize);
    bool compareSignature(const QString &sSignature, qint64 nOffset = 0);
    bool compareSignature(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset = 0, PDSTRUCT *pPdStruct = nullptr);
    static bool _compareByteArrayWithSignature(QByteArray baData, const QString &sSignature);
    static QString _createSignature(const QString &sSignature1, const QString &sSignature2);

    bool compareSignatureOnAddress(const QString &sSignature, XADDR nAddress);
    bool compareSignatureOnAddress(_MEMORY_MAP *pMemoryMap, const QString &sSignature, XADDR nAddress);

    bool compareEntryPoint(const QString &sSignature, qint64 nOffset = 0);
    bool compareEntryPoint(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset = 0);

    bool moveMemory(qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize);
    static bool moveMemory(QIODevice *pDevice, qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize);

    static bool dumpToFile(const QString &sFileName, const char *pData, qint64 nDataSize);
    bool dumpToFile(const QString &sFileName, qint64 nDataOffset, qint64 nDataSize, PDSTRUCT *pPdStruct = nullptr);

    bool patchFromFile(const QString &sFileName, qint64 nDataOffset, qint64 nDataSize, PDSTRUCT *pPdStruct = nullptr);

    QSet<FT> getFileTypes(bool bExtra = false);
    static QSet<FT> getFileTypes(QIODevice *pDevice,
                                 bool bExtra = false);  // mb TODO isImage
    static QSet<FT> getFileTypes(QIODevice *pDevice, qint64 nOffset, qint64 nSize, bool bExtra = false);
    static QSet<FT> getFileTypes(const QString &sFileName, bool bExtra = false);
    static QSet<FT> getFileTypes(QByteArray *pbaData, bool bExtra = false);

    static FT _getPrefFileType(QSet<XBinary::FT> *pStFileTypes);
    static FT getPrefFileType(QIODevice *pDevice, bool bExtra = false);
    static FT getPrefFileType(const QString &sFileName, bool bExtra = false);

    static QList<FT> _getFileTypeListFromSet(QSet<FT> stFileTypes);

    static QString valueToHex(quint8 nValue);
    static QString valueToHex(qint8 nValue);
    static QString valueToHex(quint16 nValue, bool bIsBigEndian = false);
    static QString valueToHex(qint16 nValue, bool bIsBigEndian = false);
    static QString valueToHex(quint32 nValue, bool bIsBigEndian = false);
    static QString valueToHex(qint32 nValue, bool bIsBigEndian = false);
    static QString valueToHex(quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHex(qint64 nValue, bool bIsBigEndian = false);
    static QString valueToHex(float fValue, bool bIsBigEndian = false);
    static QString valueToHex(double dValue, bool bIsBigEndian = false);
    static QString valueToHex(MODE mode, quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHexEx(quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHexOS(quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHexColon(MODE mode, quint64 nValue, bool bIsBigEndian = false);
    static QString xVariantToHex(XVARIANT value);

    static QString thisToString(qint64 nDelta);

    static bool checkString_byte(const QString &sValue);
    static bool checkString_word(const QString &sValue);
    static bool checkString_dword(const QString &sValue);
    static bool checkString_qword(const QString &sValue);
    static bool checkString_uint8(const QString &sValue);
    static bool checkString_int8(const QString &sValue);
    static bool checkString_uint16(const QString &sValue);
    static bool checkString_int16(const QString &sValue);
    static bool checkString_uint32(const QString &sValue);
    static bool checkString_int32(const QString &sValue);
    static bool checkString_uint64(const QString &sValue);
    static bool checkString_int64(const QString &sValue);
    static bool checkString_float(const QString &sValue);
    static bool checkString_double(const QString &sValue);

    static QString boolToString(bool bValue);
    static QString getSpaces(qint32 nNumberOfSpaces);

    static QString getUnpackedFileName(QIODevice *pDevice, bool bShort = false);
    static QString getUnpackedFileName(const QString &sFileName);
    static QString getDumpFileName(QIODevice *pDevice);
    static QString getDumpFileName(const QString &sFileName);
    static QString getBackupFileName(QIODevice *pDevice);
    static QString getBackupFileName(const QString &sFileName);
    static QString getResultFileName(QIODevice *pDevice, const QString &sAppendix);
    static QString getResultFileName(const QString &sFileName, const QString &sAppendix);
    static QString getDeviceFileName(QIODevice *pDevice);
    static QString getDeviceFilePath(QIODevice *pDevice);
    static QString getDeviceDirectory(QIODevice *pDevice);
    static QString getDeviceFileBaseName(QIODevice *pDevice);
    static QString getDeviceFileCompleteSuffix(QIODevice *pDevice);
    static QString getDeviceFileSuffix(QIODevice *pDevice);
    static QString getFileDirectory(const QString &sFileName);

    static bool isBackupPresent(QIODevice *pDevice);
    static bool saveBackup(QIODevice *pDevice);

    static QString getCurrentBackupDate();

    static QList<qint64> getFixupList(QIODevice *pDevice1, QIODevice *pDevice2, qint64 nDelta);

    static QString getHash(HASH hash, const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static QString getHash(HASH hash, QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    QString getHash(HASH hash, qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);
    QString getHash(HASH hash, QList<OFFSETSIZE> *pListOS, PDSTRUCT *pPdStruct = nullptr);

    static QSet<HASH> getHashMethods();
    static QList<HASH> getHashMethodsAsList();
    static QString hashIdToString(HASH hash);

    static bool isFileHashValid(HASH hash, const QString &sFileName, const QString &sHash);

    static quint32 getAdler32(const QString &sFileName);  // TODO ProcessData
    static quint32 getAdler32(QIODevice *pDevice);        // TODO ProcessData
    quint32 getAdler32(qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    static void _createCRC32Table(quint32 *pCRCTable, quint32 nPoly = 0xEDB88320);
    static quint32 *_getCRC32Table_EDB88320();

    static quint32 _getCRC32(const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static quint32 _getCRC32(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    static quint32 _getCRC32(const char *pData, qint32 nDataSize, quint32 nInit, quint32 *pCRCTable);
    static quint32 _getCRC32(const QByteArray &baData, quint32 nInit, quint32 *pCRCTable);
    quint32 _getCRC32(qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    static quint32 _getCRC32ByFileContent(const QString &sFileName);
    static quint32 _getCRC32ByDirectory(const QString &sDirectoryName, bool bRecursive, quint32 nInit = 0xFFFFFFFF);  // TODO PDSTRUCT

    static double getEntropy(const QString &sFileName);  // TODO ProcessData
    static double getEntropy(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    double getEntropy(qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    BYTE_COUNTS getByteCounts(qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    void _xor(quint8 nXorValue, qint64 nOffset = 0, qint64 nSize = -1);

    //    static quint32 _ror32(quint32 nValue,quint32 nShift);
    //    static quint32 _rol32(quint32 nValue,quint32 nShift);
    static quint32 getStringCustomCRC32(const QString &sString);

    QIODevice *getDevice();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    static MODE getMode(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);

    bool isBigEndian();
    bool is16();
    bool is32();
    bool is64();
    static bool isBigEndian(_MEMORY_MAP *pMemoryMap);
    static bool is16(_MEMORY_MAP *pMemoryMap);
    static bool is32(_MEMORY_MAP *pMemoryMap);
    static bool is64(_MEMORY_MAP *pMemoryMap);

    void setVersion(const QString &sVersion);
    virtual QString getVersion();
    virtual bool isEncrypted();

    static QString getSignature(QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    QString getSignature(qint64 nOffset, qint64 nSize);

    OFFSETSIZE convertOffsetAndSize(qint64 nOffset,
                                    qint64 nSize);  // TODO rename
    static OFFSETSIZE convertOffsetAndSize(QIODevice *pDevice, qint64 nOffset,
                                           qint64 nSize);  // TODO rename

    static bool compareSignatureStrings(const QString &sBaseSignature, const QString &sOptSignature);
    static QString stringToHex(const QString &sString);
    static QString hexToString(const QString &sHex);
    static QString floatToString(float fValue, qint32 nPrec = 2);
    static QString doubleToString(double dValue, qint32 nPrec = 2);

    static quint8 hexToUint8(const QString &sHex);
    static qint8 hexToInt8(const QString &sHex);
    static quint16 hexToUint16(const QString &sHex, bool bIsBigEndian = false);
    static qint16 hexToInt16(const QString &sHex, bool bIsBigEndian = false);
    static quint32 hexToUint32(const QString &sHex, bool bIsBigEndian = false);
    static qint32 hexToInt32(const QString &sHex, bool bIsBigEndian = false);
    static quint64 hexToUint64(const QString &sHex, bool bIsBigEndian = false);
    static qint64 hexToInt64(const QString &sHex, bool bIsBigEndian = false);

    static QString invertHexByteString(const QString &sHex);

    static void _swapBytes(char *pSource, qint32 nSize);
    static quint16 swapBytes(quint16 nValue);
    static quint32 swapBytes(quint32 nValue);
    static quint64 swapBytes(quint64 nValue);

    bool isPlainTextType();
    bool isPlainTextType(QByteArray *pbaData);
    bool isUTF8TextType();
    bool isUTF8TextType(QByteArray *pbaData);

    static bool isPlainTextType(QIODevice *pDevice);

    enum UNICODE_TYPE {
        UNICODE_TYPE_NONE = 0,
        UNICODE_TYPE_LE,
        UNICODE_TYPE_BE
    };

    UNICODE_TYPE getUnicodeType();
    UNICODE_TYPE getUnicodeType(QByteArray *pbaData);

    static bool tryToOpen(QIODevice *pDevice);

    bool checkOffsetSize(OFFSETSIZE osRegion);
    bool checkOffsetSize(qint64 nOffset, qint64 nSize);

    static QString get_uint32_full_version(quint32 nValue);
    static QString get_uint32_version(quint32 nValue);
    static bool isResizeEnable(QIODevice *pDevice);
    static bool resize(QIODevice *pDevice, qint64 nSize);

    struct PACKED_INT {
        bool bIsValid;
        quint64 nValue;
        quint32 nByteSize;
    };

    PACKED_INT read_uleb128(qint64 nOffset, qint64 nSize);
    PACKED_INT _read_uleb128(char *pData, qint64 nSize);

    PACKED_INT read_acn1_integer(qint64 nOffset, qint64 nSize);

    struct PACKED {
        quint64 nValue;
        quint32 nByteSize;
    };

    PACKED get_packedNumber(qint64 nOffset);

    static QList<QString> getListFromFile(const QString &sFileName);

    qint64 getOverlaySize();
    static qint64 getOverlaySize(_MEMORY_MAP *pMemoryMap);
    qint64 getOverlayOffset();
    static qint64 getOverlayOffset(_MEMORY_MAP *pMemoryMap);
    bool isOverlayPresent();
    static bool isOverlayPresent(_MEMORY_MAP *pMemoryMap);

    bool compareOverlay(const QString &sSignature, qint64 nOffset);
    bool compareOverlay(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset);

    bool addOverlay(char *pData, qint64 nDataSize);
    bool addOverlay(const QString &sFileName);
    bool removeOverlay();

    bool isSignatureInLoadSegmentPresent(qint32 nLoadSegment, const QString &sSignature);
    bool isSignatureInLoadSegmentPresent(_MEMORY_MAP *pMemoryMap, qint32 nLoadSegment, const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);

    static QString getStringCollision(QList<QString> *pListStrings, const QString &sString1, const QString &sString2);

    static bool writeToFile(const QString &sFileName, QByteArray baData);
    static bool writeToFile(const QString &sFileName, QIODevice *pDevice);
    static bool appendToFile(const QString &sFileName, const QString &sString);  // TODO rename
    static bool clearFile(const QString &sFileName);
    static qint32 getStringNumberFromList(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static qint32 getStringNumberFromListExp(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static bool isStringInListPresent(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static bool isStringInListPresentExp(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static QString getStringByIndex(QList<QString> *pListStrings, qint32 nIndex, qint32 nNumberOfStrings = -1);

    static bool isStringUnicode(const QString &sString, qint32 nMaxCheckSize = -1);

    static quint32 elfHash(const quint8 *pData);

    static QString getVersionString(const QString &sString);
    static qint64 getVersionIntValue(const QString &sString);
    static bool checkStringNumber(const QString &sString, quint32 nMin, quint32 nMax);

    enum DT_TYPE {
        DT_TYPE_UNKNOWN = 0,
        DT_TYPE_POSIX
        // TODO more
    };

    static QDateTime valueToTime(quint64 nValue, DT_TYPE type);
    static QString valueToTimeString(quint64 nValue, DT_TYPE type);

    enum VL_TYPE {
        VL_TYPE_LIST = 0,
        VL_TYPE_FLAGS
    };

    static QString valueToFlagsString(quint64 nValue, QMap<quint64, QString> mapFlags, VL_TYPE vlType);

    static bool isX86asm(const QString &sArch);  // TODO remove use getDisasmMode
    static QString disasmIdToString(DM disasmMode);
    static QString syntaxIdToString(SYNTAX syntax);
    static SYNTAX stringToSyntaxId(const QString &sString);
    static QString osNameIdToString(OSNAME osName);
    DM getDisasmMode();
    static DM getDisasmMode(_MEMORY_MAP *pMemoryMap);
    static DM getDisasmMode(const QString &sArch, bool bIsBigEndian = false, MODE mode = MODE_UNKNOWN);
    static DM getDisasmMode(OSINFO osInfo);
    static DMFAMILY getDisasmFamily(DM disasmMode);
    static DMFAMILY getDisasmFamily(_MEMORY_MAP *pMemoryMap);
    static QList<SYNTAX> getDisasmSyntax(DM disasmMode);
    static MODE getModeFromDisasmMode(DM disasmMode);

    static bool checkFileType(XBinary::FT fileTypeMain, XBinary::FT fileTypeOptional);

    static void filterFileTypes(QSet<XBinary::FT> *pStFileTypes);
    static void filterFileTypes(QSet<XBinary::FT> *pStFileTypes, XBinary::FT fileType);

    static bool isFileTypePresent(QSet<XBinary::FT> *pStFileTypes, QSet<XBinary::FT> *pStAvailableFileTypes);

    struct PROCENT {
        qint64 nCurrentValue;
        qint64 nMaxValue;
        qint32 nCurrentProcent;
        qint32 nMaxProcent;
        bool bTimer;
        QElapsedTimer timer;
    };

    static PROCENT procentInit(qint64 nMaxValue, bool bTimer = false);
    static bool procentSetCurrentValue(PROCENT *pProcent, qint64 nCurrentValue);

    static qint64 getTotalOSSize(QList<OFFSETSIZE> *pListOffsetSize);

    static MODE getWidthModeFromSize(quint64 nSize);  // TODO rename
    static MODE getWidthModeFromMemoryMap(_MEMORY_MAP *pMemoryMap);

    static MODE getWidthModeFromByteSize(quint32 nByteSize);

    static bool isAnsiSymbol(quint8 cCode, bool bExtra = false);
    static bool isUTF8Symbol(quint8 cCode, qint32 *pnWidth);
    static bool isUnicodeSymbol(quint16 nCode, bool bExtra = false);
    QString getStringFromIndex(qint64 nOffset, qint64 nSize, qint32 nIndex);

    static QList<QString> getAllFilesFromDirectory(const QString &sDirectory, const QString &sExtension);

    enum OPCODE_STATUS {
        OPCODE_STATUS_SUCCESS = 0,
        OPCODE_STATUS_END
    };

    QList<OPCODE> getOpcodes(qint64 nOffset, XADDR nStartAddress, qint64 nSize, quint32 nType, PDSTRUCT *pPdStruct = nullptr);
    virtual XADDR readOpcodes(quint32 nType, char *pData, XADDR nStartAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus);

    bool _read_opcode_uleb128(OPCODE *pOpcode, char **ppData, qint64 *pnSize, XADDR *pnAddress, XADDR *pnResult, const QString &sPrefix);
    bool _read_opcode_ansiString(OPCODE *pOpcode, char **ppData, qint64 *pnSize, XADDR *pnAddress, XADDR *pnResult, const QString &sPrefix);

    QList<quint32> get_uint32_list(qint64 nOffset, qint32 nNumberOfRecords, bool bIsBigEndian = false);
    QList<quint64> get_uint64_list(qint64 nOffset, qint32 nNumberOfRecords, bool bIsBigEndian = false);

    static bool _isOffsetsCrossed(qint64 nOffset1, qint64 nSize1, qint64 nOffset2, qint64 nSize2);
    static bool _isAddressCrossed(XADDR nAddress1, qint64 nSize1, XADDR nAddress2, qint64 nSize2);
    static bool _isReplaced(qint64 nOffset, qint64 nSize, QList<MEMORY_REPLACE> *pListMemoryReplace);
    static bool _replaceMemory(qint64 nDataOffset, char *pData, qint64 nDataSize, QList<MEMORY_REPLACE> *pListMemoryReplace);
    static bool _updateReplaces(qint64 nDataOffset, char *pData, qint64 nDataSize, QList<MEMORY_REPLACE> *pListMemoryReplace);

    virtual QList<SYMBOL_RECORD> getSymbolRecords(XBinary::_MEMORY_MAP *pMemoryMap, SYMBOL_TYPE symbolType = SYMBOL_TYPE_ALL);
    static SYMBOL_RECORD findSymbolByAddress(QList<SYMBOL_RECORD> *pListSymbolRecords, XADDR nAddress);     // Check InfoDB
    static SYMBOL_RECORD findSymbolByName(QList<SYMBOL_RECORD> *pListSymbolRecords, const QString &sName);  // Check InfoDB
    static SYMBOL_RECORD findSymbolByOrdinal(QList<SYMBOL_RECORD> *pListSymbolRecords, qint32 nOrdinal);    // Check InfoDB

    static QString generateUUID();

    static QString appendText(const QString &sResult, const QString &sString, const QString &sSeparate);
    static QString bytesCountToString(quint64 nValue, quint64 nBase = 1024);
    static QString numberToString(quint64 nValue);
    static QString fullVersionDwordToString(quint32 nValue);
    static QString versionDwordToString(quint32 nValue);
    static QString formatXML(const QString &sXML);

    struct XDWORD {
        quint16 nValue1;
        quint16 nValue2;
    };

    static quint32 make_dword(XDWORD xdword);
    static quint32 make_dword(quint16 nValue1, quint16 nValue2);
    static XDWORD make_xdword(quint32 nValue);

    static QString recordFilePartIdToString(FILEPART id);

    static QString createTypeString(const SCANSTRUCT *pScanStruct);
    static SCANSTRUCT createHeaderScanStruct(const SCANSTRUCT *pScanStruct);
    static QString createResultString2(const SCANSTRUCT *pScanStruct);

    static bool checkVersionString(const QString &sVersion);

    static XVARIANT getXVariant(bool bValue);
    static XVARIANT getXVariant(quint8 nValue);
    static XVARIANT getXVariant(quint16 nValue, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint32 nValue, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nValue, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nValue[2], bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nLow, quint64 nHigh, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nLow1, quint64 nLow2, quint64 nHigh1, quint64 nHigh2, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint8 nValue[10], bool bIsBigEndian = false);

    static quint64 xVariantToQword(XVARIANT xvariant);

    static quint32 getDwordFromQword(quint64 nValue, qint32 nIndex);
    static quint16 getWordFromQword(quint64 nValue, qint32 nIndex);
    static quint8 getByteFromQword(quint64 nValue, qint32 nIndex);
    static quint16 getWordFromDword(quint32 nValue, qint32 nIndex);
    static quint8 getByteFromDword(quint32 nValue, qint32 nIndex);
    static quint8 getByteFromWord(quint16 nValue, qint32 nIndex);
    static bool getBitFromByte(quint8 nValue, qint32 nIndex);
    static bool getBitFromWord(quint16 nValue, qint32 nIndex);
    static bool getBitFromDword(quint32 nValue, qint32 nIndex);
    static bool getBitFromQword(quint64 nValue, qint32 nIndex);

    static quint64 setDwordToQword(quint64 nInit, quint32 nValue, qint32 nIndex);
    static quint64 setWordToQword(quint64 nInit, quint16 nValue, qint32 nIndex);
    static quint64 setByteToQword(quint64 nInit, quint8 nValue, qint32 nIndex);
    static quint32 setWordToDword(quint32 nInit, quint16 nValue, qint32 nIndex);
    static quint32 setByteToDword(quint32 nInit, quint8 nValue, qint32 nIndex);
    static quint16 setByteToWord(quint16 nInit, quint8 nValue, qint32 nIndex);
    static quint8 setBitToByte(quint8 nInit, bool bValue, qint32 nIndex);
    static quint16 setBitToWord(quint16 nInit, bool bValue, qint32 nIndex);
    static quint32 setBitToDword(quint32 nInit, bool bValue, qint32 nIndex);
    static quint64 setBitToQword(quint64 nInit, bool bValue, qint32 nIndex);

    static bool isXVariantEqual(XVARIANT value1, XVARIANT value2);
    static void clearXVariant(XVARIANT *pVar);

    static MODE getModeOS();

    static PDSTRUCT createPdStruct();
    static void setPdStructInit(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nTotal);
    static void setPdStructTotal(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nValue);
    static void setPdStructCurrent(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nValue);
    static void setPdStructCurrentIncrement(PDSTRUCT *pPdStruct, qint32 nIndex);
    static void setPdStructStatus(PDSTRUCT *pPdStruct, qint32 nIndex, const QString &sStatus);
    static void setPdStructFinished(PDSTRUCT *pPdStruct, qint32 nIndex);
    static qint32 getFreeIndex(PDSTRUCT *pPdStruct);
    static bool isPdStructFinished(PDSTRUCT *pPdStruct);
    static bool isPdStructSuccess(PDSTRUCT *pPdStruct);
    static qint32 getPdStructProcent(PDSTRUCT *pPdStruct);  // 0-100

    struct REGION_FILL {
        quint64 nSize;
        quint8 nByte;
    };

    REGION_FILL getRegionFill(qint64 nOffset, qint64 nSize, qint32 nAlignment);
    static QString getDataString(char *pData, qint32 nDataSize);

    struct HREGION {
        XADDR nAddress;
        qint64 nOffset;
        qint64 nSize;
        QString sName;
    };

    QList<HREGION> getHRegions(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);
    virtual QList<HREGION> getHighlights(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    static qint64 align_up(qint64 nValue, qint64 nAlignment);
    static qint64 align_down(qint64 nValue, qint64 nAlignment);

    static char *strCopy(char *pszDest, char *pszSource);
    static QString getAndroidVersionFromApi(quint32 nAPI);

private:
    static const qint32 READWRITE_BUFFER_SIZE = 0x1000;
    static QString convertSignature(const QString &sSignature);
    static QString qcharToHex(QChar cSymbol);

    static QList<SIGNATURE_RECORD> getSignatureRecords(const QString &sSignature, bool *pbValid, PDSTRUCT *pPdStruct);
    bool _compareSignature(_MEMORY_MAP *pMemoryMap, QList<SIGNATURE_RECORD> *pListSignatureRecords, qint64 nOffset);

    static qint32 _getSignatureSkip(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureNotNull(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureANSI(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureNotANSI(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureNotANSIAndNull(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureDelta(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex, bool *pbValid, PDSTRUCT *pPdStruct);
    static qint32 _getSignatureRelOffset(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureAddress(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureBytes(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex, bool *pbValid, PDSTRUCT *pPdStruct);

protected:
    bool _isOffsetValid(qint64 nOffset);
    void _errorMessage(const QString &sErrorMessage);
    void _infoMessage(const QString &sInfoMessage);
    qint64 _calculateRawSize();
    static qint64 _calculateRawSize(_MEMORY_MAP *pMemoryMap);

signals:
    void errorMessage(const QString &sErrorMessage);
    void infoMessage(const QString &sInfoMessage);

private:
    QIODevice *g_pDevice;
    QString g_sFileName;
    QMutex *g_pReadWriteMutex;
    bool g_bIsImage;
    XADDR g_nBaseAddress;
    qint64 g_nEntryPointOffset;
    XADDR g_nModuleAddress;
    QString g_sArch;
    QString g_sFileFormatString;
    QString g_sFileFormatExt;
    qint64 g_nFileFormatSize;
    OSNAME g_osName;
    QString g_sOsVersion;
    ENDIAN g_endian;
    MODE g_mode;
    QString g_sVersion;
    qint32 g_nType;
    FT g_fileType;
    bool g_bLog;  // TODO remove
    bool g_bMultiSearchCallback;
    qint64 g_nSize;
};

#endif  // XBINARY_H
