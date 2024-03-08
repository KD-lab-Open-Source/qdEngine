#pragma once

#include "IShowProgress.h"
#include "WizardBasePage.h"

// ProgressMDSPP dialog

/**
	��������� ������� �������� ��������� ���������, 
	������������ �������� ��������.
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

	/// ����������� ��������� ��������� ��� ��������� �������
	afx_msg void OnSize(UINT nType, int cx, int cy);
	/// ��������� ������� ��������
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnStop();

	/**
		��������� ������, ��� ������������ �������� ����������� ������� ��������
		����������.
		����� ��� ��� ����, ����� ��������� ����� ���� ��������
	 */
	virtual BOOL OnSetActive();
	DECLARE_MESSAGE_MAP()

	/// ������������ ��������� �� ����� �������� ����������
	void PumpMessages();
private:
	CProgressCtrl m_wndProgress;
	CButton m_wndStop;
	bool m_bStopPressed;

};
