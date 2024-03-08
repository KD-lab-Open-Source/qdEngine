// WizardSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WizardSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WizardSheet

IMPLEMENT_DYNAMIC(WizardSheet, CPropertySheet)

WizardSheet::WizardSheet(CPropertyPage** ptrPPs, int count, int iActive,
						 UINT uiCap, CWnd* pWndParent)
: CPropertySheet(uiCap, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().
	
	m_ptrPages = ptrPPs;
	m_nCount = count;
	//	AddPage(&m_Page1);
	for(int i = 0; i < count; ++i)
	{
		AddPage(ptrPPs[i]);
		//AddStep(ptrPPs[i]);
	}
	SetActivePage(iActive);

	SetWizardMode();
	m_bShowSteps = false;
}

WizardSheet::~WizardSheet()
{
}


BEGIN_MESSAGE_MAP(WizardSheet, CPropertySheet)
//{{AFX_MSG_MAP(WizardSheet)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// WizardSheet message handlers

BOOL WizardSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	if (m_bShowSteps) 
	{
		// add a preview window to the property sheet.
		CRect rectWnd;
		GetWindowRect(rectWnd);
		SetWindowPos(NULL, 0, 0,
			rectWnd.Width() + 100,
			rectWnd.Height(),
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		CRect rectTab;
		GetTabControl()->GetWindowRect(&rectTab);
		GetTabControl()->SetWindowPos(0, rectTab.left + 100, rectTab.top, 0, 0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);


		CRect rectPreview(rectWnd.Width() + 25, 25,
			rectWnd.Width()+75, 75);
		
	//	m_wndPreview.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
	//		rectPreview, this, 0x1000);
	
	}
	GetDlgItem(ID_WIZBACK)->EnableWindow(FALSE);
	GetDlgItem(ID_WIZNEXT)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->SetWindowText("Закрыть");

	CenterWindow();

	return TRUE;
}

