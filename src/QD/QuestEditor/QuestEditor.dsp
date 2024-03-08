# Microsoft Developer Studio Project File - Name="QuestEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=QuestEditor - Win32 Debug for QuestEditor
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuestEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuestEditor.mak" CFG="QuestEditor - Win32 Debug for QuestEditor"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QuestEditor - Win32 Debug for QuestEditor" (based on "Win32 (x86) Application")
!MESSAGE "QuestEditor - Win32 Release for QuestEditor" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/QUESTEDITOR", XULAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QuestEditor - Win32 Debug for QuestEditor"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "QuestEditor___Win32_Debug_for_QuestEditor"
# PROP BASE Intermediate_Dir "QuestEditor___Win32_Debug_for_QuestEditor"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\qdEngine\Source" /I "..\qdEngine\Parser" /I "..\qdEngine\xGR" /I "..\qdEngine" /I "..\qdEngine\AppCore" /I "..\qdEngine\Sound" /I "..\qdEngine\SoundDS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Core\qdCore\Util" /I "..\Core\qdCore" /I "..\Core\Parser" /I "..\Core\System\Graphics" /I "..\Core\System\Input" /I "..\Core\System" /I "..\Core\Runtime" /I "..\Core\System\Sound" /D "_DEBUG" /D "_GR_ENABLE_ZBUFFER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_QUEST_EDITOR" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xutilMT.lib xmathMT.lib Shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /pdbtype:sept
# ADD LINK32 xutilMT.lib xmathMT.lib Shlwapi.lib strmbase.lib dsound.lib winmm.lib libexpatd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /nodefaultlib:"WLDAP32.LIB" /pdbtype:sept

!ELSEIF  "$(CFG)" == "QuestEditor - Win32 Release for QuestEditor"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QuestEditor___Win32_Release_for_QuestEditor"
# PROP BASE Intermediate_Dir "QuestEditor___Win32_Release_for_QuestEditor"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\qdEngine\Source" /I "..\qdEngine\Parser" /I "..\qdEngine\xGR" /I "..\qdEngine" /I "..\qdEngine\AppCore" /I "..\qdEngine\Sound" /I "..\qdEngine\SoundDS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Core\qdCore\Util" /I "..\Core\qdCore" /I "..\Core\Parser" /I "..\Core\System\Graphics" /I "..\Core\System\Input" /I "..\Core\System" /I "..\Core\Runtime" /I "..\Core\System\Sound" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_QUEST_EDITOR" /D "_GR_ENABLE_ZBUFFER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xutilMT.lib xmathMT.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 xutilMT.lib xmathMT.lib Shlwapi.lib strmbase.lib dsound.lib winmm.lib libexpat.lib /nologo /subsystem:windows /debug /debugtype:coff /machine:I386 /nodefaultlib:"WLDAP32.LIB"

!ENDIF 

# Begin Target

# Name "QuestEditor - Win32 Debug for QuestEditor"
# Name "QuestEditor - Win32 Release for QuestEditor"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddCellsetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChoseName.cpp
# End Source File
# Begin Source File

SOURCE=.\CoordAnimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CurDirSafer.cpp
# End Source File
# Begin Source File

SOURCE=.\FileExtAssociator.cpp
# End Source File
# Begin Source File

SOURCE=.\shelltree\filename.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MWAAProgressPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MWAddAnimFirst.cpp
# End Source File
# Begin Source File

SOURCE=.\MWConvertWayPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MWGlobalSettingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\MWSelectExstFilePage.cpp
# End Source File
# Begin Source File

SOURCE=.\NewProjMultiWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\QuestEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\QuestEditor.rc
# End Source File
# Begin Source File

SOURCE=.\SceneList.cpp
# End Source File
# Begin Source File

SOURCE=.\SelAnimObj.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectDirs.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterClient.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\sv_api.cpp
# End Source File
# Begin Source File

SOURCE=.\wizards.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddAnimWizData.h
# End Source File
# Begin Source File

SOURCE=.\AddCellsetDlg.h
# End Source File
# Begin Source File

SOURCE=.\AnimObjBound.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\ChoseName.h
# End Source File
# Begin Source File

