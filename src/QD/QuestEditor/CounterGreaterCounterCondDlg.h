#pragma once
#include "afxwin.h"


// CounterGreaterCounterCondDlg dialog

#include "CounterCondBase.h"
/**
	Один Счетчик больше другого 
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
	/// Прописывает второй счетчик в условие и вызывает проверку
	afx_msg void OnCbnSelchangeComboSecondCounter();

	DECLARE_MESSAGE_MAP()

	bool IsCondDataComplete();
private:
	/// Еще один список коунтеров
	CCJFlatComboBox m_wndCounters2;
};
