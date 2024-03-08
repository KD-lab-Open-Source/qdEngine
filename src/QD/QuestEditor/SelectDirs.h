#if !defined(AFX_SELECTDIRS_H__7585DAB9_C726_4FB1_9E76_8C5F8CA51A67__INCLUDED_)
#define AFX_SELECTDIRS_H__7585DAB9_C726_4FB1_9E76_8C5F8CA51A67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectDirs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectDirs dialog
#include "shelltree\shelltree.h"
/**
	����� ������� ������ � ��������� ������� ��������� ��������� � �����.
	��� ����������� ������ ������������ CShellTree
 */
class SelectDirs : public CDialog
{
// Construction
	/// True - ���� ���� ���������� ������ ����������
	bool m_bTunneling;
	/// ��������� �� ������, � ������� ����� ������� ��������� ��������
	CStringVect_t* m_ptrCatalogs;
	/// ��� CShellTree. �������� �������, �� �������� ���������� ����������
	CString m_strRoot;
	/// ������ �� ������� ��������� ��������� ������������
	bool m_bSingleSelect;
public:
	SelectDirs(CStringVect_t* ptrStrings, CWnd* pParent = NULL);   // standard constructor

	/// �������������, ��� ����� ������� ������ ���� �������
	void set_single_select(){
		m_bSingleSelect = true;
	}
// Dialog Data
	//{{AFX_DATA(SelectDirs)
	enum { IDD = IDD_DLG_SEL_CATALOGS };
	/// ������ ���������
	CShellTree	m_tree;
	/// ������ ���������, � ������� ����� ����������� �����
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectDirs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectDirs)
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// �������� �� m_list ��� ���������� �������� � ������ �� � m_ptrCatalogs
	void UpdateList();
	/// ��������� ������ ��������� ��� ������ m_list
	bool FillList(const CString &strRoot);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SELECTDIRS_H__7585DAB9_C726_4FB1_9E76_8C5F8CA51A67__INCLUDED_)
