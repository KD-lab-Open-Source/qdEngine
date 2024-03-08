// TextIDSelectorDLG.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "TextIDSelectorDLG.h"
#include ".\textidselectordlg.h"
#include "TextIDSelector.h"

#include "qd_textdb.h"

#include <boost/rtl.hpp>
#include <boost/tokenizer.hpp>

// TextIDSelectorDLG dialog

bool TextIdSelector::run(HWND hParent, qdTextDB const* ptrTextDB, CString& textID)
{
	TextIDSelectorDLG dlg;
	dlg.setTextDB(ptrTextDB);
	dlg.setCurrentId(textID);
	if (dlg.DoModal() == IDOK)
	{
		textID = dlg.getSelectedId();
		return true;
	}
	return false;
}


IMPLEMENT_DYNAMIC(TextIDSelectorDLG, CDialog)
TextIDSelectorDLG::TextIDSelectorDLG(CWnd* pParent /*=NULL*/)
	: CDialog(TextIDSelectorDLG::IDD, pParent)
	, textPreview_(_T(""))
	, ptrTextDB_(NULL)
{
}

TextIDSelectorDLG::~TextIDSelectorDLG()
{
}

void TextIDSelectorDLG::setTextDB(qdTextDB const* ptrTextDB){
	ptrTextDB_ = ptrTextDB;
}

void TextIDSelectorDLG::setCurrentId(CString const& textId){
	currentId_ = textId;
}

CString const TextIDSelectorDLG::getSelectedId() const{
	return currentId_;
}

void TextIDSelectorDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT_VIEW, textPreview_);
	DDX_Control(pDX, IDC_TREE, tree_);
}


BEGIN_MESSAGE_MAP(TextIDSelectorDLG, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE, OnTvnGetdispinfoTree)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL TextIDSelectorDLG::OnInitDialog()
{
	__super::OnInitDialog();

	CWaitCursor wait;
	if (!loadRootIds())
		return FALSE;
	insertNOTEXTItem();
	showCurrentId();

	return TRUE;
}


// TextIDSelectorDLG message handlers

void TextIDSelectorDLG::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView->itemNew.hItem)
	{
		currentId_ = getIDMask(pNMTreeView->itemNew.hItem);
		textPreview_ = ptrTextDB_->getText(currentId_);
		UpdateData(FALSE);
	}
	*pResult = 0;
}

CString const TextIDSelectorDLG::getIDMask(HTREEITEM hItem) const
{
	CString res;
	do {
		CString text = tree_.GetItemText(hItem);
		if (res.IsEmpty())
			res = text;
		else
			res = text + _T('.') + res;
	} while(hItem = tree_.GetParentItem(hItem));
	return res;
}

void TextIDSelectorDLG::OnTvnGetdispinfoTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	if ((pTVDispInfo->item.mask&TVIF_CHILDREN))
	{
		CWaitCursor wait;
		CString const mask = getIDMask(pTVDispInfo->item.hItem);
		qdTextDB::IdList list;
		ptrTextDB_->getIdList(mask, list);
		loadIds(list, pTVDispInfo->item.hItem);
		TVITEM item;
		item.mask = TVIF_DI_SETITEM|TVIF_CHILDREN;
		item.cChildren = (list.size()>0);
		item.hItem = pTVDispInfo->item.hItem;
		tree_.SetItem(&item);
		CRect rc;
		tree_.GetItemRect(pTVDispInfo->item.hItem, rc, FALSE);
		tree_.InvalidateRect(rc);

	}

	*pResult = 0;
}

bool TextIDSelectorDLG::loadRootIds()
{
	qdTextDB::IdList idList;
	if (!ptrTextDB_->getRootIdList(idList))
		return false;

	return loadIds(idList, TVI_ROOT);
}

bool TextIDSelectorDLG::loadIds(qdTextDB::IdList const& idList, HTREEITEM hParent)
{
	boost::rtl::crange<const qdTextDB::IdList> ids(idList);
	TVINSERTSTRUCT insertItem;
	memset(&insertItem, 0, sizeof(TVINSERTSTRUCT));
	insertItem.hParent = hParent;
	insertItem.hInsertAfter = TVI_SORT;
	insertItem.item.mask = TVIF_CHILDREN|TVIF_PARAM|TVIF_TEXT;
	insertItem.item.cChildren = I_CHILDRENCALLBACK;
	for(; ids; ++ids)
	{
		TVITEM &item = insertItem.item;
		item.pszText = const_cast<LPSTR>((*ids).c_str());

		tree_.InsertItem(&insertItem);
	}
	return true;
}

