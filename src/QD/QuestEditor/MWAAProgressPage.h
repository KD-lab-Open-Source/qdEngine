#pragma once


// MWAAProgressPage dialog

#include "WizardBasePage.h"
#include "ishowprogress.h"

class qdGameDispatcher;
class qdGameDispatcherBase;

/**
	Страничка отражающая прогресс процесса сборки
 */
class MWAAProgressPage : public WizardBasePage, public IShowProgress
{
	DECLARE_DYNAMIC(MWAAProgressPage)

public:
	MWAAProgressPage();
	virtual ~MWAAProgressPage();

// Dialog Data
	//{{AFX_DATA(MWAddAnimFirst)
	enum { IDD = IDD_PROPPAGE_AA_PROGRES };
	//}}AFX_DATA

	void SetPrecents(int prec);
	bool BreakProcess();
	void SetText(LPCTSTR lpszMsg);
	/// Назначаются диспетчер проекта и каталог, в который будет сложена анимация
	void set_disps(qdGameDispatcher* ptrGD, qdGameDispatcherBase* ptrDB);
protected:
	//{{AFX_VIRTUAL(MWAAProgressPage)
	virtual BOOL OnInitDialog();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

//{{AFX_MSG(MWAAProgressPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnStop();
	//}}AFX_MSG
	/// Обрабатывает пришедшие сообщения, пока идет процесс сборки
	void PumpMessages();
private:

	CProgressCtrl m_wndProgress;
	CButton m_wndStop;
	/// Была ли нажата кнопка "стоп"
	bool m_bStopPressed;
	
	/// Диспетчер проекта
	qdGameDispatcher* m_ptrGameDisp;
	/// Каталог, в который будет сложена анимация
	qdGameDispatcherBase* m_ptrDispBase;
};
