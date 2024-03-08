@echo off
attrib "F:\xLibs.OUT\TreeListCtrl\*.*" -R /s
xcopy NullCustomDrawNotifyListener.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy I*Listener.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListColumnInfo.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListComboCtrl.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListCtrl.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListDC.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListEditCtrl.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListHeaderCtrl.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListItem.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListStaticCtrl.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy TreeListTipCtrl.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy EditItemCtrlBase.h "F:\xLibs.OUT\TreeListCtrl\" /Y /C
xcopy "Null Listeners\*.h" "F:\xLibs.OUT\TreeListCtrl\Null Listeners\" /Y /C
attrib "F:\xLibs.OUT\TreeListCtrl\*.*" +R /s