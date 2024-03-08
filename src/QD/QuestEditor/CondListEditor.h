#pragma once
#include "afxwin.h"

#include "condition.h"

// CCondListEditor dialog

class CCondListEditor : public CDialog
{
	DECLARE_DYNAMIC(CCondListEditor)

public:
	CCondListEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCondListEditor();

// Dialog Data
	enum { IDD = IDD_DLG_CONDITION_LIST };

	void SwapLines(int i, int j);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//! ������ ������ ��������� � ����������� � ����������� �� ��������� ������ � ListBox
	void UpdateButtons();
	//! ������� �������� ��� ������� ���������� ������� ������ �������� �������
	void TryLiftUp();
	void TryLiftDown();
	
	//! ������������� ����� ������������
	Permutation modifPermut_;

	//! ������ �������� �������
	CListBox m_listBox; 
	CButton m_upButton;
	CButton m_downButton;
	CButton m_fastUpButton;
	CButton m_fastDownButton;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnLbnSelchangeListConditions();
	afx_msg void OnLbnSelcancelListConditions();
	afx_msg void OnBnClickedButtonFastUp();
	afx_msg void OnBnClickedButtonFastDown();
protected:
	virtual void OnOK();
};