void TextIDSelectorDLG::showCurrentId() 
{
	typedef boost::tokenizer<
		boost::char_separator<TCHAR>,
		LPCTSTR> tokenizer;
	boost::char_separator<TCHAR> sep(_T("."));

	CString const id(currentId_);
	LPCTSTR const begin(id);
	LPCTSTR const end(begin + id.GetLength());
	boost::rtl::crange<const tokenizer> tokens(
					tokenizer((LPCTSTR)id, (LPCTSTR)id + id.GetLength(), sep));
	HTREEITEM hParent = TVI_ROOT;
	for(; tokens; ++tokens)
	{
		hParent = findItemByText((*tokens).c_str(), hParent);
		if (hParent)
		{
			tree_.EnsureVisible(hParent);
			tree_.Expand(hParent, TVE_EXPAND);
			tree_.SelectItem(hParent);
		}
	}
}

HTREEITEM TextIDSelectorDLG::findItemByText(CString const& text, HTREEITEM hParent) const
{
	HTREEITEM hChild = tree_.GetChildItem(hParent);
	if (!hChild)
		return NULL;
	do {
		if (text == tree_.GetItemText(hChild))
			return hChild;
	} while(hChild = tree_.GetNextSiblingItem(hChild));
	return NULL;
}

void TextIDSelectorDLG::insertNOTEXTItem()
{
	CString str;
	str.LoadString(IDS_NOTHING);
	tree_.InsertItem(str, TVI_ROOT, TVI_FIRST);
}


void TextIDSelectorDLG::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	int const LEFT_OFFSET = 7;
	int const RIGHT_OFFSET = 7;
	int const TOP_OFFSET = 7;
	int const BOTTOM_OFFSET = 7;

	CRect clientRect;
	GetClientRect(clientRect);

	CRect ctrlResultRect;
	ctrlResultRect = clientRect;

	CWnd* ctrl = GetDlgItem(IDC_TEXT_VIEW);
	if (!ctrl)
		return;
	CRect ctrlRect;
	ctrl->GetWindowRect(ctrlRect);
	ScreenToClient(ctrlRect);
	ctrlResultRect.left += LEFT_OFFSET;
	ctrlResultRect.right -= RIGHT_OFFSET;
	ctrlResultRect.top += TOP_OFFSET;
	ctrlResultRect.bottom = ctrlResultRect.top + ctrlRect.Height();
	ctrl->MoveWindow(ctrlResultRect, FALSE);

	ctrl = GetDlgItem(IDCANCEL);
	ctrl->GetWindowRect(ctrlRect);
	ScreenToClient(ctrlRect);
	ctrlResultRect = clientRect;
	ctrlResultRect.bottom -= BOTTOM_OFFSET;
	ctrlResultRect.right -= RIGHT_OFFSET;
	ctrlResultRect.top = ctrlResultRect.bottom - ctrlRect.Height();
	ctrlResultRect.left = ctrlResultRect.right - ctrlRect.Width();
	ctrl->MoveWindow(ctrlResultRect, FALSE);

	ctrl = GetDlgItem(IDOK);
	ctrl->GetWindowRect(ctrlRect);
	ScreenToClient(ctrlRect);
	ctrlResultRect.OffsetRect(-ctrlRect.Width() - 2, 0);
	ctrl->MoveWindow(ctrlResultRect, FALSE);

	// дерево
	ctrl = GetDlgItem(IDC_TEXT_VIEW);
	ctrl->GetWindowRect(ctrlResultRect);
	ScreenToClient(ctrlResultRect);

	ctrlResultRect.OffsetRect(0, ctrlResultRect.Height() + 1);
	ctrlResultRect.bottom = ctrlRect.top - 4;
	tree_.MoveWindow(ctrlResultRect, FALSE);
	Invalidate();
}
