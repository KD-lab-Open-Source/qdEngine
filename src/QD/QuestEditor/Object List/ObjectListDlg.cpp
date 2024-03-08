// Object List\ObjectListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\QuestEditor.h"
#include "ObjectListDlg.h"
#include ".\objectlistdlg.h"
#include "ObjectListImpl.h"


// ObjectListDlg dialog

IMPLEMENT_DYNAMIC(ObjectListDlg, CDialog)
ObjectListDlg::ObjectListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ObjectListDlg::IDD, pParent)
{
}

ObjectListDlg::~ObjectListDlg()
{
}

void ObjectListDlg::setObjectListImpl(ObjectListImpl* objListImpl){
	objectListImpl_ = objListImpl;
	m_list.setItemOperationNotifyListener(objectListImpl_);
	m_list.setGeneralNotifyListener(objectListImpl_);

}

void ObjectListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJECT_LIST, m_list);
}


BEGIN_MESSAGE_MAP(ObjectListDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

QETreeListCtrl& ObjectListDlg::getList() {
	return m_list;
}

// ObjectListDlg message handlers

BOOL ObjectListDlg::OnInitDialog()
{
	if (!__super::OnInitDialog())
		return FALSE;

	m_list.SetStyle( 0
		| TLC_TREELIST
		| TLC_TREELINE
		| TLC_ROOTLINE
		| TLC_BUTTON
		| TLC_DOUBLECOLOR							// double color background
		| TLC_HEADER
		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode

		| TLC_HGRID									// show horizonal grid lines
		| TLC_VGRID									// show vertical grid lines
		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
		| TLC_HGRID_EXT								// show extention horizonal grid lines
		| TLC_VGRID_EXT								// show extention vertical grid lines
		| TLC_HGRID_FULL							// show full horizonal grid lines
		| TLC_READONLY								// read only

		| TLC_HOTTRACK								// show hover text 
		);
	return TRUE;
}

void ObjectListDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (IsWindow(m_list.m_hWnd))
		adjustListSize();
}

void ObjectListDlg::adjustListSize()
{
	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(2, 2);
	m_list.MoveWindow(rc);
}

BOOL ObjectListDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (objectListImpl_->OnCommand(wParam, lParam))
		return TRUE;
	return __super::OnCommand(wParam, lParam);
}

BOOL ObjectListDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	return __super::OnNotify(wParam, lParam, pResult);
}

void ObjectListDlg::OnCancel()
{
}
