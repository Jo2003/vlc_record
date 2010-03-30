;-------------------------------------------------------
; Include Modern UI

  !include "MUI2.nsh"

;-------------------------------------------------------
; Include defines ...
  !include "defines.nsh"

;-------------------------------------------------------
; General

  ;Name and file
  Name "${APPNAME} ${VER_INC}"
  OutFile "${PACKAGES}\${APPNAME}-${VER_INC}-win-x86-setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\${APPNAME}"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${APPNAME}" ""

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
  !define MUI_FINISHPAGE_RUN "$INSTDIR\vlc-record.exe"

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

  SetOutPath "$INSTDIR\language"
  File "${SRCDIR}\lang_de.qm"
  File "${SRCDIR}\lang_ru.qm"

  SetOutPath "$INSTDIR\modules"
  File "${SRCDIR}\modules\1_vlc-player.mod"
  File "${SRCDIR}\modules\2_MPlayer.mod"
  File "${SRCDIR}\modules\3_vlc-mp4.mod"
  File "${SRCDIR}\modules\4_vlc-player-avi.mod"
  File "${SRCDIR}\modules\5_libvlc.mod"
  File "${SRCDIR}\modules\6_libvlc-mpeg2.mod"
  File "${SRCDIR}\modules\7_vlc-mpeg2.mod"

SectionEnd

;-------------------------------------------------------
; Installer Sections for libVLC
Section "libVLC Framework" SecFw
   SetOutPath "$INSTDIR"
   File "${LIBVLCFW}\libvlc.dll"
   File "${LIBVLCFW}\libvlccore.dll"
   FILE "${LIBVLCFW}\libvlc.dll.manifest"
;   File "${LIBVLCFW}\axvlc.dll"
;   File "${LIBVLCFW}\npvlc.dll"

   SetOutPath "$INSTDIR\plugins"
   File /r "${LIBVLCFW}\plugins\*.dll"
SectionEnd

;-------------------------------------------------------
; start menu entries 
Section "Start Menu Entries" SecStart
	CreateDirectory "$SMPROGRAMS\${APPNAME}"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\vlc-record.exe"
	CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

;-------------------------------------------------------
; desktop shortcut ...
Section /o "Desktop Shortcut" SecDesktop
	CreateShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\vlc-record.exe"
SectionEnd

;-------------------------------------------------------
; write uninstall stuff ...
Section -FinishSection
  ;store installation folder ...
  WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	
  ; create uninstall entries in registry ...
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"

  ; write uninstaller ...
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

;-------------------------------------------------------
; Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecInst} "The vlc-record executable, the language files and player modules."
	!insertmacro MUI_DESCRIPTION_TEXT ${SecFw} "The libVLC framework. Only disable this section if you have already installed this framework or you want install it manually."
	!insertmacro MUI_DESCRIPTION_TEXT ${SecStart} "Creates a start menu entry for ${APPNAME}"
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Creates a desktop shortcut for ${APPNAME}"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-------------------------------------------------------
; Uninstaller Section framework ...
Section "un.Framework"
  ; delete vlc framework ...
  Delete "$INSTDIR\plugins\*.dll"
  Delete "$INSTDIR\libvlc.dll"
  Delete "$INSTDIR\libvlccore.dll"
  Delete "$INSTDIR\libvlc.dll.manifest"
;  Delete "$INSTDIR\axvlc.dll"
;  Delete "$INSTDIR\npvlc.dll"
  RMDir  "$INSTDIR\plugins"
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
  Delete "$INSTDIR\modules\5_libvlc.mod"
  Delete "$INSTDIR\modules\6_libvlc-mpeg2.mod"
  Delete "$INSTDIR\modules\7_vlc-mpeg2.mod"

  ; delete directories ...
  RMDir  "$INSTDIR\modules"
  RMDir  "$INSTDIR\language"

  ; delete vlc-record itself ...
  Delete "$INSTDIR\vlc-record.exe"

SectionEnd

;-------------------------------------------------------
; Remove from registry...
Section "un.registry"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"
SectionEnd

;-------------------------------------------------------
; Delete Shortcuts
Section "un.Shortcuts"
	Delete "$DESKTOP\${APPNAME}.lnk"
	Delete "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk"
	Delete "$SMPROGRAMS\${APPNAME}\Uninstall.lnk"
	RMDir  "$SMPROGRAMS\${APPNAME}"
SectionEnd

;-------------------------------------------------------
; make final cleaning ...
Section "un.FinalCleaning"
	; delete uninstaller ...
  Delete "$INSTDIR\Uninstall.exe"

  ; delete install dir ...
	RMDir "$INSTDIR"
SectionEnd
