#pragma once


// MWAAProgressPage dialog

#include "WizardBasePage.h"
#include "ishowprogress.h"

class qdGameDispatcher;
class qdGameDispatcherBase;

/**
	��������� ���������� �������� �������� ������
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
	/// ����������� ��������� ������� � �������, � ������� ����� ������� ��������
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
	/// ������������ ��������� ���������, ���� ���� ������� ������
	void PumpMessages();
private:

	CProgressCtrl m_wndProgress;
	CButton m_wndStop;
	/// ���� �� ������ ������ "����"
	bool m_bStopPressed;
	
	/// ��������� �������
	qdGameDispatcher* m_ptrGameDisp;
	/// �������, � ������� ����� ������� ��������
	qdGameDispatcherBase* m_ptrDispBase;
};
