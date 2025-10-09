;Installer script for Qmmp
;Based on script written by redxii (redxii@users.sourceforge.net)
;Tested/Developed with Unicode NSIS 2.46.5
 
;--------------------------------
;Disable description area

  !define MUI_COMPONENTSPAGE_NODESC
  
;-------------------------------
;Defines
 
   !define QMMP_VERSION "2.3.0"
   !define QMMP_PRODUCT_VERSION "${QMMP_VERSION}.0"
   !define WIN64 "1"
   !define QMMP_DEF_PROGS_KEY "Software\Clients\Media\Qmmp"
   !define QMMP_UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp"
   !define APP_DESCRIPTION $(text_app_desc)
   
     
;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  
;-------------------------------
;Includes
 
  !include WinVer.nsh
  !include x64.nsh

;--------------------------------
;General

  ;Name and file
  Name "Qt-based Multimedia Player ${QMMP_VERSION}"
!ifdef WIN64
  OutFile "qmmp-${QMMP_VERSION}-win64.exe"
!else
  OutFile "qmmp-${QMMP_VERSION}-win32.exe"
!endif

  ;Default installation folder
!ifdef WIN64
  InstallDir "$PROGRAMFILES64\Qt-based Multimedia Player"
!else
  InstallDir "$PROGRAMFILES\Qt-based Multimedia Player"
!endif
  
  ;Get installation folder from registry if available
  ;InstallDirRegKey HKCU "Software\Modern UI Test" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE ".\COPYING.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "Finnish"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Polish"  
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Ukrainian"
  !insertmacro MUI_LANGUAGE "TradChinese"
  
  
  !include "nsis-translations\English.nsh"
  !include "nsis-translations\Dutch.nsh"
  !include "nsis-translations\Finnish.nsh"
  !include "nsis-translations\French.nsh"
  !include "nsis-translations\Italian.nsh"
  !include "nsis-translations\Korean.nsh"
  !include "nsis-translations\Polish.nsh"
  !include "nsis-translations\Russian.nsh"
  !include "nsis-translations\Ukrainian.nsh"
  !include "nsis-translations\TradChinese.nsh"
  
;--------------------------------
;Macros

!macro MacroAllExtensions _action
  !insertmacro ${_action} ".669"
  !insertmacro ${_action} ".8svx"
  !insertmacro ${_action} ".ac3"
  !insertmacro ${_action} ".aif"
  !insertmacro ${_action} ".aiff"
  !insertmacro ${_action} ".amf"
  !insertmacro ${_action} ".ams"
  !insertmacro ${_action} ".ape"
  !insertmacro ${_action} ".au"
  !insertmacro ${_action} ".ay"
  !insertmacro ${_action} ".cue"
  !insertmacro ${_action} ".dbf"
  !insertmacro ${_action} ".dbm"
  !insertmacro ${_action} ".dmfumx"
  !insertmacro ${_action} ".dsm"
  !insertmacro ${_action} ".far"
  !insertmacro ${_action} ".flac"
  !insertmacro ${_action} ".ft2"
  !insertmacro ${_action} ".gms"
  !insertmacro ${_action} ".gym"
  !insertmacro ${_action} ".hes"
  !insertmacro ${_action} ".it"
  !insertmacro ${_action} ".itgz"
  !insertmacro ${_action} ".itr"
  !insertmacro ${_action} ".itz"
  !insertmacro ${_action} ".j2b"
  !insertmacro ${_action} ".kss"
  !insertmacro ${_action} ".m4a"
  !insertmacro ${_action} ".mdbz"
  !insertmacro ${_action} ".mdgz"
  !insertmacro ${_action} ".mdl"
  !insertmacro ${_action} ".mdr"
  !insertmacro ${_action} ".mdz"
  !insertmacro ${_action} ".mod"
  !insertmacro ${_action} ".mp1"
  !insertmacro ${_action} ".mp2"
  !insertmacro ${_action} ".mp3"
  !insertmacro ${_action} ".mpc"
  !insertmacro ${_action} ".mt2"
  !insertmacro ${_action} ".mtm"
  !insertmacro ${_action} ".mus"
  !insertmacro ${_action} ".nsf"
  !insertmacro ${_action} ".nsfe"
  !insertmacro ${_action} ".oga"
  !insertmacro ${_action} ".ogg"
  !insertmacro ${_action} ".opus"
  !insertmacro ${_action} ".P00"
  !insertmacro ${_action} ".prg"
  !insertmacro ${_action} ".psm"
  !insertmacro ${_action} ".ra"
  !insertmacro ${_action} ".s3gz"
  !insertmacro ${_action} ".s3m"
  !insertmacro ${_action} ".s3r"
  !insertmacro ${_action} ".sap"
  !insertmacro ${_action} ".sf"
  !insertmacro ${_action} ".shn"
  !insertmacro ${_action} ".sid"
  !insertmacro ${_action} ".snd"
  !insertmacro ${_action} ".spc"
  !insertmacro ${_action} ".sph"
  !insertmacro ${_action} ".stm"
  !insertmacro ${_action} ".str"
  !insertmacro ${_action} ".tta"
  !insertmacro ${_action} ".ult"
  !insertmacro ${_action} ".vgm"
  !insertmacro ${_action} ".vgz"
  !insertmacro ${_action} ".voc"
  !insertmacro ${_action} ".vqf"
  !insertmacro ${_action} ".w64"
  !insertmacro ${_action} ".wav"
  !insertmacro ${_action} ".wma"
  !insertmacro ${_action} ".wv"
  !insertmacro ${_action} ".xm"
  !insertmacro ${_action} ".xmgz"
  !insertmacro ${_action} ".xmr"
  !insertmacro ${_action} ".xmz"
  !insertmacro ${_action} ".au"
  !insertmacro ${_action} ".m4b"
