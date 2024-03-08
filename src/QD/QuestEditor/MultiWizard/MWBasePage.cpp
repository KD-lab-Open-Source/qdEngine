// MWBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "../QuestEditor.h"
#include "MWBasePage.h"

#include "multiwizard.h"
#include "mwwizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MWBasePage property page

IMPLEMENT_DYNCREATE(MWBasePage, CPropertyPage)

MWBasePage::MWBasePage(UINT dlgIDD) : CPropertyPage(dlgIDD), m_ptrMWWizard(NULL)
{
	//{{AFX_DATA_INIT(MWBasePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

MWBasePage::~MWBasePage()
{
}

void MWBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MWBasePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MWBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(MWBasePage)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MWBasePage message handlers

/// ѕо-моему этот код не достиг своей цели
int MWBasePage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->x += 60;
	lpCreateStruct->y += 60;

	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
		
	return 0;
}

BOOL MWBasePage::OnSetActive() 
{
	MultiWizard* par = static_cast<MultiWizard*>(GetParent());
	par->AdjustPageLayout(this);
	m_ptrMWWizard->set_cur_page(this);
	return CPropertyPage::OnSetActive();
}

BOOL MWBasePage::OnWizardFinish() 
{
	MultiWizard* par = static_cast<MultiWizard*>(GetParent());
	par->OnEndWizard();
	return FALSE;
}
