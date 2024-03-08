// WizardBasePage.cpp: implementation of the WizardBasePage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "WizardBasePage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(WizardBasePage, MWBasePage)

WizardBasePage::WizardBasePage(): m_ptrPrevPage(NULL)
{

}

WizardBasePage::WizardBasePage(UINT dlgIDD) : 
MWBasePage(dlgIDD), m_ptrPrevPage(NULL)
{
}

WizardBasePage::~WizardBasePage()
{

}

VOID WizardBasePage::set_prev_page(WizardBasePage* ptr){
	m_ptrPrevPage = ptr;
}

