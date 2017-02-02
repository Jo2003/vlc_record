@echo off

rem Please insert the needed pathes here ...
set QTDIR=c:\Qt\4.8.7
set SEVENZIP=c:\Program Files\7-Zip
set MINGW=c:\Qt\mingw_4.8.0\mingw32\bin
set NSIS=c:\Program Files\NSIS

rem nothing to change below this line ...
rem ########################################

set PATH=%MINGW%\bin;%NSIS%;%QTDIR%\bin;%SEVENZIP%;%PATH%

rem clean old releases ...
cd packages && del *.* && cd ..

rem build all packages ...
FOR %%B IN (tvclub) DO qmake.exe %%B.pro && make -s clean && make -s -j3 release && cd installer && makensis %%B.nsi && cd ..

rem 7zip packages ...
cd packages
FOR %%B IN (*.exe) DO 7z a %%~nB.zip %%B
cd ..
