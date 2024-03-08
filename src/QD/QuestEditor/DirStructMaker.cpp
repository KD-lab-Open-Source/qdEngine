#include "stdafx.h"
#include "resource.h"
#include "dirstructmaker.h"
#include <string>

DirStructMaker::DirStructMaker(IShowProgress* psp):m_ptrShowProgress(psp)
{
}

DirStructMaker::~DirStructMaker(void)
{
}

bool DirStructMaker::make(LPCTSTR lpszDir, int scenes_count)
{
	if(!make_resource_dir_struct(lpszDir, scenes_count))
		return false;
	if (!make_materials_dir_struct(lpszDir, scenes_count))
		return false;
	return true;
}

bool DirStructMaker::make_dir(LPCTSTR lpszDirName){
	BOOL res = CreateDirectory(lpszDirName, NULL);
	if (!res&&(GetLastError() == ERROR_FILE_EXISTS
		||GetLastError() == ERROR_ALREADY_EXISTS))
		return true;
	else 
		return res;
}

bool DirStructMaker::make_sub_dir(LPCTSTR lpszDirBase, LPCTSTR lpszSubDir)
{
	return make_dir(CString(lpszDirBase) + lpszSubDir);
}

bool DirStructMaker::make_scene_dir_struct(const CString& strSceneName,
											LPCTSTR lpszMovObjDir,
											LPCTSTR lpszDynObjDir,
											LPCTSTR lpszStaticObjDir,
											LPCTSTR lpszWavsDir,
											LPCTSTR lpszDlgsDir
										   )
{
	//создаем каталог самой сцены
	if (!make_dir(strSceneName))
		return false;;
	
	CString str(strSceneName + _T("\\"));
	if (!make_sub_dir(str, lpszMovObjDir))
		return false;
	if (!make_sub_dir(str, lpszDynObjDir))
		return false;
	if (!make_sub_dir(str, lpszStaticObjDir))
		return false;
	if (!make_sub_dir(str, lpszWavsDir))
		return false;
	if (!make_sub_dir(str, lpszDlgsDir))
		return false;
	return true;
}

bool DirStructMaker::make_scene_dir_struct2(const CString& strSceneName,
											LPCTSTR lpszAnimationDir,
											LPCTSTR lpszStaticObjDir,
											LPCTSTR lpszDlgsDir,
											LPCTSTR lpszWavsDir
											)
{
	//создаем каталог самой сцены
	if (!make_dir(strSceneName))
		return false;;

	CString str(strSceneName + _T("\\"));
	if (!make_sub_dir(str, lpszAnimationDir))
		return false;
	if (!make_sub_dir(str, lpszStaticObjDir))
		return false;
	if (!make_sub_dir(str, lpszWavsDir))
		return false;
	if (!make_sub_dir(str, lpszDlgsDir))
		return false;
	return true;
}

bool DirStructMaker::make_materials_dir_struct(LPCTSTR lpszDir, int nScenesCount)
{
	CString strMaterialsDir((LPTSTR)IDS_MATERIALS_CATALOG);
	strMaterialsDir = _T("\\") + strMaterialsDir;
	strMaterialsDir = lpszDir + strMaterialsDir;
	strMaterialsDir += _T("\\");

	if (!make_dir(strMaterialsDir))
		return false;

	if (m_ptrShowProgress) 
	{
		m_ptrShowProgress->SetPrecents(2);
		if (m_ptrShowProgress->BreakProcess()) 
			return true;
	}

	CString strCat((LPTSTR)IDS_GLOBAL_ANIMATION_CATALOG);
	if (!make_dir(strMaterialsDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_VIDEO_CATALOG);
	if (!make_dir(strMaterialsDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_INVENTORY_CATALOG);
	if (!make_dir(strMaterialsDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_INTERFACE_CATALOG);
	if (!make_dir(strMaterialsDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_MUSIC_CATALOG);
	if (!make_dir(strMaterialsDir + strCat))
		return false;

	CString strDynObjDir((LPTSTR)IDS_DISK_DYNOBJ_CATALOG);
	CString strMovingObjDir((LPTSTR)IDS_DISK_CHARACTERS_CATALOG);
	CString strStaticObjDir((LPTSTR)IDS_STATIC_OBJ_CATALOG);
	CString strWavsDir((LPTSTR)IDS_DISK_WAVS_CATALOG);
	CString strDialogsDir((LPTSTR)IDS_DISK_DIALOGS_CATALOG);

	CString strScenNameTmpl((LPCTSTR)IDS_SCENE_NAME_TMPL);
	CString strSceneName;
	float fProgress = 51.f;
	float fStep = 49.f/nScenesCount;
	for(int i = 0; i < nScenesCount; ++i){
		strSceneName.Format((LPCTSTR)strScenNameTmpl, (LPCTSTR)strMaterialsDir, i);
		if(!make_scene_dir_struct(strSceneName,
									strMovingObjDir,
									strDynObjDir, 
									strStaticObjDir,
									strDialogsDir,
									strWavsDir
									))
			return false;
		fProgress += fStep;
		if (m_ptrShowProgress) 
		{
			m_ptrShowProgress->SetPrecents(static_cast<int>(fProgress));
			if (m_ptrShowProgress->BreakProcess())
				return true;
		}
	}
	return true;
}

bool DirStructMaker::make_resource_dir_struct(LPCTSTR lpszDir, int nScenesCount)
{
	CString strResourceDir((LPTSTR)IDS_RESOURCE_CATALOG);
	strResourceDir = _T("\\") + strResourceDir;
	strResourceDir = lpszDir + strResourceDir;
	strResourceDir += _T("\\");

	if (!make_dir(strResourceDir))
		return false;
	if (m_ptrShowProgress) 
		m_ptrShowProgress->SetPrecents(1);

	CString strCat((LPTSTR)IDS_ANIMATION_SAVE_CATALOG);
	if (!make_dir(strResourceDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_VIDEO_CATALOG);
	if (!make_dir(strResourceDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_INVENTORY_CATALOG);
	if (!make_dir(strResourceDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_INTERFACE_CATALOG);
	if (!make_dir(strResourceDir + strCat))
		return false;
	strCat.LoadString(IDS_DISK_MUSIC_CATALOG);
	if (!make_dir(strResourceDir + strCat))
		return false;


	strCat.LoadString(IDS_ANIMATION_SAVE_CATALOG);
	CString strStaticObjDir((LPTSTR)IDS_STATIC_OBJ_CATALOG);
	CString strWavsDir((LPTSTR)IDS_DISK_WAVS_CATALOG);
	CString strDlgsDir((LPTSTR)IDS_DISK_DIALOGS_CATALOG);

	CString strScenNameTmpl((LPCTSTR)IDS_SCENE_NAME_TMPL);
	CString strSceneName;
	float fProgress = 1.f;
	float fStep = 49.f/nScenesCount;
	for(int i = 0; i < nScenesCount; ++i){
		strSceneName.Format((LPCTSTR)strScenNameTmpl, (LPCTSTR)strResourceDir, i);
		if(!make_scene_dir_struct2(strSceneName, 
			strCat, 
			strStaticObjDir,
			strDlgsDir,
			strWavsDir
			))
			return false;
		fProgress += fStep;
		if (m_ptrShowProgress) 
		{
			m_ptrShowProgress->SetPrecents(static_cast<int>(fProgress));
			if (m_ptrShowProgress->BreakProcess())
				return true;
		}
	}
	return true;
}
