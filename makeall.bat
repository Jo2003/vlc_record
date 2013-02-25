@echo off

rem Please insert the needed pathes here ...
set QTDIR=c:\Qt\4.8.3
set SEVENZIP=c:\Program Files\7-Zip
set MINGW=c:\Qt\mingw
set NSIS=c:\Program Files\NSIS

rem nothing to change below this line ...
rem ########################################

set PATH=%MINGW%\bin;%NSIS%;%QTDIR%\bin;%SEVENZIP%;%PATH%

rem clean old releases ...
cd packages && del *.* && cd ..

rem build all packages ...
FOR %%B IN (vlc-record kartina_tv polsky_tv moidom_tv) DO qmake.exe %%B.pro && mingw32-make -s clean && mingw32-make -s -j2 release && cd installer && makensis %%B.nsi && cd .. 

rem 7zip packages ...
cd packages
FOR %%B IN (*.exe) DO 7z a %%~nB.7z %%B
cd ..
