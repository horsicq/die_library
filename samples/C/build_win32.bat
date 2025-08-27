set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0"

call %VS_PATH%\VC\bin\vcvars32.bat

cl.exe /c simple.c /D_USING_V110_SDK71_ /GS- /Oi- /I "../../include"
link.exe simple.obj die.lib /SUBSYSTEM:CONSOLE,5.01 
del /s simple.obj