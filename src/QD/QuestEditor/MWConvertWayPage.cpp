// MWConvertWayPage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "MWConvertWayPage.h"

#include "addanimwizdata.h"
#include "AddAnimMWWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MWConvertWayPage property page

IMPLEMENT_DYNCREATE(MWConvertWayPage, WizardBasePage)

MWConvertWayPage::MWConvertWayPage() : WizardBasePage(MWConvertWayPage::IDD)
{
	//{{AFX_DATA_INIT(MWConvertWayPage)
	m_iConvertWay = -1;
	//}}AFX_DATA_INIT
	m_pNextPage = NULL;
}

MWConvertWayPage::~MWConvertWayPage()
{
}

void MWConvertWayPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MWConvertWayPage)
	DDX_Radio(pDX, IDC_RADIO_CONVERT_1, m_iConvertWay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MWConvertWayPage, WizardBasePage)
	//{{AFX_MSG_MAP(MWConvertWayPage)
	ON_BN_CLICKED(IDC_RADIO_CONVERT_1, OnRadioConvert1)
	ON_BN_CLICKED(IDC_RADIO_CONVERT_2, OnRadioConvert2)
	ON_BN_CLICKED(IDC_RADIO_CONVERT3, OnRadioConvert3)
	ON_BN_CLICKED(IDC_RADIO_CONVERT4, OnRadioConvert4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MWConvertWayPage message handlers

void MWConvertWayPage::OnRadioConvert1() 
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

void MWConvertWayPage::OnRadioConvert2() 
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

void MWConvertWayPage::OnRadioConvert3() 
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

void MWConvertWayPage::OnRadioConvert4() 
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

BOOL MWConvertWayPage::OnSetActive() 
{
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* data = pw->data();
	m_iConvertWay = static_cast<int>(data->convert_way_);

	UpdateData(FALSE);
	if (data->convert_way_ == CW_NONE)
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	return WizardBasePage::OnSetActive();
}

LRESULT MWConvertWayPage::OnWizardNext() 
{
	UpdateData();

	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* data = pw->data();
	data->convert_way_ = static_cast<CONVERT_WAY>(m_iConvertWay);
	m_pNextPage->set_prev_page(this);	
	return WizardBasePage::OnWizardNext();
}

LRESULT MWConvertWayPage::OnWizardBack() 
{
	GET_PSHEET()->SetActivePage(m_ptrPrevPage);
	return 1;//WizardBasePage::OnWizardBack();
}
