// VideoPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "VideoPage.h"
#include "qd_video.h"
#include "curdirsafer.h"
#include "relativepath.h"
#include <ShLwApi.h>
#include "custom_ddx.h"
#include "file_tests.h"

namespace
{
	/// Взависимости от testValue назначает или убирает флаг
	void testAndSet(bool testValue, int flag, qdVideo* pv)
	{
		if (testValue)
			pv->set_flag(flag);
		else
			pv->drop_flag(flag);
	}
}
// VideoPage dialog
CurDirSafer& GetVideoCurDirSafer()
{
	static CurDirSafer videoDirSafer;
	return videoDirSafer;
}

CurDirSafer& GetBackDirSafer()
{
	static CurDirSafer backDirSafer;
	return backDirSafer;
}

IMPLEMENT_DYNAMIC(VideoPage, CBasePage)
VideoPage::VideoPage()
	: CBasePage(VideoPage::IDD),m_ptrSource(NULL),m_ptrGameDisp(NULL)
	, m_iX(0)
	, m_iY(0)
	, m_strBackFileName(_T(""))
	, m_strVideoFileName(_T(""))
	, m_bToCenter(FALSE)
	, m_bFullScreen(FALSE)
	, m_bNoInterrupt(FALSE)
	, m_bIntroVideo(FALSE)
	, m_bEnableMusic(FALSE)
{
	m_ptrVideoCopy.reset(new qdVideo());
}

VideoPage::~VideoPage()
{
}

void VideoPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);																
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_BACK_FILE, m_strBackFileName, 
		file_tests::non_strict_file_test());
	custom_ddx::DDX_Text(pDX, IDC_EDIT_VIDEO_FILE, m_strVideoFileName, 
		file_tests::strict_file_test());
	DDX_Check(pDX, IDC_CHECK_TO_CENTER, m_bToCenter);
	DDX_Check(pDX, IDC_CHECK_FULL_SCREEN, m_bFullScreen);
	DDX_Check(pDX, IDC_CHECK_NO_INTERRUPT, m_bNoInterrupt);
	DDX_Check(pDX, IDC_CHECK_VIDEO_INTRO, m_bIntroVideo);
	DDX_Check(pDX, IDC_CHECK_ENABLE_MUSIC, m_bEnableMusic);
}

void VideoPage::set_data(qdGameDispatcher* ptrDisp, qdVideo* ptrVideo){
	m_ptrGameDisp = ptrDisp;
	*m_ptrVideoCopy.get() = *ptrVideo;
	m_ptrSource = ptrVideo;
}

BEGIN_MESSAGE_MAP(VideoPage, CBasePage)
	ON_BN_CLICKED(IDC_BTN_SELECT_BFILE, OnBnClickedBtnSelectBfile)
	ON_BN_CLICKED(IDC_BTN_SELECT_VFILE, OnBnClickedBtnSelectVfile)
	ON_BN_CLICKED(IDC_CHECK_TO_CENTER, OnBnClickedCheckToCenter)
	ON_BN_CLICKED(IDC_CHECK_FULL_SCREEN, OnBnClickedCheckFullScreen)
	ON_BN_CLICKED(IDC_CHECK_NO_INTERRUPT, OnClickInCheck)
	ON_BN_CLICKED(IDC_CHECK_VIDEO_INTRO, OnClickInCheck)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_MUSIC, OnClickInCheck)
END_MESSAGE_MAP()


// VideoPage message handlers

BOOL VideoPage::OnApply()
{
	if (!UpdateData())
		return FALSE;

	wnd2video();
	*m_ptrSource = *m_ptrVideoCopy.get();
	return CBasePage::OnApply();
}


void VideoPage::video2wnd()
{
	m_iX = m_ptrVideoCopy->position().x;
	m_iY = m_ptrVideoCopy->position().y;
	m_strVideoFileName = m_ptrVideoCopy->file_name();
	m_strBackFileName = m_ptrVideoCopy->background_file_name();

	m_bFullScreen = m_ptrVideoCopy->check_flag(qdVideo::VID_FULLSCREEN_FLAG);
	m_bToCenter = m_ptrVideoCopy->check_flag(qdVideo::VID_CENTER_FLAG);
	EnableLayout(!m_bFullScreen&&!m_bToCenter);
	m_bNoInterrupt = m_ptrVideoCopy->check_flag(qdVideo::VID_DISABLE_INTERRUPT_FLAG);
	m_bIntroVideo = m_ptrVideoCopy->check_flag(qdVideo::VID_INTRO_MOVIE);
	m_bEnableMusic = m_ptrVideoCopy->check_flag(qdVideo::VID_ENABLE_MUSIC);
}

void VideoPage::wnd2video()
{
	m_ptrVideoCopy->set_position(Vect2s(m_iX, m_iY));
	m_ptrVideoCopy->set_background_file_name(m_strBackFileName);
	m_ptrVideoCopy->set_file_name(m_strVideoFileName);
	
	testAndSet(m_bFullScreen, qdVideo::VID_FULLSCREEN_FLAG, m_ptrVideoCopy.get());
	testAndSet(m_bToCenter, qdVideo::VID_CENTER_FLAG, m_ptrVideoCopy.get());
	testAndSet(m_bNoInterrupt,qdVideo::VID_DISABLE_INTERRUPT_FLAG,m_ptrVideoCopy.get());
	testAndSet(m_bIntroVideo, qdVideo::VID_INTRO_MOVIE, m_ptrVideoCopy.get());
	testAndSet(m_bEnableMusic, qdVideo::VID_ENABLE_MUSIC, m_ptrVideoCopy.get());
}