SOURCE=.\CoordAnimDlg.h
# End Source File
# Begin Source File

SOURCE=.\CurDirSafer.h
# End Source File
# Begin Source File

SOURCE=.\DispsListsMerger.h
# End Source File
# Begin Source File

SOURCE=.\FileExtAssociator.h
# End Source File
# Begin Source File

SOURCE=.\shelltree\filename.h
# End Source File
# Begin Source File

SOURCE=.\FileSelector.h
# End Source File
# Begin Source File

SOURCE=.\helpers.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MWAAProgressPage.h
# End Source File
# Begin Source File

SOURCE=.\MWAddAnimFirst.h
# End Source File
# Begin Source File

SOURCE=.\MWConvertWayPage.h
# End Source File
# Begin Source File

SOURCE=.\MWGlobalSettingPage.h
# End Source File
# Begin Source File

SOURCE=.\MWSelectExstFilePage.h
# End Source File
# Begin Source File

SOURCE=.\name_op.h
# End Source File
# Begin Source File

SOURCE=.\NewProjMultiWizard.h
# End Source File
# Begin Source File

SOURCE=.\ProgDlg.h
# End Source File
# Begin Source File

SOURCE=.\qdengine_frwrds.h
# End Source File
# Begin Source File

SOURCE=.\QuestEditor.h
# End Source File
# Begin Source File

SOURCE=.\RelativePath.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SceneList.h
# End Source File
# Begin Source File

SOURCE=.\SelAnimObj.h
# End Source File
# Begin Source File

SOURCE=.\SelectDirs.h
# End Source File
# Begin Source File

SOURCE=.\SplitterClient.h
# End Source File
# Begin Source File

SOURCE=.\state_guard.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\sv_api.h
# End Source File
# Begin Source File

SOURCE=.\user_finder.h
# End Source File
# Begin Source File

SOURCE=.\wizards.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\animated.bmp
# End Source File
# Begin Source File

SOURCE=.\arrow.cur
# End Source File
# Begin Source File

SOURCE=.\arrowcop.cur
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap13.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap14.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap15.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap17.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap18.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap19.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap20.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00008.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00009.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_item.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00004.cur
# End Source File
# Begin Source File

SOURCE=.\res\CUR_SIZE.CUR
# End Source File
# Begin Source File

SOURCE=.\res\cur_split.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\default.tga
# End Source File
# Begin Source File

SOURCE=.\res\dialog_s.bmp
# End Source File
# Begin Source File

SOURCE=.\res\draghand.cur
# End Source File
# Begin Source File

SOURCE=.\res\font00.idx
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico_ok.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\nodrop.cur
# End Source File
# Begin Source File

SOURCE=.\Pencil32z.cur
# End Source File
# Begin Source File

SOURCE=".\res\Quest editor1.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Quest editor1x.ico"
# End Source File
# Begin Source File

SOURCE=.\res\QuestEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\QuestEditor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\tga1.bin
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TrigEd.ico
# End Source File
# End Group
# Begin Group "Wizard"

# PROP Default_Filter "*.h,*.cpp"
# Begin Source File

SOURCE=.\AddAnimFirstWPg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddAnimFirstWPg.h
# End Source File
# Begin Source File

SOURCE=.\AddAnimStateWPg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddAnimStateWPg.h
# End Source File
# Begin Source File

SOURCE=.\AnimAddMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimAddMgr.h
# End Source File
# Begin Source File

SOURCE=.\WizardSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardSheet.h
# End Source File
# Begin Source File

SOURCE=.\WPgGlobSet.cpp
# End Source File
# Begin Source File

SOURCE=.\WPgGlobSet.h
# End Source File
# End Group
# Begin Group "PropertyPages"

# PROP Default_Filter "*.h, *.cpp"
# Begin Source File

SOURCE=.\AnimObjBound.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimObjStates.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimObjStates.h
# End Source File
# Begin Source File

SOURCE=.\BasePage.cpp
# End Source File
# Begin Source File

SOURCE=.\BasePage.h
# End Source File
# Begin Source File

SOURCE=.\BasePlanePage.cpp
# End Source File
# Begin Source File

