// SelectDirDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "SelectDirDlg.h"


// SelectDirDlg dialog

IMPLEMENT_DYNAMIC(SelectDirDlg, CDialog)
SelectDirDlg::SelectDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SelectDirDlg::IDD, pParent)
	, m_strFullPath(_T("")), m_bTunneling(false)
{
	
}

SelectDirDlg::~SelectDirDlg()
{
}

void SelectDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Text(pDX, IDC_EDIT, m_strFullPath);
}


BEGIN_MESSAGE_MAP(SelectDirDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
END_MESSAGE_MAP()


// SelectDirDlg message handlers

void SelectDirDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if(m_tree.OnFolderSelected(pNMHDR, pResult, m_strRoot))
	{
		if(!m_bTunneling)
		{
			m_tree.GetSelectedFolderPath(m_strFullPath);
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}

BOOL SelectDirDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_tree.EnableImages();
	m_tree.PopulateTree();

	m_bTunneling = true;
	TCHAR buff[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buff);
	m_tree.TunnelTree(buff);
	m_bTunneling = false;

	HTREEITEM hSelected = m_tree.GetSelectedItem();
	m_tree.SelectItem(NULL);
	m_tree.SelectItem(hSelected);

	return TRUE;  
}
