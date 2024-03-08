#pragma once

#include "IShowProgress.h"
#include "WizardBasePage.h"

// ProgressMDSPP dialog

/**
	—траничка визарда создани€ структуры каталогов, 
	отображающи€ прогресс процесса.
 */
class ProgressMDSPP : public WizardBasePage, public IShowProgress
{
	DECLARE_DYNAMIC(ProgressMDSPP)

public:
	ProgressMDSPP();
	virtual ~ProgressMDSPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MDS_PROGRES };

	void SetPrecents(int prec);
	bool BreakProcess();
	void SetText(LPCTSTR lpszMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/// ¬ыравнивает положение контролов при изменении размера
	afx_msg void OnSize(UINT nType, int cx, int cy);
	/// «апускает процесс создани€
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnStop();

	/**
		«апускает таймер, при срабатывании которого запускаетс€ процесс создани€
		директорий.
		Ќужно это дл€ того, чтобы страничка могла себ€ показать
	 */
	virtual BOOL OnSetActive();
	DECLARE_MESSAGE_MAP()

	/// ќбрабатывает сообщени€ во врем€ создани€ директорий
	void PumpMessages();
private:
	CProgressCtrl m_wndProgress;
	CButton m_wndStop;
	bool m_bStopPressed;

};
