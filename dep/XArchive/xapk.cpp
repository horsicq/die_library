/* Copyright (c) 2017-2023 hors<horsicq@gmail.com>
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
#include "xapk.h"

XAPK::XAPK(QIODevice *pDevice) : XJAR(pDevice)
{
}

bool XAPK::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    XZip xzip(getDevice());

    if (xzip.isValid()) {
        QList<XArchive::RECORD> listArchiveRecords = xzip.getRecords(20000, pPdStruct);

        bResult = isValid(&listArchiveRecords, pPdStruct);
    }

    return bResult;
}

bool XAPK::isValid(QIODevice *pDevice)
{
    XAPK xapk(pDevice);

    return xapk.isValid();
}

bool XAPK::isValid(QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    bResult =
        (XArchive::isArchiveRecordPresent("classes.dex", pListRecords, pPdStruct) || XArchive::isArchiveRecordPresent("AndroidManifest.xml", pListRecords, pPdStruct));

    return bResult;
}

XBinary::FT XAPK::getFileType()
{
    return FT_APK;
}

XBinary::FILEFORMATINFO XAPK::getFileFormatInfo()
{
    XBinary::FILEFORMATINFO result = {};

    XAPK xapk(getDevice());

    if (xapk.isValid()) {
        result.bIsValid = true;
        result.nSize = xapk.getFileFormatSize();
        result.sString = "APK";
        result.sExt = "apk";
        result.fileType = FT_APK;
    }

    return result;
}

QString XAPK::getFileFormatExt()
{
    return "apk";
}

XBinary::OSINFO XAPK::getOsInfo()
{
    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    QList<XArchive::RECORD> listRecords = getRecords(2000, &pdStructEmpty);

    return getOsInfo(&listRecords, &pdStructEmpty);
}

XBinary::OSINFO XAPK::getOsInfo(QList<RECORD> *pListRecords, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pListRecords)
    Q_UNUSED(pPdStruct)

    XBinary::OSINFO result = {};

    result.osName = OSNAME_ANDROID;

    result.sArch = getArch();
    result.mode = getMode();
    result.sType = typeIdToString(getType());
    result.endian = getEndian();

    QByteArray baAndroidManifest = decompress(pListRecords, "AndroidManifest.xml", pPdStruct);

    if (baAndroidManifest.size() > 0) {
        QString sAndroidManifest = XAndroidBinary::getDecoded(&baAndroidManifest);

        QString sCompileSdkVersion = XBinary::regExp("android:compileSdkVersion=\"(.*?)\"", sAndroidManifest, 1);
        QString sCompileSdkVersionCodename = XBinary::regExp("android:compileSdkVersionCodename=\"(.*?)\"", sAndroidManifest, 1);
        QString sPlatformBuildVersionCode = XBinary::regExp("platformBuildVersionCode=\"(.*?)\"", sAndroidManifest, 1);
        QString sPlatformBuildVersionName = XBinary::regExp("platformBuildVersionName=\"(.*?)\"", sAndroidManifest, 1);
        QString sTargetSdkVersion = XBinary::regExp("android:targetSdkVersion=\"(.*?)\"", sAndroidManifest, 1);
        QString sMinSdkVersion = XBinary::regExp("android:minSdkVersion=\"(.*?)\"", sAndroidManifest, 1);

        // Check
        if (!XBinary::checkStringNumber(sCompileSdkVersion, 1, 40)) sCompileSdkVersion = "";
        if (!XBinary::checkStringNumber(sPlatformBuildVersionCode, 1, 40)) sPlatformBuildVersionCode = "";
        if (!XBinary::checkStringNumber(sTargetSdkVersion, 1, 40)) sTargetSdkVersion = "";
        if (!XBinary::checkStringNumber(sMinSdkVersion, 1, 40)) sMinSdkVersion = "";

        if (!XBinary::checkStringNumber(sCompileSdkVersionCodename.section(".", 0, 0), 1, 15)) sCompileSdkVersionCodename = "";
        if (!XBinary::checkStringNumber(sPlatformBuildVersionName.section(".", 0, 0), 1, 15)) sPlatformBuildVersionName = "";

        if ((sCompileSdkVersion != "") || (sCompileSdkVersionCodename != "") || (sPlatformBuildVersionCode != "") || (sPlatformBuildVersionName != "") ||
            (sTargetSdkVersion != "") || (sMinSdkVersion != "")) {
            QString _sVersion;
            QString _sAndroidVersion;

            if (_sVersion == "") _sVersion = sTargetSdkVersion;
            if (_sVersion == "") _sVersion = sMinSdkVersion;
            if (_sVersion == "") _sVersion = sCompileSdkVersion;
            if (_sVersion == "") _sVersion = sPlatformBuildVersionCode;

            if (_sAndroidVersion == "") _sAndroidVersion = sCompileSdkVersionCodename;
            if (_sAndroidVersion == "") _sAndroidVersion = sPlatformBuildVersionName;

            if (_sAndroidVersion == "") {
                _sAndroidVersion = XBinary::getAndroidVersionFromApi(_sVersion.toUInt());
            }

            result.sOsVersion = _sAndroidVersion;
            result.sBuild = sPlatformBuildVersionCode;
        }
    }

    return result;
}

XBinary::MODE XAPK::getMode()
{
    return MODE_DATA;
}

QString XAPK::getArch()
{
    return tr("Universal");
}

qint32 XAPK::getType()
{
    return TYPE_PACKAGE;
}

QString XAPK::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_PACKAGE: sResult = tr("Package");
    }

    return sResult;
}
