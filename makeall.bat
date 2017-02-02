@echo off

rem Please insert the needed pathes here ...
set QTDIR=c:\Qt\4.8.7
set SEVENZIP=c:\Program Files\7-Zip
set MINGW=c:\Qt\mingw_4.8.0\mingw32
set NSIS=c:\Program Files\NSIS\Unicode

rem nothing to change below this line ...
rem ########################################

set PATH=%MINGW%\bin;%NSIS%;%QTDIR%\bin;%SEVENZIP%;%PATH%

:: clean old releases ...
cd packages && del *.* && cd ..

:: build all packages ...
FOR %%B IN (tvclub) DO (
    qmake.exe %%B.pro
    make clean
    make -j3 release
    cd installer
    makensis %%B.nsi
    cd ..\packages
    FOR %%X IN (%%B*.exe) DO 7z a %%X.zip %%X
    cd ..
)
