/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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
#include "jar_script.h"

JAR_Script::JAR_Script(XZip *pZip, OPTIONS *pOptions, XBinary::PDSTRUCT *pPdStruct) : ZIP_Script(pZip, pOptions, pPdStruct)
{
    this->g_pZip = pZip;

    g_sManifest = pZip->decompress(getArchiveRecords(), "META-INF/MANIFEST.MF", pPdStruct);
}

JAR_Script::~JAR_Script()
{
}

QString JAR_Script::getManifest()
{
    return g_sManifest;
}

QString JAR_Script::getManifestRecord(const QString &sRecord)
{
    QString sResult;
    QString sRegex = sRecord + ": (.*?)\n";

    sResult = XBinary::regExp(sRegex, g_sManifest, 1).remove("\r");

    return sResult;
}