SOURCE=.\BasePlanePage.h
# End Source File
# Begin Source File

SOURCE=.\CharacterPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\characterPropPage.h
# End Source File
# Begin Source File

SOURCE=.\CharBound.cpp
# End Source File
# Begin Source File

SOURCE=.\CharBound.h
# End Source File
# Begin Source File

SOURCE=.\InventoryPP.cpp
# End Source File
# Begin Source File

SOURCE=.\InventoryPP.h
# End Source File
# Begin Source File

SOURCE=.\InventoryPPForPers.cpp
# End Source File
# Begin Source File

SOURCE=.\InventoryPPForPers.h
# End Source File
# Begin Source File

SOURCE=.\MyPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MyPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\ObjPosPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjPosPropPage.h
# End Source File
# Begin Source File

SOURCE=.\ParalaxPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ParalaxPage.h
# End Source File
# Begin Source File

SOURCE=.\PPNotification.h
# End Source File
# Begin Source File

SOURCE=.\ScreenProp.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenProp.h
# End Source File
# Begin Source File

SOURCE=.\StaticObjPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StaticObjPage.h
# End Source File
# Begin Source File

SOURCE=.\ZonePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ZonePropPage.h
# End Source File
# End Group
# Begin Group "Условия"

# PROP Default_Filter "*.h, *.cpp"
# Begin Source File

SOURCE=.\CondDlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CondDlgBase.h
# End Source File
# Begin Source File

SOURCE=.\condition.h
# End Source File
# Begin Source File

SOURCE=.\MouseClickCond.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseClickCond.h
# End Source File
# Begin Source File

SOURCE=.\MouseDlgClickCond.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseDlgClickCond.h
# End Source File
# Begin Source File

SOURCE=.\MouseObjClickCond.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseObjClickCond.h
# End Source File
# Begin Source File

SOURCE=.\MovingOnNetCond.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingOnNetCond.h
# End Source File
# Begin Source File

SOURCE=.\ObjOnNetCond.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjOnNetCond.h
# End Source File
# Begin Source File

SOURCE=.\StateCondDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StateCondDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimerCond.cpp
# End Source File
# Begin Source File

SOURCE=.\TimerCond.h
# End Source File
# Begin Source File

SOURCE=.\WalkStopDirCond.cpp
# End Source File
# Begin Source File

SOURCE=.\WalkStopDirCond.h
# End Source File
# End Group
# Begin Group "Переписанные контролы"

# PROP Default_Filter "*.h, *.cpp"
# Begin Group "ShellTree"

# PROP Default_Filter "*.cpp, *.h"
# Begin Source File

SOURCE=.\shelltree\shellpidl.cpp
# End Source File
# Begin Source File

SOURCE=.\shelltree\shellpidl.h
# End Source File
# Begin Source File

SOURCE=.\shelltree\shelltree.cpp
# End Source File
# Begin Source File

SOURCE=.\shelltree\shelltree.h
# End Source File
# End Group
# Begin Group "Выбор изображения"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SelectPicture\ImageList.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPicture\ImageList.h
# End Source File
# Begin Source File

SOURCE=.\SelectPicture\SelectPicture.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPicture\SelectPicture.h
# End Source File
# Begin Source File

SOURCE=.\SelectPicture\SPMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPicture\SPMenu.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CoolDialogBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolDialogBar.h
# End Source File
# Begin Source File

SOURCE=.\editlistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\editlistctrl.h
# End Source File
# Begin Source File

SOURCE=.\MyTree.cpp
# End Source File
# Begin Source File

SOURCE=.\MyTree.h
# End Source File
# End Group
# Begin Group "Custom Iterators"

# PROP Default_Filter "*.cpp, *.h"
# Begin Source File

SOURCE=.\dfn_iterator.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrl_insert_iterator.h
# End Source File
# End Group
# Begin Group "Собственность Тимофея"

# PROP Default_Filter "*.h, *.cpp"
# Begin Source File

SOURCE=.\auto_object.h
# End Source File
# Begin Source File

SOURCE=.\auto_parray.h
# End Source File
# Begin Source File

SOURCE=.\object_eraser.h
# End Source File
# End Group
# Begin Group "Настройки приложения"

