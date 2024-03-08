// MusicTrackPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MusicTrackPropPage.h"
#include "qd_music_track.h"

// MusicTrackPropPage dialog

int const VOLUME_MIN = 0;
int const VOLUME_MAX = 256;
IMPLEMENT_DYNAMIC(MusicTrackPropPage, CBasePage)
MusicTrackPropPage::MusicTrackPropPage()
	: CBasePage(MusicTrackPropPage::IDD)
	, m_iVolume(0)
	, m_bCycle(FALSE)
{
	disableRestart_ = false;
	disableSwitchOff_ = false;
}

MusicTrackPropPage::~MusicTrackPropPage()
{
}

bool MusicTrackPropPage::set_data(qdMusicTrack* pmt)
{
	m_ptrMusicTrack = pmt;
	if (!m_ptrMusicTrack)
		return false;
	return true;
}
void MusicTrackPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_wndVolume);
	DDX_Text(pDX, IDC_EDIT_VOLUME, m_iVolume);
	DDX_Check(pDX, IDC_CHECK_CYCLE, m_bCycle);
	DDX_Check(pDX, IDC_CHECK_NO_RESTART, disableRestart_);
	DDX_Check(pDX, IDC_CHECK_NO_SWITCH_OFF, disableSwitchOff_);
}


BEGIN_MESSAGE_MAP(MusicTrackPropPage, CBasePage)
	ON_BN_CLICKED(IDC_CHECK_CYCLE, OnClickOnChecks)
	ON_BN_CLICKED(IDC_CHECK_NO_RESTART, OnClickOnChecks)
	ON_BN_CLICKED(IDC_CHECK_NO_SWITCH_OFF, OnClickOnChecks)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// MusicTrackPropPage message handlers

BOOL MusicTrackPropPage::OnApply()
{
	if (!UpdateData())
		return FALSE;

	wnd2mt();
	return CBasePage::OnApply();
}

void MusicTrackPropPage::OnCancel()
{
//	*m_ptrMusicTrack = *m_ptrCopy;
	CBasePage::OnCancel();
}

BOOL MusicTrackPropPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	mt2wnd();
	m_wndVolume.SetRange(VOLUME_MIN, VOLUME_MAX);
	m_wndVolume.SetPos(m_iVolume);

	CString str(m_ptrMusicTrack->file_name());
	GetDlgItem(IDC_MUSIC_FILE_NAME)->SetWindowText(str);

	UpdateData(FALSE);
	return TRUE;
}

BOOL MusicTrackPropPage::PreTranslateMessage(MSG* pmsg)
{
	if (pmsg->message == WM_KEYDOWN&&pmsg->wParam == VK_RETURN)
	{
		if (pmsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_VOLUME))
		{
			if (UpdateData())
			{
				m_wndVolume.SetPos(m_iVolume);
				SetModified();
			}
			return TRUE;
		}
	}
	return CBasePage::PreTranslateMessage(pmsg);
}

void MusicTrackPropPage::wnd2mt()
{
	m_ptrMusicTrack->set_volume(m_iVolume);
	m_ptrMusicTrack->toggle_cycle(m_bCycle);

	if(disableRestart_)
		m_ptrMusicTrack->set_flag(QD_MUSIC_TRACK_DISABLE_RESTART);
	else
		m_ptrMusicTrack->drop_flag(QD_MUSIC_TRACK_DISABLE_RESTART);

	if(disableSwitchOff_)
		m_ptrMusicTrack->set_flag(QD_MUSIC_TRACK_DISABLE_SWITCH_OFF);
	else
		m_ptrMusicTrack->drop_flag(QD_MUSIC_TRACK_DISABLE_SWITCH_OFF);
}

void MusicTrackPropPage::mt2wnd()
{
	m_iVolume = m_ptrMusicTrack->volume();
	m_bCycle = m_ptrMusicTrack->is_cycled();

	disableRestart_ = m_ptrMusicTrack->check_flag(QD_MUSIC_TRACK_DISABLE_RESTART);
	disableSwitchOff_ = m_ptrMusicTrack->check_flag(QD_MUSIC_TRACK_DISABLE_SWITCH_OFF);
}

void MusicTrackPropPage::OnClickOnChecks()
{
	if (!UpdateData())
		return;
	SetModified();
}

void MusicTrackPropPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->m_hWnd == m_wndVolume.m_hWnd) 
	{
		if (nSBCode == TB_ENDTRACK)
		{
			m_iVolume = m_wndVolume.GetPos();
			SetModified();
			UpdateData(FALSE);
		}
	}
	CBasePage::OnHScroll(nSBCode, nPos, pScrollBar);
}