!macroend

!macro WriteRegStrSupportedTypes EXT
  WriteRegStr HKLM  "${QMMP_DEF_PROGS_KEY}\Capabilities\FileAssociations" ${EXT} "QmmpFileAudio"
!macroend

;Check windows version

Function .onInit
  ${IfNot} ${AtLeastWin10}
    MessageBox MB_OK "$(text_win10_warning)"
    Quit
  ${EndIf}
!ifdef WIN64
  ${IfNot} ${RunningX64}
    MessageBox MB_OK|MB_ICONSTOP $(text_win64_warning)
    Quit
  ${EndIf}
!endif
FunctionEnd

;Metadata

VIProductVersion "${QMMP_PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Qmmp Development Team"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Qt-based Multimedia Player Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" $(QMMP_VERSION)
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C) 2006-2025 Qmmp Development Team"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Qmmp"

;--------------------------------

;Installer functions

Function RegisterDefaultPrograms
  WriteRegStr HKCR "QmmpFileAudio\DefaultIcon" "" '"$INSTDIR\bin\qmmp.exe",1'
  WriteRegStr HKCR "QmmpFileAudio\shell\enqueue" "" $(text_enqueue)
  WriteRegStr HKCR "QmmpFileAudio\shell\enqueue\command" "" '"$INSTDIR\bin\qmmp.exe" --enqueue "%1"'
  WriteRegStr HKCR "QmmpFileAudio\shell\open" "FriendlyAppName" $(text_app_full_name)
  WriteRegStr HKCR "QmmpFileAudio\shell\open\command" "" '"$INSTDIR\bin\qmmp.exe" "%1"'
  ;Modify the list of extensions added in the MacroAllExtensions macro
  WriteRegStr HKLM "${QMMP_DEF_PROGS_KEY}" "" "Qmmp"
  WriteRegStr HKLM "${QMMP_DEF_PROGS_KEY}\Capabilities" "ApplicationDescription" "${APP_DESCRIPTION}"
  WriteRegStr HKLM "${QMMP_DEF_PROGS_KEY}\Capabilities" "ApplicationName" "Qmmp"
  WriteRegStr HKLM "Software\RegisteredApplications" "Qmmp" "${QMMP_DEF_PROGS_KEY}\Capabilities"
  !insertmacro MacroAllExtensions WriteRegStrSupportedTypes
FunctionEnd


;Installer Sections

Section /o $(text_portable_configuration) PORTABLE
SectionEnd

Section "-General Section"

  SetOutPath "$INSTDIR"
  
  RMDir /r "$INSTDIR"
  
  ${If} ${SectionIsSelected} ${PORTABLE}
     FileOpen $0 "qmmp_portable.txt" w
     FileWrite $0 $(text_portable_warning)
     FileClose $0
  ${EndIf}
 
  SetOutPath "$INSTDIR\bin" 
  File /r bin\*

  SetOutPath "$INSTDIR\lib" 
  File /r lib\*.dll lib\*.qm

  SetOutPath "$INSTDIR\share\projectM"
  File /r share\projectM\*.inp share\projectM\*.ttf share\projectM\*.milk

  SetOutPath "$INSTDIR\share\qmmp"
  File /r share\qmmp\*
 
  SetOutPath "$INSTDIR\share\themes"
  File /r share\themes\* 
  
  ;ADD YOUR OWN FILES HERE...
  
  ;Store installation folder
  ;WriteRegStr HKCU "Software\Modern UI Test" "" $INSTDIR
  
  ;Create uninstaller
  ${IfNot} ${SectionIsSelected} ${PORTABLE}
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	; Write the uninstall keys for Windows
!ifdef WIN64
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayName" "Qt-based Multimedia Player (x64)"
!else
        WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayName" "Qt-based Multimedia Player"
