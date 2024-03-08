// SelectDirs.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "SelectDirs.h"
#include <io.h>
#include "curdirsafer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelectDirs dialog
extern TCHAR __CURRENT_DIR__[];// = _T(".");
extern TCHAR __PARENT_DIR__[]; //= _T("..");


SelectDirs::SelectDirs(CStringVect_t* ptrStrings, CWnd* pParent /*=NULL*/)
	: CDialog(SelectDirs::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectDirs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bTunneling = false;
	m_ptrCatalogs = ptrStrings;
}


void SelectDirs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectDirs)
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectDirs, CDialog)
	//{{AFX_MSG_MAP(SelectDirs)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnLvnItemchangedList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectDirs message handlers

void SelectDirs::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if(m_tree.OnFolderSelected(pNMHDR, pResult, m_strRoot))
	{
		if(!m_bTunneling)
		{
			FillList(m_strRoot);
		}
	}
	*pResult = 0;
}

BOOL SelectDirs::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_list.SetExtendedStyle(m_list.GetExtendedStyle()|LVS_EX_CHECKBOXES);	
	
	m_tree.EnableImages();
	m_tree.PopulateTree();
	m_bTunneling = true;
	m_tree.TunnelTree(CurDirSafer::GetCurrentDirectory());

	HTREEITEM hSelected = m_tree.GetSelectedItem();
	m_tree.SelectItem(NULL);

	m_bTunneling = false;

	m_list.InsertColumn(0, CString((LPTSTR)IDS_CLMN_DIR_NAMES));
	m_tree.SelectItem(hSelected);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool SelectDirs::FillList(const CString & strRoot)
{
	m_list.DeleteAllItems();
	SetCurrentDirectory(strRoot);

	//выбираем поочереди все поддиректории
	_finddata_t fData;
	LONG hFile = _findfirst(_T("*.*"),&fData);
	if(hFile != -1L)
	{
		int nPos = 0;
		do{
			if(fData.attrib&_A_SUBDIR)
			{
				if(strcmp(fData.name, __CURRENT_DIR__)
					&& strcmp(fData.name, __PARENT_DIR__))
				{
					m_list.InsertItem(nPos, fData.name);
					++nPos;
				}
			}
		}while(_findnext(hFile, &fData)==0);
		_findclose( hFile );
	}
	
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	return true;
}

void SelectDirs::OnOK() 
{
	UpdateList();	
	CDialog::OnOK();
}

void SelectDirs::UpdateList()
{
	m_ptrCatalogs->clear();
	m_ptrCatalogs->push_back(m_strRoot);

	int nCount = m_list.GetItemCount();
	for(int nPos = 0; nPos < nCount; ++nPos)
	{
		if(ListView_GetCheckState(m_list.m_hWnd,  nPos))
			m_ptrCatalogs->push_back(m_list.GetItemText(nPos, 0));
	}
	if(m_ptrCatalogs->size() == 1)
		m_ptrCatalogs->clear();
}

void SelectDirs::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateList();
	GetDlgItem(IDOK)->EnableWindow(!m_ptrCatalogs->empty());
	*pResult = 0;
}
