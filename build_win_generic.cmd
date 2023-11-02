call %X_SOURCE_PATH%\build_tools\windows.cmd make_init

IF NOT [%X_ERROR%] == [] goto exit

call %X_SOURCE_PATH%\build_tools\windows.cmd make_build %X_SOURCE_PATH%\die_library.pro

call %X_SOURCE_PATH%\build_tools\windows.cmd check_file %X_SOURCE_PATH%\build\release\die.dll

IF NOT [%X_ERROR%] == [] goto exit

copy %X_SOURCE_PATH%\build\release\die.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
copy %X_SOURCE_PATH%\build\release\die.exp %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
copy %X_SOURCE_PATH%\build\release\die.lib %X_SOURCE_PATH%\release\%X_BUILD_NAME%\

xcopy %X_SOURCE_PATH%\Detect-It-Easy\db %X_SOURCE_PATH%\release\%X_BUILD_NAME%\db /E /I
xcopy %X_SOURCE_PATH%\include %X_SOURCE_PATH%\release\%X_BUILD_NAME%/E /I

call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_qt_library Qt5Core
call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_qt_library Qt5Script

call %X_SOURCE_PATH%\build_tools\windows.cmd make_release

:exit
call %X_SOURCE_PATH%\build_tools\windows.cmd make_clear
