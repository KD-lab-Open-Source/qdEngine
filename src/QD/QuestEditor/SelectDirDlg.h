#pragma once
#include "shelltree\shelltree.h"


// SelectDirDlg dialog
//��������� ������� ����������

/**
	������������� ����� ����� ������� �������� �� ����� disk_op::GetDiskFolder
	�� ��� ������, ������ ��� �� ������ �� ������� �������� � ������� ������� �������
	�������
 */
class SelectDirDlg : public CDialog
{
	DECLARE_DYNAMIC(SelectDirDlg)

public:
	SelectDirDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SelectDirDlg();

// Dialog Data
	enum { IDD = IDD_DLG_DIR_SELECT };

	/// ��������� ��������� ����
	const CString& GetPath()const{
		return m_strFullPath;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
private:
	/// ��������� ����
	CString m_strFullPath;

	CShellTree m_tree;

	/// ���� �� �������� ������ � ������
	bool m_bTunneling;
	/// ����� ��� CShellTree
	CString m_strRoot;
};
