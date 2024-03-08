// MinigamePropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MinigamePropPage.h"

#include "curdirsafer.h"
#include <ShLwApi.h>
#include "custom_ddx.h"
#include "file_tests.h"
#include ".\minigameproppage.h"
#include "disk_op.h"

#include "qd_minigame.h"

bool editMinigameParams(qdGameDispatcher* gameDisp, qdMiniGame* miniGame);

namespace{
CurDirSafer& getCurDirSafer()
{
	static CurDirSafer safer;
	return safer;
}
}

// MinigamePropPage dialog

IMPLEMENT_DYNAMIC(MinigamePropPage, CBasePage)
MinigamePropPage::MinigamePropPage()
	: CBasePage(MinigamePropPage::IDD)
	, fileNameChanged_(false)
{
}

MinigamePropPage::~MinigamePropPage()
{
}

void MinigamePropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_INI_FILE, iniFileName_, 
		file_tests::non_strict_file_test());
}


BEGIN_MESSAGE_MAP(MinigamePropPage, CBasePage)
	ON_BN_CLICKED(IDC_BTN_SELECT_FILE, OnBnClickedBtnSelectFile)
	ON_EN_KILLFOCUS(IDC_EDIT_INI_FILE, OnEnKillfocusEditIniFile)
	ON_BN_CLICKED(IDC_BUTTON_PARAMS, OnBnClickedButtonParams)
END_MESSAGE_MAP()


// MinigamePropPage message handlers
void MinigamePropPage::set_data(qdMiniGame* miniGame, qdGameDispatcher* gameDisp){
	gameDisp_ = gameDisp;
	miniGame_ = miniGame;
}


BOOL MinigamePropPage::OnInitDialog()
{
	CBasePage::OnInitDialog();
	iniFileName_ = miniGame_->config_file_name();
	CString str(miniGame_->dll_name());
	GetDlgItem(IDC_MINIGAME_DLL_NAME)->SetWindowText(str);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL MinigamePropPage::OnApply()
{
	if (fileNameChanged_)
	{
		miniGame_->set_config_file_name(iniFileName_);
		miniGame_->load_config();
		fileNameChanged_ = false;
	}
	return CBasePage::OnApply();
}

void MinigamePropPage::OnBnClickedBtnSelectFile()
{
	CString title;
	title.LoadString(IDS_OPEN_MINIGAME_CONFIG_TITLE);
	CString filter;
	filter.LoadString(IDS_OPEN_MINIGAME_CONFIG_FILTER);
	CString ext;
	ext.LoadString(IDS_OPEN_MINIGAME_CONFIG_EXT);
	CString fileName = 
			disk_op::Get1DiskFileName(getCurDirSafer, filter, ext, title, true);

	if (fileName.IsEmpty())
		return;
	iniFileName_ = fileName;
	fileNameChanged_ = true;
	UpdateData(FALSE);
	SetModified();
}

void MinigamePropPage::OnEnKillfocusEditIniFile()
{
	if (!UpdateData())
		return;
	if (iniFileName_ != miniGame_->config_file_name())
		SetModified();
}

void MinigamePropPage::OnBnClickedButtonParams()
{
	if (editMinigameParams(gameDisp_, miniGame_))
		SetModified();
}
