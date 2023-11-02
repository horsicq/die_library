set MINGW_PATH="C:\Qt\Tools\mingw810_32\bin"
set QMAKE_PATH="C:\Qt\Static\5.15.2\bin\qmake.exe"
set SEVENZIP_PATH="C:\Program Files\7-Zip\7z.exe"

set X_SOURCE_PATH=%~dp0
set X_BUILD_NAME=dielib_static
set X_BUILD_PREFIX=win32
set /p X_RELEASE_VERSION=<%X_SOURCE_PATH%\Detect-It-Easy\die_version.txt

call %X_SOURCE_PATH%\build_tools\windows.cmd make_init

IF NOT [%X_ERROR%] == [] goto exit

call %X_SOURCE_PATH%\build_tools\windows.cmd make_build %X_SOURCE_PATH%\die_library.pro

call %X_SOURCE_PATH%\build_tools\windows.cmd check_file %X_SOURCE_PATH%\build\release\die.dll

IF NOT [%X_ERROR%] == [] goto exit

copy %X_SOURCE_PATH%\build\release\die.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\

xcopy %X_SOURCE_PATH%\Detect-It-Easy\db %X_SOURCE_PATH%\release\%X_BUILD_NAME%\db /E /I
xcopy %X_SOURCE_PATH%\include %X_SOURCE_PATH%\release\%X_BUILD_NAME%/E /I

call %X_SOURCE_PATH%\build_tools\windows.cmd make_release

:exit
call %X_SOURCE_PATH%\build_tools\windows.cmd make_clear
