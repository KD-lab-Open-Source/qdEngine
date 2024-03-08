#pragma once

#include "qd_condition.h"
#include "CondDlgBase.h"
#include "afxwin.h"

// WTGreateWTCond dialog

/**
	������� ����� ������ ���������, ������ �������� ������� ������� ���������
 */
class WTGreateWTCond : public CondDlgBase
{
	DECLARE_DYNAMIC(WTGreateWTCond)

public:
	WTGreateWTCond(DWORD id, CWnd* pParent = NULL);   // standard constructor
	virtual ~WTGreateWTCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_WORK_TIME_GREATE_WORK_TIME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	/// ����������� ������
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	/// ������ � ������� ������ ���������
	afx_msg void OnCbnSelchangeComboFirstState();
	/// ������ � ������� ������ ���������
	afx_msg void OnCbnSelchangeComboSecondState();
	/// ��������� ��������� � ������ m_wndFirstState
	afx_msg void OnCbnSelchangeComboFirstStateScene();
	/// ��������� ��������� � ������ m_wndSecondState
	afx_msg void OnCbnSelchangeComboSecondStateScene();

	DECLARE_MESSAGE_MAP()

	bool IsCondDataComplete();
private:
	CCJFlatComboBox m_wndFirstState;
	CCJFlatComboBox m_wndSecondState;
	CCJFlatComboBox m_wndFirstStateScene;
	CCJFlatComboBox m_wndSecondStateScene;
};