!endif
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "UninstallString" "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayIcon" "$INSTDIR\qmmp.exe,0"
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayVersion" "${QMMP_VERSION}"
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "Publisher" "Qmmp Development Team"
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "InstallLocation" "$INSTDIR"
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "URLInfoAbout" "https://qmmp.ylsoftware.com"
	WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "URLUpdateInfo" "https://qmmp.ylsoftware.com"
	WriteRegDWORD HKLM ${QMMP_UNINSTALL_KEY} "NoModify" 1
	WriteRegDWORD HKLM ${QMMP_UNINSTALL_KEY} "NoRepair" 1
  
	;Default Programs Registration (Vista & later)

	${If} ${AtLeastWinVista}
		Call RegisterDefaultPrograms
	${EndIf}
  ${EndIf}
	
SectionEnd

Section $(text_extra_skins)
  SetOutPath "$INSTDIR\share\qmmp\skins"
  File /r share\qmmp\skins\*.txt share\qmmp\skins\*.png share\qmmp\skins\*.cur
SectionEnd

Section /o $(text_default_simple_ui)
  SetOutPath "$INSTDIR\share\qmmp"
  FileOpen $1 qmmp-default.ini a
  FileSeek $1 0 END
  FileWrite $1 "[Ui]$\r$\n"
  FileWrite $1 "current_plugin=qsui$\r$\n"
  FileClose $1
SectionEnd

Section /o $(text_enable_librcd)
  SetOutPath "$INSTDIR\share\qmmp"
  FileOpen $2 qmmp-default.ini a
  FileSeek $2 0 END
  FileWrite $2 "[MPEG]$\r$\n"
  FileWrite $2 "detect_encoding=true$\r$\n"
  FileWrite $2 "ID3v1_encoding=WINDOWS-1251$\r$\n"
  FileClose $2
SectionEnd

Section /o $(text_enable_adlib)
  SetOutPath "$INSTDIR\bin"
  File adplug\libbinio*.dll adplug\libadplug*.dll
  SetOutPath "$INSTDIR\lib\qmmp-2.3\Input" 
  File adplug\cas-adplug.dll
SectionEnd

Section $(text_startmenu_shortcuts) SHORTCUTS
  ${IfNot} ${SectionIsSelected} ${PORTABLE}
    SetShellVarContext all
    CreateDirectory "$SMPROGRAMS\Qt-based Multimedia Player"
    CreateShortCut "$SMPROGRAMS\Qt-based Multimedia Player\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
    CreateShortCut "$SMPROGRAMS\Qt-based Multimedia Player\Qmmp.lnk" "$INSTDIR\bin\qmmp.exe" "" "$INSTDIR\bin\qmmp.exe" 0
  ${EndIf}
SectionEnd

Function .onSelChange
  SectionGetFlags ${SHORTCUTS} $0
  ${If} ${SectionIsSelected} ${PORTABLE}
    IntOp $0 $0 | ${SF_RO}
    SectionSetFlags ${SHORTCUTS} $0
  ${Else}
    IntOp $1 ${SF_RO} ~ 
    IntOp $0 $0 & $1
    SectionSetFlags ${SHORTCUTS} $0     
  ${EndIf}
FunctionEnd


;--------------------------------
;Descriptions

  ;Language strings
  ;LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ;  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
    SetShellVarContext all

    ; Remove directories used
    RMDir /r "$SMPROGRAMS\Qt-based Multimedia Player"
    RMDir /r "$INSTDIR"

    Delete "$INSTDIR\Uninstall.exe"

    ; Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp"
    DeleteRegKey HKLM "SOFTWARE\Qmmp"
    DeleteRegKey HKLM "${QMMP_DEF_PROGS_KEY}"
    DeleteRegKey HKCR "QmmpFileAudio"
    DeleteRegValue HKLM "Software\RegisteredApplications" "Qmmp"
SectionEnd

