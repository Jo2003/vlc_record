@echo off
set QTDIR=c:\Qt\4.8.3
set PATH=c:\Qt\mingw\bin;c:\Program Files\NSIS;%QTDIR%\bin;%PATH%
FOR %%B IN (vlc-record kartina_tv polsky_tv moidom_tv) DO qmake.exe %%B.pro && mingw32-make -s clean && mingw32-make -s -j2 release && cd installer && makensis %%B.nsi && cd .. 