# PROP Default_Filter "*.h,*.cpp"
# Begin Source File

SOURCE=.\AppStngsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AppStngsDlg.h
# End Source File
# Begin Source File

SOURCE=.\GenStngsPP.cpp
# End Source File
# Begin Source File

SOURCE=.\GenStngsPP.h
# End Source File
# Begin Source File

SOURCE=.\reg_setting.cpp
# End Source File
# Begin Source File

SOURCE=.\reg_setting.h
# End Source File
# Begin Source File

SOURCE=.\Setting.cpp
# End Source File
# Begin Source File

SOURCE=.\Setting.h
# End Source File
# Begin Source File

SOURCE=.\SettingDefs.h
# End Source File
# Begin Source File

SOURCE=.\TESettings.cpp
# End Source File
# Begin Source File

SOURCE=.\TESettings.h
# End Source File
# End Group
# Begin Group "Редактирование состояний"

# PROP Default_Filter "*.h;*.cpp"
# Begin Source File

SOURCE=.\EditStateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditStateDlg.h
# End Source File
# Begin Source File

SOURCE=.\StateBoundDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StateBoundDlg.h
# End Source File
# Begin Source File

SOURCE=.\StateCoordsStacker.cpp
# End Source File
# Begin Source File

SOURCE=.\StateCoordsStacker.h
# End Source File
# End Group
# Begin Group "Undo"

# PROP Default_Filter "*.h,*.cpp"
# Begin Source File

SOURCE=.\ube_obj_pos.h
# End Source File
# Begin Source File

SOURCE=.\UBEntry.h
# End Source File
# Begin Source File

SOURCE=.\UndoBufferMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoBufferMgr.h
# End Source File
# End Group
# Begin Group "Новый редактор триггеров"

# PROP Default_Filter "*.h;*.cpp;"
# Begin Source File

SOURCE=.\ITriggerView.h
# End Source File
# Begin Source File

SOURCE=.\te_filter_base.h
# End Source File
# Begin Source File

SOURCE=.\te_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\te_helpers.h
# End Source File
# Begin Source File

SOURCE=.\TEWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TEWnd.h
# End Source File
# Begin Source File

SOURCE=.\TEWnd_Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\TEWnd_Scale_Scrl.cpp
# End Source File
# End Group
# Begin Group "Дерево проекта"

# PROP Default_Filter "*.cpp,*.h,*.hpp"
# Begin Source File

SOURCE=.\ptree_add.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_api.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_api.h
# End Source File
# Begin Source File

SOURCE=.\ptree_dg_dp.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_hlprs.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_hlprs.hpp
# End Source File
# Begin Source File

SOURCE=.\ptree_loading.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_loading.h
# End Source File
# Begin Source File

SOURCE=.\ptree_menu.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_remove.cpp
# End Source File
# Begin Source File

SOURCE=.\ptree_te_filters.h
# End Source File
# Begin Source File

SOURCE=.\SceneTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneTreeView.h
# End Source File
# End Group
# Begin Group "Редактирование походок"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MoveStopDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveStopDlg.h
# End Source File
# Begin Source File

SOURCE=.\MovingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingDlg.h
# End Source File
# End Group
# Begin Group "Каталоги"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimCatalogDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimCatalogDlg.h
# End Source File
# Begin Source File

SOURCE=.\SoundCatalogDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundCatalogDlg.h
# End Source File
# End Group
# Begin Group "MultiWizard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MultiWizard\MultiWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiWizard\MultiWizard.h
# End Source File
# Begin Source File

SOURCE=.\MultiWizard\MWBasePage.cpp
# End Source File
# Begin Source File

SOURCE=.\MultiWizard\MWBasePage.h
# End Source File
# End Group
# Begin Group "Новые визарды"

# PROP Default_Filter ""
# Begin Group "Настройки игры"

# PROP Default_Filter ""
# End Group
# Begin Group "Добавление фона"

# PROP Default_Filter ""
# End Group
# Begin Group "Добавление анимации"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\mwdatabase.h
# End Source File
# Begin Source File

SOURCE=.\WizardBasePage.cpp
# End Source File
# Begin Source File

SOURCE=.\WizardBasePage.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\font00.tga
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
