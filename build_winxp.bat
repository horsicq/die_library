set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0"
set SEVENZIP_PATH="C:\Program Files\7-Zip"
set QT_PATH="C:\Qt5.6.3\5.6.3\msvc2013"

set BUILD_NAME=die_lib_winxp
set SOURCE_PATH=%~dp0
mkdir %SOURCE_PATH%\release
set /p RELEASE_VERSION=<%SOURCE_PATH%\release_version.txt

set QT_PATH=%QT_PATH%
call %VS_PATH%\VC\bin\vcvars32.bat
set ZIP_NAME=%BUILD_NAME%_%RELEASE_VERSION%

cd build_libs
%QT_PATH%\bin\qmake.exe build_libs.pro -r -spec win32-msvc2013 "CONFIG+=release"

nmake Makefile.Release clean
nmake
del Makefile
del Makefile.Release
del Makefile.Debug

cd ..

cd lib_source
%QT_PATH%\bin\qmake.exe lib_source.pro -r -spec win32-msvc2013 "CONFIG+=release"

nmake Makefile.Release clean
nmake
del Makefile
del Makefile.Release
del Makefile.Debug

cd ..

mkdir %SOURCE_PATH%\release\%BUILD_NAME%

copy %SOURCE_PATH%\build\release\die.lib %SOURCE_PATH%\release\%BUILD_NAME%\
copy %SOURCE_PATH%\build\release\die.dll %SOURCE_PATH%\release\%BUILD_NAME%\

xcopy %SOURCE_PATH%\include %SOURCE_PATH%\release\%BUILD_NAME%\include /E /I
xcopy %SOURCE_PATH%\examples %SOURCE_PATH%\release\%BUILD_NAME%\examples /E /I

xcopy %SOURCE_PATH%\Detect-It-Easy\db %SOURCE_PATH%\release\%BUILD_NAME%\db /E /I

cd %SOURCE_PATH%\release
if exist %ZIP_NAME%.zip del %ZIP_NAME%.zip
%SEVENZIP_PATH%\7z.exe a %ZIP_NAME%.zip %BUILD_NAME%\*
rmdir /s /q %SOURCE_PATH%\release\%BUILD_NAME%
cd ..