;-------------------------------------------------------
; Include Modern UI
  !include "MUI2.nsh"
  !include "FileFunc.nsh"

;-------------------------------------------------------
; General

  ; Version information ...
  !define STR_VERSION "2.${VER_MINOR}-${DATESTRING}"

  ;Name and file
  Name "${APPNAME} ${STR_VERSION}"

  OutFile "${PACKAGES}\${APPNAME}-${STR_VERSION}-win-x86-setup.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\${APPNAME}"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${APPNAME}" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

  SetCompressor /FINAL /SOLID lzma

;-------------------------------------------------------
; Interface Settings
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP ${INSTBMP}
  !define MUI_ICON "..\resources\${INSTICON}"
  !define MUI_ABORTWARNING

  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY "Software\${APPNAME}"
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;-------------------------------------------------------
; what to run when finished ... ?
  !define MUI_FINISHPAGE_RUN "$INSTDIR\${BINNAME}"

;-------------------------------------------------------
; Pages
;  !insertmacro MUI_PAGE_WELCOME
;  !insertmacro MUI_PAGE_LICENSE "gpl-3.0.txt"
;  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;-------------------------------------------------------
; Languages
  !insertmacro MUI_LANGUAGE "Russian" ;first language is the default language
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_RESERVEFILE_LANGDLL

;-------------------------------------------------------
; Installer Sections for vlc-record
Section "VLC-Record" SecInst
  SectionIn RO
  SetOutPath "$INSTDIR"
  File "${SRCDIR}\release\${BINNAME}"
  File "${SRCDIR}\resources\${INSTICON}"
  File "${SRCDIR}\installer\shortcut.url"

  SetOutPath "$INSTDIR\language"
  File /r "${SRCDIR}\*.qm"

  SetOutPath "$INSTDIR\modules"
  File "${SRCDIR}\modules\1_vlc-player.mod"
  File "${SRCDIR}\modules\2_MPlayer.mod"
  File "${SRCDIR}\modules\3_vlc-mp4.mod"
  File "${SRCDIR}\modules\4_vlc-player-avi.mod"
  File "${SRCDIR}\modules\5_libvlc.mod"
  File "${SRCDIR}\modules\6_libvlc-mpeg2.mod"
  File "${SRCDIR}\modules\7_vlc-mpeg2.mod"
  File "${SRCDIR}\modules\8_libvlc_xvid_avi.mod"
  File "${SRCDIR}\modules\9_libvlc_odl.mod"
  File "${SRCDIR}\modules\10_vlc-player_odl.mod"
  File "${SRCDIR}\modules\11_libvlc-mp4.mod"
  File "${SRCDIR}\modules\12_libvlc_wingdi.mod"

  SetOutPath "$INSTDIR\doc"
  File /r "${SRCDIR}\qhc\${APPNAME}\*.qhc"
  File /r "${SRCDIR}\qhc\${APPNAME}\*.qch"

SectionEnd

;-------------------------------------------------------
; Installer Sections for libVLC
Section "libVLC Framework" SecFw
   SectionIn RO
   SetOutPath "$INSTDIR"
   File "${LIBVLCFW}\libvlc.dll"
   File "${LIBVLCFW}\libvlccore.dll"
;   File "${LIBVLCFW}\axvlc.dll"
;   File "${LIBVLCFW}\npvlc.dll"

; In case of update make sure we delete all old 
; VLC plugins!
   RMDir /r "$INSTDIR\plugins"

   SetOutPath "$INSTDIR\plugins"
   File /r "${LIBVLCFW}\plugins\*"
SectionEnd

;-------------------------------------------------------
; create batch file to create / update plugin cache
Section "PlugIn Cache Tools" SecCache
   SectionIn RO
   SetOutPath "$INSTDIR"
   FILE "${LIBVLCFW}\cache-gen.exe"

   FileOpen  $0 "$INSTDIR\clearcache.bat" w
   FileWrite $0 "@echo off$\r$\n"
   FileWrite $0 "echo Creating PlugIn Cache, please wait ... $\r$\n"
   FileWrite $0 '"$INSTDIR\cache-gen.exe" -f "$INSTDIR\plugins"$\r$\n'
   FileClose $0

   ExecWait '"$INSTDIR\clearcache.bat"'
SectionEnd


;-------------------------------------------------------
; Installer Sections for qt libraries
Section "qt Framework" SecQt
   SetOutPath "$INSTDIR"
   File "${QTLIBS}\*.dll"

   SetOutPath "$INSTDIR\imageformats"
   File /r "${QTLIBS}\imageformats\*.dll"

   SetOutPath "$INSTDIR\sqldrivers"
   File /r "${QTLIBS}\sqldrivers\*.dll"
   
   SetOutPath "$INSTDIR\platforms"
   File /r "${QTLIBS}\platforms\*.dll"

   SetOutPath "$INSTDIR\translations"
   File /r "${QTLIBS}\translations\*.qm"
SectionEnd

