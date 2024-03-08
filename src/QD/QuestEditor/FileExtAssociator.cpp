// FileExtAssociator.cpp: implementation of the FileExtAssociator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FileExtAssociator.h"

#include "questeditor.h"
#include "disk_op.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString GetOpenCommandLine()
{
	CString res = disk_op::GetModulePath();
	
	res += _T(" \"%1\" ");
	return res;
}

CString GetDefaultIconString()
{
	CString res = disk_op::GetModulePath();
	res += _T(",1");
	return res;
}

FileExtAssociator::FileExtAssociator()
{
	
}

FileExtAssociator::~FileExtAssociator()
{
	
}

bool FileExtAssociator::DO(UINT uiExtStrID)
{
	m_uiExtStrID = uiExtStrID;
	ATL::CRegKey rkKeyExt, rkKeyProgID;
	if(!DefineFileClass(&rkKeyExt, &rkKeyProgID))
		return false;

	DefiningAttributes4FileClass(&rkKeyProgID);
	DefineShellSubKeys(&rkKeyProgID);
	
	DefineAppKeyEntry();
	
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0 );
	return true;
}

bool FileExtAssociator::DefineFileClass(ATL::CRegKey *pKeyExt, ATL::CRegKey* pKeyProgID)
{
	return (DefineExtEntry(pKeyExt)
		&&DefineProgIDEntre(pKeyProgID));
}

bool FileExtAssociator::DefineExtEntry(ATL::CRegKey* pKeyExt)
{
	CString str;
	str.LoadString(m_uiExtStrID);
	if(pKeyExt->Create(HKEY_CLASSES_ROOT, str)!= ERROR_SUCCESS)
		return false;
	str.LoadString(IDS_PROG_ID_REG_KEY);
	if(pKeyExt->SetValue(NULL, REG_SZ, 
		static_cast<LPCTSTR>(str), str.GetLength()+1) != ERROR_SUCCESS)
		return false;
	return true;
}

bool FileExtAssociator::DefineProgIDEntre(ATL::CRegKey* pKeyProgID)
{
	CString str;
	str.LoadString(IDS_PROG_ID_REG_KEY);
	if(pKeyProgID->Create(HKEY_CLASSES_ROOT, str)!= ERROR_SUCCESS)
		return false;

	CString strVal((LPTSTR)IDS_PROG_ID_KEY_VALUE);
	if(pKeyProgID->
		SetValue(NULL, REG_SZ, static_cast<LPCTSTR>(strVal), strVal.GetLength()+1)
		!=ERROR_SUCCESS)
		return false;

	return true;
}

bool FileExtAssociator::DefiningAttributes4FileClass(ATL::CRegKey* pKey)
{
	DWORD dwFlags = 0x00000010 | 0x00000020;
	return(RegSetValueEx(*pKey, _T("EditFlags"), 0, REG_BINARY, 
		(BYTE*)&dwFlags, sizeof(DWORD)) == ERROR_SUCCESS);
}

bool FileExtAssociator::DefineAppKeyEntry()
{
#ifndef _DEBUG
	CRegKey keyApps;
	if(keyApps.Open(HKEY_CLASSES_ROOT, _T("Applications")) != 
		ERROR_SUCCESS)
		return false;

	ATL::CRegKey keyApp;
	CString str;
	str.LoadString(IDS_APPS_SUB_KEY);
	if(keyApp.Create(keyApps, str) != ERROR_SUCCESS)
		return false;
	
	//запрещаем появление в OpenWith диалоге
	keyApp.SetValue(_T("NoOpenWith"), REG_SZ, _T(""), 1);

	str.LoadString(IDS_FRIENDLY_APP_NAME);
	keyApp.SetKeyValue(_T("FriendlyAppName"),str);

	keyApp.SetKeyValue(_T("DefaultIcon"),GetDefaultIconString());

	DefineShellSubKeys(&keyApp);
#endif

	return true;
}

bool FileExtAssociator::DefineShellSubKeys(ATL::CRegKey* pKey)
{
#ifndef _DEBUG
	ATL::CRegKey shellKey;
	if(shellKey.Create(*pKey, _T("shell")) != ERROR_SUCCESS)
		return false;

	ATL::CRegKey openKey;
	if(openKey.Create(shellKey, _T("open")) != ERROR_SUCCESS)
		return false;
	
	CString str = GetOpenCommandLine();
	if (openKey.SetKeyValue(_T("command"), str) != ERROR_SUCCESS) {
		return false;
	}
#endif

	return true;
}
