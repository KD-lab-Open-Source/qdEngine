#pragma once

#include "BasePage.h"
// InterfScreenProp dialog

class qdInterfaceScreen;
/**
	�������� ������������� ������
 */
class InterfScreenProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfScreenProp)

public:
	InterfScreenProp();
	virtual ~InterfScreenProp();

	/// ��������� �����, � ������� ����� ���������
	void set_data(qdInterfaceScreen* pscrn);

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpinAutohideTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOffset(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnSelectMusicTrack();
	afx_msg void OnEnKillfocusEditMusicTrack();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckMusicTrackCycled();
protected:
	/// ������ �� ������ � �������� �������
	void screen2wnd();
	/// ������ �� ��������� ������� � �����
	void wnd2screen();
private:
	/// �����
	qdInterfaceScreen* m_pScreen;
	/// m_pScreen->set_autohide_time(m_fAutoHideTime)
	float m_fAutoHideTime;
	/// m_pScreen->set_autohide_offset(m_vAutoHideOffset)
	Vect2i m_vAutoHideOffset;
	/// ��� ����� ������������ �����
	CString musicTrackFile_;
	/// �������� �� ����������� ����
	BOOL cycled_;
	/// ���������
	int volume_;
};
