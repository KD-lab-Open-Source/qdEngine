// FileSelector.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "FileSelector.h"
#include "file_enumerator.h"
#include "listCtrl_insert_iterator.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FileSelector dialog

extern TCHAR __CURRENT_DIR__[];
extern TCHAR __PARENT_DIR__[];

namespace listCtrlInserter{

	/// Специализация свойств для listCtrl_insert_iterator
	template<>
	struct data_traits<file_find_data>{
		static int insert(CListCtrl* ctrl, int pos, const file_find_data& data){
			pos =  ctrl->InsertItem(pos, data.name());
			if (pos != -1){
				LVITEM lvi;
				lvi.iItem = pos;
				lvi.iSubItem = 0;
				lvi.mask = LVIF_IMAGE;
				SHFILEINFO shi;
				SHGetFileInfo(data.name(),0, &shi, sizeof(shi), 
										SHGFI_SMALLICON|SHGFI_SYSICONINDEX);

				lvi.iImage = shi.iIcon;
				ctrl->SetItem(&lvi);
			}
			return pos;
		}
	};
}

FileSelector::FileSelector(CWnd* pParent /*=NULL*/)
	: CDialog(FileSelector::IDD, pParent)
{
	//{{AFX_DATA_INIT(FileSelector)
	//}}AFX_DATA_INIT
	m_bTunneling = false;
	m_strFileExts = _T("*.*");
}


void FileSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FileSelector)
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileSelector, CDialog)
	//{{AFX_MSG_MAP(FileSelector)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FileSelector message handlers

bool FileSelector::SetPath(LPCTSTR lpszPath){
	m_bTunneling = true;
	m_tree.TunnelTree(lpszPath);

	HTREEITEM hSelected = m_tree.GetSelectedItem();
	m_tree.SelectItem(NULL);

	m_bTunneling = false;

	return true;
}

void FileSelector::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
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

bool FileSelector::FillList(const CString & strRoot)
{
	m_list.DeleteAllItems();
	SetCurrentDirectory(strRoot);

	m_vecFiles.clear();
	
	//выбираем поочереди все поддиректории
	file_enumerator fe(m_strFileExts);
	if(!fe.good())
		return false;

	typedef std::vector<file_find_data> ffd_vec_t;
	ffd_vec_t vecFiles;
	std::copy(fe.begin(), fe.end(), std::back_inserter(vecFiles));
	
	if(vecFiles.empty())
		return true;
	
	std::sort(vecFiles.begin(), vecFiles.end());

	ffd_vec_t::const_iterator i = vecFiles.begin(), e = vecFiles.end();
	std::copy(i, e, listCtrlInserter::listCtrl_inserter(m_list, 0));
		
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	return true;
}

void FileSelector::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (IsWindow(m_tree.m_hWnd)&&IsWindow(m_tree.m_hWnd))
	{
		CRect rcClient;
		GetClientRect(rcClient);

		CRect rcWnd(rcClient);
		rcWnd.DeflateRect(1, 1, (rcClient.Width()<<1)/3+1, 1);

		m_tree.MoveWindow(rcWnd);

		rcWnd.left = rcWnd.right + 1;
		rcWnd.right = rcClient.right - 1;

		m_list.MoveWindow(rcWnd);
	}
}

const CStringVect_t& FileSelector::GetFiles()const{
	if (m_vecFiles.empty())
		GetSelectedFiles();
	return m_vecFiles;
}


VOID FileSelector::GetSelectedFiles() const{
	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos) {
		int nItem = m_list.GetNextSelectedItem(pos);
		m_vecFiles.push_back(m_list.GetItemText(nItem, 0));
	}
}

VOID FileSelector::EnableImages(){
	m_tree.EnableImages();
	m_list.SetImageList(m_tree.GetImageList(TVSIL_NORMAL), LVSIL_SMALL);
}

void FileSelector::OnDestroy() 
{
	m_list.SetImageList(NULL, LVSIL_SMALL);
	CDialog::OnDestroy();
}

void FileSelector::OnCancel(){
}
