// TreeListEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListEditCtrl.h"
#include "TreeListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditCtrl

CTreeListEditCtrl::CTreeListEditCtrl()  
{
}

CTreeListEditCtrl::~CTreeListEditCtrl()
{
}

BEGIN_MESSAGE_MAP(CTreeListEditCtrl, CTreeListEditCtrl::Base)
	//{{AFX_MSG_MAP(CTreeListEditCtrl)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListEditCtrl message handlers
int CTreeListEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (Base::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	setTreeListCtrl(reinterpret_cast<CTreeListCtrl*>(GetParent()));
	return 0;
}

void CTreeListEditCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	Base::OnKillFocus(pNewWnd);
}
