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
	 ласс выводит диалог и позвол€ет выбрать несколько каталогов с диска.
	ƒл€ отображени€ дерева используетс€ CShellTree
 */
class SelectDirs : public CDialog
{
// Construction
	/// True - если идет заполнение дерева каталогами
	bool m_bTunneling;
	/// ”казатель на список, в который будут сложены выбранные каталоги
	CStringVect_t* m_ptrCatalogs;
	/// дл€ CShellTree. —одержит каталог, из которого выбираетс€ содержимое
	CString m_strRoot;
	/// ћожено ли выбрать несколько каталогов одновременно
	bool m_bSingleSelect;
public:
	SelectDirs(CStringVect_t* ptrStrings, CWnd* pParent = NULL);   // standard constructor

	/// ”станавливает, что можно выбрать только один каталог
	void set_single_select(){
		m_bSingleSelect = true;
	}
// Dialog Data
	//{{AFX_DATA(SelectDirs)
	enum { IDD = IDD_DLG_SEL_CATALOGS };
	/// ƒерево каталогов
	CShellTree	m_tree;
	/// —писок каталогов, в котором можно производить выбор
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
	/// ¬ыбирает из m_list все отмеченные галочкой и кладет их в m_ptrCatalogs
	void UpdateList();
	/// «аполн€ет список каталогов дл€ выбора m_list
	bool FillList(const CString &strRoot);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_SELECTDIRS_H__7585DAB9_C726_4FB1_9E76_8C5F8CA51A67__INCLUDED_)