BOOL VideoPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	video2wnd();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(0, 1000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(0, 1000);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void VideoPage::OnBnClickedBtnSelectBfile()
{
	CString str;
	str.LoadString(IDS_OPEN_SPRITE_EXT);
	CString str2;
	str2.LoadString(IDS_OPEN_SPRITE_FILTER);

	//запомнили текущую и выставили соответствующую этому диалогу
	CurDirGuard __cdg(&GetBackDirSafer());

	CFileDialog dlg(TRUE, str, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, str2);

	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	if(dlg.DoModal() == IDOK)
	{
		//достали сохраненную
		CString strCurrDir = __cdg.get_saved();
		strCurrDir += "\\";

		CString rel_path;
		if((!r_p::RelativePath(dlg.GetPathName(), strCurrDir, rel_path)))
		{
			strCurrDir.LoadString(IDS_ERR_MSG_CAPTION);
			rel_path.LoadString(IDS_EM_CNNT_GET_REL_PATH);
			MessageBox(rel_path, strCurrDir, MB_OK|MB_ICONHAND);
			return ;
		}
		GetBackDirSafer().save_current();

		m_strBackFileName = rel_path;
		m_ptrVideoCopy->set_background_file_name(m_strBackFileName);
		SetModified();
	}
	UpdateData(FALSE);
}

void VideoPage::OnBnClickedBtnSelectVfile()
{
	CurDirGuard __cdg(&GetVideoCurDirSafer());

	CString str2;
	str2.LoadString(IDS_OPEN_VIDEO_FILTER);
	CString strTitle((LPTSTR)IDS_OSD_CAP_SEL_VIDEO);

	CFileDialog dlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST , str2);
	CString curDir = CurDirSafer::GetCurrentDirectory();
	dlg.m_ofn.lpstrInitialDir = curDir;
	dlg.m_ofn.lpstrTitle = strTitle;
	if(dlg.DoModal() == IDOK)
	{
		CString strCurrDir = __cdg.get_saved();
		strCurrDir += "\\";

		CString rel_path;
		if((!r_p::RelativePath(dlg.GetPathName(), strCurrDir, rel_path)))
		{
			ShowError(m_hWnd, IDS_EM_CNNT_GET_REL_PATH);
			return ;
		}
		GetVideoCurDirSafer().save_current();
		m_strVideoFileName = rel_path;
		m_ptrVideoCopy->set_file_name(m_strVideoFileName);
		SetModified();
	}
	UpdateData(FALSE);
}

BOOL VideoPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN) {
			CWnd* pfoc = GetFocus();
			if (pfoc) 
			{
				switch(pfoc->GetDlgCtrlID())
				{
				case IDC_EDIT_X:
				case IDC_EDIT_Y:
					OnEnterInPos();
					break;
				case IDC_EDIT_BACK_FILE:
					OnEnterInBackFile();
					break;
				case IDC_EDIT_VIDEO_FILE:
					OnEnterInVideoFile();
					break;
				}
				return TRUE;
			}
		}

	}
	return CBasePage::PreTranslateMessage(pMsg);
}

void VideoPage::OnEnterInPos()
{
	if (!UpdateData())
		return;
	m_ptrVideoCopy->set_position(Vect2s(m_iX, m_iY));
	SetModified();
}

void VideoPage::OnEnterInVideoFile()
{
	if (!UpdateData())
		return;
	if (m_strVideoFileName.CompareNoCase(m_ptrVideoCopy->file_name()))
		SetModified();
}

void VideoPage::OnEnterInBackFile()
{
	if (!UpdateData())
		return;
	if (m_strBackFileName.CompareNoCase(m_ptrVideoCopy->background_file_name()))
		SetModified();
}

void VideoPage::OnEnKillfocusEditVideoFile()
{
	if (!UpdateData())
		return;
	if (m_strVideoFileName.CompareNoCase(m_ptrVideoCopy->file_name()))
		SetModified();
}

void VideoPage::OnEnKillfocusEditBackFile()
{
	if (!UpdateData())
		return;
	if (m_strBackFileName.CompareNoCase(m_ptrVideoCopy->background_file_name()))
		SetModified();
}

void VideoPage::EnableLayout(bool benable)
{
	GetDlgItem(IDC_SPIN_X)->EnableWindow(benable);
	GetDlgItem(IDC_SPIN_Y)->EnableWindow(benable);
	GetDlgItem(IDC_EDIT_X)->EnableWindow(benable);
	GetDlgItem(IDC_EDIT_Y)->EnableWindow(benable);
}

void VideoPage::OnBnClickedCheckToCenter()
{
	if (!UpdateData())
		return;
	EnableLayout(!m_bFullScreen&&!m_bToCenter);
	SetModified();
}

void VideoPage::OnBnClickedCheckFullScreen()
{
	if (!UpdateData())
		return;
	EnableLayout(!m_bFullScreen&&!m_bToCenter);
	SetModified();
}

/*
void VideoPage::OnBnClickedCheckNoInterrupt()
{
	if (!UpdateData())
		return;
	SetModified();
}
*/

void VideoPage::OnClickInCheck()
{
	if (!UpdateData())
		return;
	SetModified();
}
