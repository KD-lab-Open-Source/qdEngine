#pragma once


// CounterInIntervalCondDlg dialog
#include "CounterCondBase.h"

/**
	Счетчик в интервале
 */
class CounterInIntervalCondDlg : public CounterCondBase
{
	DECLARE_DYNAMIC(CounterInIntervalCondDlg)

public:
	CounterInIntervalCondDlg(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~CounterInIntervalCondDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_COUNTER_IN_INTERVAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditValue();
	afx_msg void OnEnChangeEditGrValue();
	afx_msg void OnDeltaposSpinGrValue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinValue(NMHDR *pNMHDR, LRESULT *pResult);

	bool IsCondDataComplete();
private:
	/// Идет инициализация диалога
	bool m_bInitDialog;
	/// Минимальное значение
	int  m_iLessValue;
	/// Максимальное значение
	int m_iGrValue;
};
