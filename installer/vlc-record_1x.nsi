;-------------------------------------------------------
; Include Modern UI

  !include "MUI2.nsh"
  !include "FileFunc.nsh"

;-------------------------------------------------------
; Include defines ...
  !include "defines.nsh"

;-------------------------------------------------------
; General

  ; Version information ...
  !define STR_VERSION "1.${VER_MINOR}-${DATESTRING}"
  
  ;Name and file
  Name "${APPNAME} Classic ${STR_VERSION}"
  OutFile "${PACKAGES}\${APPNAME}-${STR_VERSION}-win-x86-setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\${APPNAME} Classic"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${APPNAME} Classic" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

  SetCompressor /FINAL /SOLID lzma

;-------------------------------------------------------
; Interface Settings
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" ; optional
  !define MUI_ABORTWARNING

;-------------------------------------------------------
; what to run when finished ... ?
  !define MUI_FINISHPAGE_RUN "$INSTDIR\vlc-record-classic.exe"

;-------------------------------------------------------
; Pages
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "gpl-3.0.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;-------------------------------------------------------
; Languages
 
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_RESERVEFILE_LANGDLL

;-------------------------------------------------------
; Installer Sections for vlc-record
Section "VLC-Record" SecInst
  SectionIn RO
  SetOutPath "$INSTDIR"
  File "${SRCDIR}\release\vlc-record.exe"
  File "${SRCDIR}\resources\television.ico"
  File "${SRCDIR}\installer\shortcut.url"
  File "${QTLIBS}\libgcc_s_dw2-1.dll"
  File "${QTLIBS}\mingwm10.dll"
  Rename vlc-record.exe vlc-record-classic.exe

  SetOutPath "$INSTDIR\language"
  File "${SRCDIR}\lang_de.qm"
  File "${SRCDIR}\lang_ru.qm"

  SetOutPath "$INSTDIR\modules"
  File "${SRCDIR}\modules\1_vlc-player.mod"
  File "${SRCDIR}\modules\2_MPlayer.mod"
  File "${SRCDIR}\modules\3_vlc-mp4.mod"
  File "${SRCDIR}\modules\4_vlc-player-avi.mod"
  File "${SRCDIR}\modules\7_vlc-mpeg2.mod"
  File "${SRCDIR}\modules\10_vlc-player_odl.mod"

SectionEnd

;-------------------------------------------------------
; Installer Sections for qt libraries
Section "qt Framework" SecQt
   SetOutPath "$INSTDIR"
   File "${QTLIBS}\QtCore4.dll"
   File "${QTLIBS}\QtSql4.dll"
   FILE "${QTLIBS}\QtGui4.dll"
   FILE "${QTLIBS}\QtNetwork4.dll"
   FILE "${QTLIBS}\QtXml4.dll"

   SetOutPath "$INSTDIR\imageformats"
   File /r "${QTLIBS}\imageformats\*.dll"

   SetOutPath "$INSTDIR\sqldrivers"
   File /r "${QTLIBS}\sqldrivers\*.dll"
SectionEnd

;-------------------------------------------------------
; start menu entries 
Section "Start Menu Entries" SecStart
	CreateDirectory "$SMPROGRAMS\${APPNAME} Classic"
	CreateShortCut "$SMPROGRAMS\${APPNAME} Classic\${APPNAME} Classic.lnk" "$INSTDIR\vlc-record-classic.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME} Classic\Uninstall.lnk" "$INSTDIR\uninstall.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME} Classic\Check for new Version.lnk" "$INSTDIR\shortcut.url"
SectionEnd

;-------------------------------------------------------
; desktop shortcut ...
;Section /o "Desktop Shortcut" SecDesktop
Section "Desktop Shortcut" SecDesktop
	CreateShortCut "$DESKTOP\${APPNAME} Classic.lnk" "$INSTDIR\vlc-record-classic.exe"
SectionEnd

;-------------------------------------------------------
; write uninstall stuff ...
Section -FinishSection
  ; compute package size ...
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0

  ;store installation folder ...
  WriteRegStr HKLM "Software\${APPNAME} Classic" "" "$INSTDIR"
	
  ; create uninstall entries in registry ...
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "DisplayName" "${APPNAME} Classic"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "DisplayIcon" "$INSTDIR\television.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "Publisher" "Jo2003"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "URLUpdateInfo" "http://code.google.com/p/vlc-record/downloads/list"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "URLInfoAbout" "http://code.google.com/p/vlc-record/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "DisplayVersion" "${STR_VERSION}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic" "EstimatedSize" "$0"

  ; write uninstaller ...
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

;-------------------------------------------------------
; Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecInst} "The vlc-record executable, the language files and player modules."
	!insertmacro MUI_DESCRIPTION_TEXT ${SecQt} "The Qt framework. Only disable this section if you have already installed the Qt framework and have set the QTDIR environment variable."
	!insertmacro MUI_DESCRIPTION_TEXT ${SecStart} "Creates a start menu entry for ${APPNAME}"
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Creates a desktop shortcut for ${APPNAME}"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-------------------------------------------------------
; Uninstaller Section Qt ...
Section "un.Qt"
  ; delete Qt framework ...
  Delete "$INSTDIR\imageformats\*.*"
  Delete "$INSTDIR\sqldrivers\*.*"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtSql4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtXml4.dll"
  RMDir  "$INSTDIR\imageformats"
  RMDir  "$INSTDIR\sqldrivers"
SectionEnd

;-------------------------------------------------------
; Uninstaller Section vlc-record ...
Section "un.Program"
  ; delete installed language files ...
  Delete "$INSTDIR\language\lang_de.qm"
  Delete "$INSTDIR\language\lang_ru.qm"

  ; delete installed module files ...
  Delete "$INSTDIR\modules\1_vlc-player.mod"
  Delete "$INSTDIR\modules\2_MPlayer.mod"
  Delete "$INSTDIR\modules\3_vlc-mp4.mod"
  Delete "$INSTDIR\modules\4_vlc-player-avi.mod"
  Delete "$INSTDIR\modules\7_vlc-mpeg2.mod"
  Delete "$INSTDIR\modules\10_vlc-player_odl.mod"

  ; delete directories ...
  RMDir  "$INSTDIR\modules"
  RMDir  "$INSTDIR\language"

  ; delete vlc-record itself ...
  Delete "$INSTDIR\vlc-record-classic.exe"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\television.ico"
  Delete "$INSTDIR\shortcut.url"

SectionEnd

;-------------------------------------------------------
; Remove from registry...
Section "un.registry"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME} Classic"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME} Classic"
SectionEnd

;-------------------------------------------------------
; Delete Shortcuts
Section "un.Shortcuts"
	Delete "$DESKTOP\${APPNAME} Classic.lnk"
	Delete "$SMPROGRAMS\${APPNAME} Classic\${APPNAME} Classic.lnk"
	Delete "$SMPROGRAMS\${APPNAME} Classic\Check for new Version.lnk"
	Delete "$SMPROGRAMS\${APPNAME} Classic\Uninstall.lnk"
	RMDir  "$SMPROGRAMS\${APPNAME} Classic"
SectionEnd

;-------------------------------------------------------
; make final cleaning ...
Section "un.FinalCleaning"
	; delete uninstaller ...
  Delete "$INSTDIR\Uninstall.exe"

  ; delete install dir ...
	RMDir "$INSTDIR"
SectionEnd
