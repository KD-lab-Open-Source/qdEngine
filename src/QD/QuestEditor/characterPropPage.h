#if !defined(AFX_CharacterGaitsPage_H__0548E911_31C4_4CC4_9350_0055806DA0C2__INCLUDED_)
#define AFX_CharacterGaitsPage_H__0548E911_31C4_4CC4_9350_0055806DA0C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CharacterGaitsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CharacterGaitsPage dialog

class qdGameObjectMoving;
class qdGameDispatcher;

#include "basepage.h"
#include "editlistctrl.h"

/*!
  ����� �������� ������� ���������, ���������� � ���������
*/

class CharacterGaitsPage : public CBasePage
{
	DECLARE_DYNCREATE(CharacterGaitsPage);

	//! ���������� � true �� ������� ������ ����
	bool m_bInitDialog;
	//! ��������� ����
	qdGameDispatcher *m_ptrDisp;
	//! ������, � ��������� �������� ��������
	qdGameObjectMoving * m_ptrObj;

// Construction
public:
	CharacterGaitsPage();
	~CharacterGaitsPage();

// Dialog Data
	//{{AFX_DATA(CharacterGaitsPage)
	enum { IDD = IDD_PROPPAGE_OBJECT_ANIM_STATES };
	//! ������ �������
	CEditListCtrl	m_list;
	//}}AFX_DATA
	//! ��������� ��������� ���� � ������
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj);
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CharacterGaitsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL OnApply();
	//! ��������� �� ����������� �� ���. ���� ���, �� ��������� ���
	bool TestName(int iItem, int iSubItem, LPCTSTR lpszText);
	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CharacterGaitsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	//! ������� �������� �������
	afx_msg void OnAddState();
	//! ������� ��������� �������
	afx_msg void OnChangeState();
	//! ������� ������� �������
	afx_msg void OnDelState();
	afx_msg void OnEndItemEditList1(NMHDR* pNMHDR, LRESULT* pResult);
	//! ������� ������������� �������
	afx_msg void OnBtnCond();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	//! ��������� ������ ������� � �������
	bool LoadStates();
	//! ��������� ������� � ������
	bool AddState2List(int iIndex, qdGameObjectStateWalk* ptrState);
	//! ��� ��������� ������� ��������� ������� ������� �������� 
	void UpdateState(int iIndex, qdGameObjectStateWalk* ptrState);
	//! ���������� ������ ������� ���������
	bool ShowEditStateDlg(qdGameObjectStateWalk* ptrState);
	//! ����������� ������ �������� �� ������
	void AdjustColumnWidth();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_CharacterGaitsPage_H__0548E911_31C4_4CC4_9350_0055806DA0C2__INCLUDED_)
