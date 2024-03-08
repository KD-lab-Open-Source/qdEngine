# Microsoft Developer Studio Generated NMAKE File, Based on TreeListCtrl.dsp
!IF "$(CFG)" == ""
CFG=TreeListCtrl - Win32 Debug Shared MT
!MESSAGE No configuration specified. Defaulting to TreeListCtrl - Win32 Debug Shared MT.
!ENDIF 

!IF "$(CFG)" != "TreeListCtrl - Win32 Release MT" && "$(CFG)" != "TreeListCtrl - Win32 Debug Static MT" && "$(CFG)" != "TreeListCtrl - Win32 Release Static MT" && "$(CFG)" != "TreeListCtrl - Win32 VC6 Debug Single Thread" && "$(CFG)" != "TreeListCtrl - Win32 VC6 Debug Static SingleThread" && "$(CFG)" != "TreeListCtrl - Win32 VC6 Release SingleThread" && "$(CFG)" != "TreeListCtrl - Win32 VC6 Release Static SingleThread" && "$(CFG)" != "TreeListCtrl - Win32 Debug Shared MT"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TreeListCtrl.mak" CFG="TreeListCtrl - Win32 Debug Shared MT"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TreeListCtrl - Win32 Release MT" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 Debug Static MT" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 Release Static MT" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Debug Single Thread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Debug Static SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Release SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Release Static SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 Debug Shared MT" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TreeListCtrl - Win32 Release MT"

OUTDIR=.\VC6_Release_MT
INTDIR=.\VC6_Release_MT

ALL : ".\Output\xTreeListCtrlVC6_SH_MT.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Output\xTreeListCtrlVC6_SH_MT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_SH_MT.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_SH_MT.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Debug Static MT"

OUTDIR=.\VC6_DebugStatic
INTDIR=.\VC6_DebugStatic

ALL : ".\Output\xTreeListCtrlVC6_ST_MT_D.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\Output\xTreeListCtrlVC6_ST_MT_D.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_ST_MT_D.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_ST_MT_D.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Release Static MT"

OUTDIR=.\VC6_Release Static MT
INTDIR=.\VC6_Release Static MT

ALL : ".\Output\xTreeListCtrlVC6_ST_MT.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Output\xTreeListCtrlVC6_ST_MT.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_ST_MT.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_ST_MT.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Single Thread"

OUTDIR=.\VC6 Debug Single Thread
INTDIR=.\VC6 Debug Single Thread

ALL : ".\Output\xTreeListCtrlVC6_SH_D.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\Output\xTreeListCtrlVC6_SH_D.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_SH_D.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_SH_D.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Static SingleThread"

OUTDIR=.\VC6 Debug Static SingleThread
INTDIR=.\VC6 Debug Static SingleThread

ALL : ".\Output\xTreeListCtrlVC6_ST_D.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\Output\xTreeListCtrlVC6_ST_D.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_ST_D.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_ST_D.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Release SingleThread"

OUTDIR=.\VC6 Release SingleThread
INTDIR=.\VC6 Release SingleThread

ALL : ".\Output\xTreeListCtrlVC6_SH.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Output\xTreeListCtrlVC6_SH.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_SH.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_SH.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Release Static SingleThread"

OUTDIR=.\VC6 Release Static SingleThread
INTDIR=.\VC6 Release Static SingleThread

ALL : ".\Output\xTreeListCtrlVC6_ST.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Output\xTreeListCtrlVC6_ST.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_ST.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_ST.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Debug Shared MT"

OUTDIR=.\Debug_Shared_MT
INTDIR=.\Debug_Shared_MT

ALL : ".\Output\xTreeListCtrlVC6_SH_MT_D.lib"


