#pragma once


#include "basepage.h"
// MusicTrackPropPage dialog

#include "qd_fwd.h"
#include "afxcmn.h"

/**
	�������� ������������ �����
 */
class MusicTrackPropPage : public CBasePage
{
	DECLARE_DYNAMIC(MusicTrackPropPage)

public:
	MusicTrackPropPage();
	virtual ~MusicTrackPropPage();

	/// ��������� ����������� ����
	bool set_data(qdMusicTrack* pmt);
// Dialog Data
	enum { IDD = IDD_PROPPAGE_MUSIC_TRACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pmsg);
	afx_msg void OnClickOnChecks();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	/// ������ �� ��������� ������� � ����������� ����
	void wnd2mt();
	/// ��������� ������������ ����� � �������� �������
	void mt2wnd();
	DECLARE_MESSAGE_MAP()
private:
	/// �������������� ����������� ����
	qdMusicTrack* m_ptrMusicTrack;
	/// ��������� ���������
	CSliderCtrl m_wndVolume;
	/// ���������
	int m_iVolume;
	/// ������� �������������
	BOOL m_bCycle;

	BOOL disableRestart_;
	BOOL disableSwitchOff_;
};
