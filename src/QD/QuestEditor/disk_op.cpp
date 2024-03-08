#include "stdafx.h"
#include "resource.h"
#include "CurDirSafer.h"
#include <Shlwapi.h>
#include "relativepath.h"
#include <vector>
#include "disk_op.h"

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);

namespace disk_op
{
CString GetModulePath(){
	CString res;

	GetModuleFileName(AfxGetApp()->m_hInstance, res.GetBufferSetLength(1024), 1024);
	res.ReleaseBuffer();
	return res;
}	

CString GetFileName(const CString &strPath)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( strPath, drive, dir, fname, ext );

	return CString(fname);
}
const CString Get1DiskFileName(CurDirSafer& (*get_dir_safer)(), LPCTSTR strFilter,
							   LPCTSTR strExt, LPCTSTR strTitle, BOOL bOpen /*= TRUE*/)
{
	return Get1DiskFileName(get_dir_safer(), strFilter, strExt, strTitle, bOpen);
}

const CString Get1DiskFileName(CurDirSafer& safer, 
							   LPCTSTR strFilter,
							   LPCTSTR strExt, LPCTSTR strTitle, BOOL bOpen /*= TRUE*/)
{
	CFileDialog dlg(bOpen, strExt, NULL,
		OFN_EXPLORER
		|OFN_FILEMUSTEXIST
		|OFN_NONETWORKBUTTON, strFilter);

	dlg.m_ofn.lpstrTitle = strTitle;

	CurDirGuard __cdg(&safer);

	//кладем текущую директорию
	CString strWork(CurDirSafer::GetCurrentDirectory());
	dlg.m_ofn.lpstrInitialDir = strWork;

	if (dlg.DoModal() == IDOK) 
	{
		if(!r_p::RelativePath(dlg.GetPathName(), __cdg.get_saved()+_T("\\"), strWork))
		{
			AfxMessageBox(IDS_EM_CNNT_GET_REL_PATH);
			return CString();
		}

		safer.save_current();
		return strWork;
	}
	return CString();
}

//запрашиваем у пользовател€ имена файлов
bool AskUser4FileNames(HWND hWnd, bool bNewFile, CStringVect_t* ptrFiles,
						const CString& strExt, const CString& strFilter,
						const CString& strTitle, const CString& strNewFileName,
						CurDirSafer& cur_dir_safer, DWORD dwFlags)
{
	//запомнили текущую и выставили соответствующую этому диалогу
	CurDirGuard __cdg(&cur_dir_safer);

	CFileDialog dlg((bNewFile?FALSE: TRUE), 
			strExt, strNewFileName, dwFlags, strFilter);

	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	dlg.m_ofn.lpstrTitle = strTitle;
	const DWORD MAX_FILE = 8192;
	dlg.m_ofn.nMaxFile = MAX_FILE;
	CString strFileBuffer;
	dlg.m_ofn.lpstrFile = strFileBuffer.GetBufferSetLength(MAX_FILE);

	if(dlg.DoModal() == IDOK)
	{
		CString strCurrDir = __cdg.get_saved();
		strCurrDir += "\\";

		//востанавливаем то, что было до этого
		cur_dir_safer.save_current();

		CString rel_path;
		POSITION pos = dlg.GetStartPosition();
		while(pos)
		{
			if((!r_p::RelativePath(dlg.GetNextPathName(pos), 
				strCurrDir, rel_path)))
			{
				ShowError(hWnd, IDS_EM_CNNT_GET_REL_PATH);
				return false;
			}
			ptrFiles->push_back(rel_path);
		}
		return true;
	}
	return false;
}

CString GetPathFromFullFileName(CString str)
{
	PathUnquoteSpaces(str.GetBufferSetLength(str.GetLength()+1));
	str.ReleaseBuffer();
	PathRemoveFileSpec(str.GetBufferSetLength(str.GetLength()+1));
	str.ReleaseBuffer();
	PathAddBackslash(str.GetBufferSetLength(str.GetLength()+2));
	str.ReleaseBuffer();
	return str;
}

eFileType GetImageFileType(LPCTSTR lpszFileName)
{
	if (!lpszFileName||!*lpszFileName)
		return FT_UNDEFINED;

	TCHAR ext[_MAX_EXT];
	_splitpath( lpszFileName, NULL, NULL, NULL, ext);
	CString strExt(reinterpret_cast<LPTSTR>(IDS_ANIMATION_EXT));
	if (!strExt.CompareNoCase(ext))
		return FT_QDA;
	strExt.LoadString(IDS_SPRITE_EXT);
	if (!strExt.CompareNoCase(ext))
		return FT_TGA;
	return FT_UNDEFINED;
}

CString GetDiskFolder(HWND hWnd, LPCTSTR title, bool bAllowCreate, LPCTSTR initDir)
{
	///	ќткрывает шельный диалог.
	class FolderBorwser{
	public:
		/// —труктура, котора€ будет предаватьс€ в BrowseCallbackProc в pData
		struct BrowseCBDataType
		{
			/// ѕуть, который надо стразу же открыть при старте диалога
			LPCTSTR lpszPath;

			BrowseCBDataType(LPCTSTR lpsz){
				lpszPath = lpsz; 
			}
		};

		/// \brief Callback, содержимое которого мне не€сно. 
		/// Ќужен, чтобы диалог открыл указанный путь
		static int __stdcall BrowseCallbackProc(HWND hwnd, 
								UINT uMsg, LPARAM lp, LPARAM pData) 
		{
			BrowseCBDataType* pCBData = (BrowseCBDataType*)pData;

			switch(uMsg) 
			{
			case BFFM_INITIALIZED:
				if(_tcslen((LPCTSTR)pData)) 
					SendMessage(hwnd, BFFM_SETSELECTION, TRUE, 
								(LPARAM)pCBData->lpszPath);
				break;
			}

			return 0;
		}
		/// ¬ыполн€ет всю работу по открытию диалога
		bool browse(LPCTSTR szTitle, 
					CString& path, 
					LPCTSTR open, 
					bool bAllowCreate,
					HWND hWndOwner)
		{
			bool           bRet = false;

			CComPtr<IMalloc>      pMalloc;
			if(::SHGetMalloc(&pMalloc) == NOERROR)
			{
				BrowseCBDataType cb_data(open);

				BROWSEINFO     bri;
				LPITEMIDLIST   idlist;
				memset(&bri, 0, sizeof(bri));
				bri.hwndOwner = hWndOwner;
				bri.lpszTitle = szTitle;
				bri.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
				if (!bAllowCreate)
					bri.ulFlags |= BIF_NONEWFOLDERBUTTON;
				bri.lpfn = BrowseCallbackProc;
				bri.lParam = (LPARAM)&cb_data;

				if(idlist = ::SHBrowseForFolder(&bri))
				{
					if(::SHGetPathFromIDList(idlist, CStrBuf(path, 1024)))
						bRet = true;
					pMalloc->Free(idlist);
				}
			}
			return bRet;
		}
	};


	FolderBorwser fb;
	CString res;
	fb.browse(title, res, initDir, bAllowCreate, hWnd);
	return res;
}

}