CLEAN :
	-@erase "$(INTDIR)\NullCustomDrawNotifyListener.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TLCDragWnd.obj"
	-@erase "$(INTDIR)\TLCDropWnd.obj"
	-@erase "$(INTDIR)\TLHDragWnd.obj"
	-@erase "$(INTDIR)\TLHDropWnd.obj"
	-@erase "$(INTDIR)\TreeListColumnInfo.obj"
	-@erase "$(INTDIR)\TreeListComboCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.obj"
	-@erase "$(INTDIR)\TreeListCtrl.pch"
	-@erase "$(INTDIR)\TreeListCtrl.res"
	-@erase "$(INTDIR)\TreeListDC.obj"
	-@erase "$(INTDIR)\TreeListEditCtrl.obj"
	-@erase "$(INTDIR)\TreeListHeaderCtrl.obj"
	-@erase "$(INTDIR)\TreeListItem.obj"
	-@erase "$(INTDIR)\TreeListStaticCtrl.obj"
	-@erase "$(INTDIR)\TreeListTipCtrl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\Output\xTreeListCtrlVC6_SH_MT_D.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\TreeListCtrl.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TreeListCtrl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"Output\xTreeListCtrlVC6_SH_MT_D.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TLCDragWnd.obj" \
	"$(INTDIR)\TLCDropWnd.obj" \
	"$(INTDIR)\TLHDragWnd.obj" \
	"$(INTDIR)\TLHDropWnd.obj" \
	"$(INTDIR)\TreeListColumnInfo.obj" \
	"$(INTDIR)\TreeListComboCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.obj" \
	"$(INTDIR)\TreeListDC.obj" \
	"$(INTDIR)\TreeListEditCtrl.obj" \
	"$(INTDIR)\TreeListHeaderCtrl.obj" \
	"$(INTDIR)\TreeListItem.obj" \
	"$(INTDIR)\TreeListStaticCtrl.obj" \
	"$(INTDIR)\TreeListTipCtrl.obj" \
	"$(INTDIR)\TreeListCtrl.res" \
	"$(INTDIR)\NullCustomDrawNotifyListener.obj"

".\Output\xTreeListCtrlVC6_SH_MT_D.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("TreeListCtrl.dep")
!INCLUDE "TreeListCtrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "TreeListCtrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TreeListCtrl - Win32 Release MT" || "$(CFG)" == "TreeListCtrl - Win32 Debug Static MT" || "$(CFG)" == "TreeListCtrl - Win32 Release Static MT" || "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Single Thread" || "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Static SingleThread" || "$(CFG)" == "TreeListCtrl - Win32 VC6 Release SingleThread" || "$(CFG)" == "TreeListCtrl - Win32 VC6 Release Static SingleThread" || "$(CFG)" == "TreeListCtrl - Win32 Debug Shared MT"
SOURCE=.\NullCustomDrawNotifyListener.cpp

"$(INTDIR)\NullCustomDrawNotifyListener.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "TreeListCtrl - Win32 Release MT"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Debug Static MT"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Release Static MT"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Single Thread"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Static SingleThread"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Release SingleThread"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Release Static SingleThread"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Debug Shared MT"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\TreeListCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\TreeListCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TLCDragWnd.cpp

"$(INTDIR)\TLCDragWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TLCDropWnd.cpp

"$(INTDIR)\TLCDropWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TLHDragWnd.cpp

"$(INTDIR)\TLHDragWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TLHDropWnd.cpp

"$(INTDIR)\TLHDropWnd.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListColumnInfo.cpp

"$(INTDIR)\TreeListColumnInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListComboCtrl.cpp

"$(INTDIR)\TreeListComboCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListCtrl.cpp

"$(INTDIR)\TreeListCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListCtrl.rc

"$(INTDIR)\TreeListCtrl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\TreeListDC.cpp

"$(INTDIR)\TreeListDC.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListEditCtrl.cpp

"$(INTDIR)\TreeListEditCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListHeaderCtrl.cpp

"$(INTDIR)\TreeListHeaderCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListItem.cpp

"$(INTDIR)\TreeListItem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListStaticCtrl.cpp

"$(INTDIR)\TreeListStaticCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"


SOURCE=.\TreeListTipCtrl.cpp

"$(INTDIR)\TreeListTipCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\TreeListCtrl.pch"



!ENDIF 