;-------------------------------------------------------
; start menu entries
Section "Start Menu Entries" SecStart
   CreateDirectory "$SMPROGRAMS\${APPNAME}"
   CreateShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${BINNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Clear Cache.lnk" "$INSTDIR\clearcache.bat"
   CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Check for new Version.lnk" "$INSTDIR\shortcut.url"
SectionEnd

;-------------------------------------------------------
; desktop shortcut ...
;Section /o "Desktop Shortcut" SecDesktop
Section "Desktop Shortcut" SecDesktop
   CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\${BINNAME}"
SectionEnd

;-------------------------------------------------------
; Installer Functions

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;-------------------------------------------------------
; write uninstall stuff ...
Section -FinishSection
  ; compute package size ...
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0

  ;store installation folder ...
  WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"

  ; create uninstall entries in registry ...
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$INSTDIR\${INSTICON}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "Jo2003"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "http://code.google.com/p/vlc-record/downloads/list"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "http://code.google.com/p/vlc-record/"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${STR_VERSION}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" "$0"

  ; write uninstaller ...
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

;-------------------------------------------------------
; Descriptions
;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecInst} "The vlc-record executable, the language files and player modules."
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecFw} "The libVLC framework. Only disable this section if you have already installed this framework or you want install it manually."
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecQt} "The Qt framework. Only disable this section if you have already installed the Qt framework and have set the QTDIR environment variable."
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecStart} "Creates a start menu entry for ${APPNAME}"
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Creates a desktop shortcut for ${APPNAME}"
;  !insertmacro MUI_DESCRIPTION_TEXT ${SecCache} "Install Plugin Cache Tools for ${APPNAME}"
;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-------------------------------------------------------
; Uninstaller Section framework ...
Section "un.Framework"
  ; delete vlc framework ...
  RMDir /r "$INSTDIR\plugins"
  Delete "$INSTDIR\libvlc.dll"
  Delete "$INSTDIR\libvlccore.dll"
;  Delete "$INSTDIR\axvlc.dll"
;  Delete "$INSTDIR\npvlc.dll"
  Delete "$INSTDIR\cache-gen.exe"
  Delete "$INSTDIR\clearcache.bat"


SectionEnd

;-------------------------------------------------------
; Uninstaller Section Qt ...
Section "un.Qt"
  ; delete Qt framework ...
  Delete "$INSTDIR\imageformats\*.*"
  Delete "$INSTDIR\sqldrivers\*.*"
  Delete "$INSTDIR\translations\*.*"
  Delete "$INSTDIR\platforms\*.*"
  Delete "$INSTDIR\*.dll"
  RMDir  "$INSTDIR\imageformats"
  RMDir  "$INSTDIR\sqldrivers"
  RMDir  "$INSTDIR\translations"
SectionEnd

;-------------------------------------------------------
; Uninstaller Section vlc-record ...
Section "un.Program"
  ; delete installed language files ...
  Delete "$INSTDIR\language\*.qm"
  Delete "$INSTDIR\doc\*.q??"


  ; delete installed module files ...
  Delete "$INSTDIR\modules\1_vlc-player.mod"
  Delete "$INSTDIR\modules\2_MPlayer.mod"
  Delete "$INSTDIR\modules\3_vlc-mp4.mod"
  Delete "$INSTDIR\modules\4_vlc-player-avi.mod"
  Delete "$INSTDIR\modules\5_libvlc.mod"
  Delete "$INSTDIR\modules\6_libvlc-mpeg2.mod"
  Delete "$INSTDIR\modules\7_vlc-mpeg2.mod"
  Delete "$INSTDIR\modules\8_libvlc_xvid_avi.mod"
  Delete "$INSTDIR\modules\9_libvlc_odl.mod"
  Delete "$INSTDIR\modules\10_vlc-player_odl.mod"
  Delete "$INSTDIR\modules\11_libvlc-mp4.mod"
  Delete "$INSTDIR\modules\12_libvlc_wingdi.mod"

  ; delete directories ...
  RMDir  "$INSTDIR\modules"
  RMDir  "$INSTDIR\language"
  RMDir  "$INSTDIR\doc"

  ; delete vlc-record itself ...
  Delete "$INSTDIR\${BINNAME}"
  Delete "$INSTDIR\${INSTICON}"
  Delete "$INSTDIR\shortcut.url"

SectionEnd

;-------------------------------------------------------
; Remove from registry...
Section "un.registry"
   DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
   DeleteRegKey HKLM "Software\${APPNAME}"
  DeleteRegKey HKCU "Software\${APPNAME}"
SectionEnd

;-------------------------------------------------------
; Delete Shortcuts
Section "un.Shortcuts"
  Delete "$DESKTOP\${APPNAME}.lnk"
  Delete "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk"
  Delete "$SMPROGRAMS\${APPNAME}\Clear Cache.lnk"
  Delete "$SMPROGRAMS\${APPNAME}\Check for new Version.lnk"
  Delete "$SMPROGRAMS\${APPNAME}\Uninstall.lnk"
  RMDir  "$SMPROGRAMS\${APPNAME}"
SectionEnd

;-------------------------------------------------------
; make final cleaning ...
Section "un.FinalCleaning"
  ; delete stored stuff ...
  RMDir /r /REBOOTOK "$APPDATA\${APPNAME}"

   ; delete uninstaller ...
  Delete "$INSTDIR\Uninstall.exe"

  ; delete install dir ...
   RMDir "$INSTDIR"
SectionEnd

;-------------------------------------------------------
; Uninstaller Functions
Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd
