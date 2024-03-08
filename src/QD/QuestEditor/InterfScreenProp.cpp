// InterfScreenProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfScreenProp.h"

#include "qd_interface_screen.h"
#include "qd_music_track.h"

#include ".\interfscreenprop.h"

#include "custom_ddx.h"
#include "file_tests.h"
#include <ShLwApi.h>
#include "curdirsafer.h"
#include "relativepath.h"

#include "disk_op.h"
// InterfScreenProp dialog

namespace
{
CurDirSafer& GetCurDirSafer()
{
	static CurDirSafer dirSafer;
	return dirSafer;
}
}

IMPLEMENT_DYNAMIC(InterfScreenProp, CBasePage)
InterfScreenProp::InterfScreenProp()
	: CBasePage(InterfScreenProp::IDD)
	, m_pScreen(NULL)
	, m_fAutoHideTime(0.f)
	, m_vAutoHideOffset(0, 0)
	, musicTrackFile_(_T(""))
	, cycled_(FALSE)
	, volume_(0)
{
}

InterfScreenProp::~InterfScreenProp()
{
}

void InterfScreenProp::set_data(qdInterfaceScreen* pscrn){
	m_pScreen = pscrn;
}
void InterfScreenProp::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AUTOHIDE_TIME, m_fAutoHideTime);
	DDX_Text(pDX, IDC_EDIT_OFFSET_X, m_vAutoHideOffset.x);
	DDX_Text(pDX, IDC_EDIT_OFFSET_Y, m_vAutoHideOffset.y);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_MUSIC_TRACK, musicTrackFile_, 
		file_tests::non_strict_file_test());
	DDX_Check(pDX, IDC_CHECK_MUSIC_TRACK_CYCLED, cycled_);
	DDX_Slider(pDX, IDC_SLIDER_MUSIC_TRACK_VOLUME, volume_);
}


BEGIN_MESSAGE_MAP(InterfScreenProp, CBasePage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AUTOHIDE_TIME, OnDeltaposSpinAutohideTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_X, OnDeltaposSpinOffset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_Y, OnDeltaposSpinOffset)
	ON_BN_CLICKED(IDC_BTN_SELECT_MUSIC_TRACK, OnBnClickedBtnSelectMusicTrack)
	ON_EN_KILLFOCUS(IDC_EDIT_MUSIC_TRACK, OnEnKillfocusEditMusicTrack)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_MUSIC_TRACK_CYCLED, OnBnClickedCheckMusicTrackCycled)
END_MESSAGE_MAP()


// InterfScreenProp message handlers

BOOL InterfScreenProp::OnApply()
{
	wnd2screen();
	return CBasePage::OnApply();
}

BOOL InterfScreenProp::OnInitDialog()
{
	CBasePage::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_OFFSET_X))->
		SetRange32(INT_MIN, INT_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_OFFSET_Y))->
		SetRange32(INT_MIN, INT_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_AUTOHIDE_TIME))->
		SetRange(0, 100);
	static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MUSIC_TRACK_VOLUME))->
		SetRange(0, 256);

	screen2wnd();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void InterfScreenProp::OnDeltaposSpinAutohideTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}

	float const DIVISOR = 100.f;
	m_fAutoHideTime += pNMUpDown->iDelta/DIVISOR;
	if (m_fAutoHideTime < 0.f)
		m_fAutoHideTime = 0.f;
	SetModified();
	UpdateData(FALSE);
	*pResult = 0;
}

BOOL InterfScreenProp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		if (::GetDlgItem(m_hWnd, IDC_EDIT_OFFSET_X) == pMsg->hwnd
		||::GetDlgItem(m_hWnd, IDC_EDIT_OFFSET_Y) == pMsg->hwnd
		||::GetDlgItem(m_hWnd, IDC_EDIT_AUTOHIDE_TIME) == pMsg->hwnd
		||::GetDlgItem(m_hWnd, IDC_EDIT_MUSIC_TRACK) == pMsg->hwnd
		)
			
		{
			UpdateData();
			SetModified();
			return TRUE;
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

void InterfScreenProp::screen2wnd()
{
	m_fAutoHideTime = m_pScreen->autohide_time();
	m_vAutoHideOffset = m_pScreen->autohide_offset();
	qdMusicTrack const& mt = m_pScreen->music_track();

	musicTrackFile_ = mt.file_name();
	cycled_ = mt.is_cycled();
	volume_ = mt.volume();
}

void InterfScreenProp::wnd2screen()
{
	m_pScreen->set_autohide_offset(m_vAutoHideOffset);
	m_pScreen->set_autohide_time(m_fAutoHideTime);
	qdMusicTrack mt;
	mt.set_file_name(musicTrackFile_);
	mt.set_volume(volume_);
	mt.toggle_cycle(cycled_);
	m_pScreen->set_music_track(mt);
}

void InterfScreenProp::OnDeltaposSpinOffset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}


void InterfScreenProp::OnBnClickedBtnSelectMusicTrack()
{
	CString strFilter;
	strFilter.LoadString(IDS_OPEN_MUSIC_TRACK_FILTER);
	CString strExt;
	strFilter.LoadString(IDS_OPEN_MUSIC_TRACK_EXT);
	CString strTitle;
	strTitle.LoadString(IDS_OPEN_MUSIC_TRACK_TITLE);
	CString str = 
		disk_op::Get1DiskFileName(GetCurDirSafer, strFilter, strExt, strTitle, TRUE);
	if (musicTrackFile_ != str)
	{
		musicTrackFile_ = str;
		SetModified();
	}
	UpdateData(FALSE);
}

void InterfScreenProp::OnEnKillfocusEditMusicTrack()
{
	if (!UpdateData())
		return;
	if (musicTrackFile_.CompareNoCase(m_pScreen->music_track().file_name()))
		SetModified();
}

void InterfScreenProp::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_MUSIC_TRACK_VOLUME)
	{
		if (nSBCode == TB_THUMBPOSITION)
			SetModified();
	}
	CBasePage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void InterfScreenProp::OnBnClickedCheckMusicTrackCycled()
{
	SetModified();
}
