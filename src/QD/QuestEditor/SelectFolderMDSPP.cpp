// SelectFolderMDSPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "SelectFolderMDSPP.h"
#include <Shlwapi.h>
#include "DirMakeWizard.h"
#include "DirMakeWizardData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// SelectFolderMDSPP dialog

IMPLEMENT_DYNAMIC(SelectFolderMDSPP, WizardBasePage)
SelectFolderMDSPP::SelectFolderMDSPP()
	: WizardBasePage(SelectFolderMDSPP::IDD)
	, m_strFolder(_T(""))
{
}

SelectFolderMDSPP::~SelectFolderMDSPP()
{
}

void SelectFolderMDSPP::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_strFolder);
}


BEGIN_MESSAGE_MAP(SelectFolderMDSPP, WizardBasePage)
	ON_EN_CHANGE(IDC_EDIT, OnEnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON, OnBnClickedButton)
END_MESSAGE_MAP()

BOOL SelectFolderMDSPP::OnSetActive()
{
	DirMakeWizard* pw = static_cast<DirMakeWizard*>(wizard());
	DirMakeWizardData* pad = pw->data();
	m_strFolder = pad->m_strDir;
	UpdateData(FALSE);
	check_next_btn_access();
	return WizardBasePage::OnSetActive();
}

void SelectFolderMDSPP::OnEnChangeEdit()
{
	UpdateData();
	check_next_btn_access();
}

CString SelectFolderMDSPP::get_disk_folder()
{
	BROWSEINFO              bri;
	LPMALLOC                pMalloc;
	LPITEMIDLIST    idlist;
	//   TCHAR                   cbBuf[MAX_PATH];
	CString res;
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		memset(&bri, 0, sizeof(bri));
		bri.hwndOwner = m_hWnd;
		CString title;
		title.LoadString(IDS_TITLE_FOR_SEL_FOLDER_DLG);
		bri.lpszTitle = title;
		bri.ulFlags = BIF_RETURNONLYFSDIRS;
		if (idlist = ::SHBrowseForFolder(&bri))
		{
			if (::SHGetPathFromIDList(idlist, res.GetBufferSetLength(1024)))
			{
				res += "\\";
			}
			pMalloc->Free(idlist);
			res.ReleaseBuffer();
		}
	}
	pMalloc->Release();
	return res;
}

void SelectFolderMDSPP::OnBnClickedButton()
{
	m_strFolder = get_disk_folder();
	UpdateData(FALSE);
	check_next_btn_access();
}

void SelectFolderMDSPP::check_next_btn_access()
{
	if (PathFileExists(m_strFolder)) 
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
}

BOOL SelectFolderMDSPP::OnKillActive()
{
	DirMakeWizard* pw = static_cast<DirMakeWizard*>(wizard());
	DirMakeWizardData* pad = pw->data();
	pad->m_strDir = m_strFolder;

	return WizardBasePage::OnKillActive();
}
