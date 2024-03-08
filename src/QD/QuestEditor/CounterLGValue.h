#pragma once


// CounterLGValue dialog

#include "CounterCondBase.h"
/**
	������� ������ ��� ������ ������������� ��������.
 */
class CounterLGValue : public CounterCondBase
{
	DECLARE_DYNAMIC(CounterLGValue)

public:
	CounterLGValue(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~CounterLGValue();

// Dialog Data
	enum { IDD = IDD_DLG_COND_COUNTER_LESS_GREATER_VALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	/// ����������� �������� � �������
	afx_msg void OnEnChangeEditValue();
	virtual BOOL OnInitDialog();
	virtual bool IsCondDataComplete();
private:
	/// ��������
	int m_iValue;
	bool m_bInitDialog;
};
