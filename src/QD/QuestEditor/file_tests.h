#pragma once

#include <ShLwApi.h>
/************************************************************************/
/* используются вместе с custom_ddx для проверки существования файла,   */
/* описанного строкой в контроле
/************************************************************************/
VOID ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
VOID ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

namespace file_tests{
/// Требует существования введенного имени файла. Пустую строку вводить нельзя
class strict_file_test{
public:
	bool operator()(CDataExchange* pDX, int nIDC)
	{
		if (!pDX->m_bSaveAndValidate) 
			return true;
		HWND hwnd = pDX->PrepareCtrl(nIDC);
		CString strBuff;
		GetWindowText(hwnd, strBuff.GetBufferSetLength(MAX_PATH), MAX_PATH);
		strBuff.ReleaseBuffer();

		if (!PathFileExists(strBuff))
		{
			ShowError(*pDX->m_pDlgWnd, IDS_FILE_MUST_EXIST);
			return false;
		}
		return true;
	}
};

///  \brief Если строка не пустая, то требует существования введенного имени файла. 
/// Позволяет вводить пустую строку
class non_strict_file_test{
public:
	bool operator()(CDataExchange* pDX, int nIDC)
	{
		if (!pDX->m_bSaveAndValidate) 
			return true;
		HWND hwnd = pDX->PrepareCtrl(nIDC);
		CString strBuff;
		GetWindowText(hwnd, strBuff.GetBufferSetLength(MAX_PATH), MAX_PATH);
		strBuff.ReleaseBuffer();
		strBuff.Trim(); 
		if (!strBuff.IsEmpty()&&!PathFileExists(strBuff))
		{
			ShowError(*pDX->m_pDlgWnd, IDS_FILE_MUST_EXIST_OR_NO_NAME);
			return false;
		}
		return true;
	}
};

}