#pragma once
#include "afxwin.h"


// CounterGreaterCounterCondDlg dialog

#include "CounterCondBase.h"
/**
	���� ������� ������ ������� 
 */
class CounterGreaterCounterCondDlg : public CounterCondBase
{
	DECLARE_DYNAMIC(CounterGreaterCounterCondDlg)

public:
	CounterGreaterCounterCondDlg(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~CounterGreaterCounterCondDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_COUNTER_GREATER_COUNTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	/// ����������� ������ ������� � ������� � �������� ��������
	afx_msg void OnCbnSelchangeComboSecondCounter();

	DECLARE_MESSAGE_MAP()

	bool IsCondDataComplete();
private:
	/// ��� ���� ������ ���������
	CCJFlatComboBox m_wndCounters2;
};
