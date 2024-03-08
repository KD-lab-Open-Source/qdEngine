#pragma once

#include "BasePage.h"
// InterfScreenProp dialog

class qdInterfaceScreen;
/**
	Свойства интерфейсного экрана
 */
class InterfScreenProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfScreenProp)

public:
	InterfScreenProp();
	virtual ~InterfScreenProp();

	/// Назначает экран, с которым будем рабоатать
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
	/// данные из экрана в контролы диалога
	void screen2wnd();
	/// данные из контролов диалога в экран
	void wnd2screen();
private:
	/// Экран
	qdInterfaceScreen* m_pScreen;
	/// m_pScreen->set_autohide_time(m_fAutoHideTime)
	float m_fAutoHideTime;
	/// m_pScreen->set_autohide_offset(m_vAutoHideOffset)
	Vect2i m_vAutoHideOffset;
	/// Имя файла музыкального трека
	CString musicTrackFile_;
	/// Зациклен ли музыкальный трек
	BOOL cycled_;
	/// громкость
	int volume_;
};
