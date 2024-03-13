call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" x86
cd ..
mkdir tmp_build
cd Release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\5.15.2\msvc2019" -G "NMake Makefiles" ..
nmake
cpack -G ZIP
xcopy /y /E packages/ ../

cd ..