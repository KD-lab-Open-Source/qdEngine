#if !defined(AFX_FILESELECTOR_H__AD4FA454_B170_4F9C_93CA_A33CE4B7AAC3__INCLUDED_)
#define AFX_FILESELECTOR_H__AD4FA454_B170_4F9C_93CA_A33CE4B7AAC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileSelector.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FileSelector dialog

#include "shelltree\shelltree.h"

/// \brief ������ ������� �� ����� ����� ������ ��������� � ����� SHELL, 
/// � ������ ������ ������ � ��������� ��������
class FileSelector : public CDialog
{
// Construction
	/// ��������� � true ���� ���� ���������� ������
	mutable bool m_bTunneling;
	/// ������ ��������� ������
	mutable CStringVect_t m_vecFiles;
	/// �������� ��������� � ������ �������
	CString m_strRoot;
	/// ������ ����������
	CString m_strFileExts;
public:
	FileSelector(CWnd* pParent = NULL);   // standard constructor

	/// ������������� ���� ������, ������� ����� ������������
	bool SetFileExts(LPCTSTR lpszExt){
		m_strFileExts = lpszExt;
		return true;
	}
	
	/// ������������� ����, ������� ����� ������������
	bool SetPath(LPCTSTR lpszPath);
	
	
	/// ���������� ��������� � ������ ������ �������
	CString GetRootDir() const{
		return m_strRoot;
	}

	VOID EnableImages();
	
	// �������� ����� ��������, ��������� ������ ���������� �������
	VOID PopulateTree() {
		m_tree.PopulateTree();
	}

	/// ���������� ��������� �����
	const CStringVect_t& GetFiles()const;
// Dialog Data
	//{{AFX_DATA(FileSelector)
	enum { IDD = IDD_PAGE_SELECT_FILES };
	CShellTree	m_tree;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FileSelector)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FileSelector)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
	/// ��������� ������ ������� ������
	bool FillList(const CString & strRoot);
	/// ���������� ��������� ����� � m_vecFiles
	VOID GetSelectedFiles() const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //!defined(AFX_FILESELECTOR_H__AD4FA454_B170_4F9C_93CA_A33CE4B7AAC3__INCLUDED_)
