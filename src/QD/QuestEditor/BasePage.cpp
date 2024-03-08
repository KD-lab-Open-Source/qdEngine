// BasePage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "BasePage.h"
#include "sv_api.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* ������� �����������                                                  */
/************************************************************************/
void SetProjChange();

/////////////////////////////////////////////////////////////////////////////
// CBasePage property page

IMPLEMENT_DYNCREATE(CBasePage, CPropertyPage)

CBasePage::CBasePage() {
	m_bChage = false;
}

CBasePage::CBasePage(UINT dlgID, UINT nIdCap) : CPropertyPage(dlgID, nIdCap)
{
	//{{AFX_DATA_INIT(CBasePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bChage = false;
}

CBasePage::~CBasePage()
{
}

void CBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBasePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CBasePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasePage message handlers


BOOL CBasePage::OnApply() 
{
	if(m_bChage)
		SetProjChange();	
	return CPropertyPage::OnApply();
}

void CBasePage::OnOK() 
{
	if(m_bChage)
		SetProjChange();	
	CPropertyPage::OnOK();
}

//! ����������/���������� ����, ��� ������ ��������
void CBasePage::SetModified(BOOL bChanged)
{
	m_bChage = bChanged;
	CPropertyPage::SetModified(bChanged);
}

BOOL CBasePage::OnWizardFinish()
{
	if(m_bChage)
		SetProjChange();	
	return CPropertyPage::OnWizardFinish();
}

//! �������������� ����� � ��������� �����
/*!
  \param bRedrawScene - ��������� ����� ������ ��� ������ ������ ������� ������
*/

void CBasePage::redraw_scene(bool bRedrawScene)
{
	sv_api::Redraw(bRedrawScene ? 
					sv_api::RM_REDRAW_CONTENT 
				  : sv_api::RM_NO_CONTENT_REDRAW);
}

//! \brief ����������� ������ ������� � ��������� ����� �� ������� ����� ��� ����,
//! ���� ���� ����
void CBasePage::AdjustBitmapSize()
{
	sv_api::AdjustBitmapSize();
}

