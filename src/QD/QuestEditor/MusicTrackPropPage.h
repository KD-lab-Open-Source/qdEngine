#pragma once


#include "basepage.h"
// MusicTrackPropPage dialog

#include "qd_fwd.h"
#include "afxcmn.h"

/**
	Свойства музыкального трека
 */
class MusicTrackPropPage : public CBasePage
{
	DECLARE_DYNAMIC(MusicTrackPropPage)

public:
	MusicTrackPropPage();
	virtual ~MusicTrackPropPage();

	/// Назначает музыкальный трек
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

	/// Данные из контролов диалога в музыкальный трек
	void wnd2mt();
	/// Параметры музыкального трека в контролы диалога
	void mt2wnd();
	DECLARE_MESSAGE_MAP()
private:
	/// Обрабатываемый Музыкальный трек
	qdMusicTrack* m_ptrMusicTrack;
	/// Регулятор громкости
	CSliderCtrl m_wndVolume;
	/// Громкость
	int m_iVolume;
	/// Признак зацикленности
	BOOL m_bCycle;

	BOOL disableRestart_;
	BOOL disableSwitchOff_;
};
