// MyPropertySheet.cpp : implementation file
//

#include "stdafx.h"

#include "QuestEditor.h"
#include "resource.h"
#include "BasePage.h"

#include "MyPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet

IMPLEMENT_DYNAMIC(CMyPropertySheet, CPropertySheet)

CMyPropertySheet::CMyPropertySheet(CPropertyPage** ptrPages, 
								   int iActive, DWORD dwCaption, CWnd* pWndParent)
	 : CPropertySheet(dwCaption, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().
	int i = 0;
	while (ptrPages[i]) {
		AddPage(ptrPages[i++]);
	}
	SetActivePage(iActive);
}


CMyPropertySheet::~CMyPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CMyPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMyPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet message handlers



LRESULT CMyPropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message == WM_SET_COORD_ANIMATION_MODE)
	{
		return OnSetCoordAnimationMode(wParam, lParam);
	}
	return CPropertySheet::WindowProc(message, wParam, lParam);
}

LRESULT CMyPropertySheet::OnSetCoordAnimationMode(WPARAM wParam, LPARAM lParam)
{
	if(GetParent()->SendMessage(WM_SET_COORD_ANIMATION_MODE, wParam, lParam))
	{
		GetParent()->EnableWindow(wParam);
		if(wParam)
			ShowWindow(SW_HIDE);
		else
			ShowWindow(SW_SHOW);
		return TRUE;
	}
	return 0;
}