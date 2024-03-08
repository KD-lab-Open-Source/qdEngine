// MWAddAnimFirst.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "MWAddAnimFirst.h"
#include "addanimwizdata.h"
#include "AddAnimMWWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MWAddAnimFirst property page

IMPLEMENT_DYNCREATE(MWAddAnimFirst, WizardBasePage)

MWAddAnimFirst::MWAddAnimFirst() : WizardBasePage(MWAddAnimFirst::IDD)
{
	//{{AFX_DATA_INIT(MWAddAnimFirst)
	m_iway = -1;
	//}}AFX_DATA_INIT
	m_ptrPageForConvert = NULL;
	m_ptrPageForSelExisting = NULL;
}

MWAddAnimFirst::~MWAddAnimFirst()
{
}

void MWAddAnimFirst::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MWAddAnimFirst)
	DDX_Radio(pDX, IDC_RADIO_EXIST, m_iway);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MWAddAnimFirst, WizardBasePage)
	//{{AFX_MSG_MAP(MWAddAnimFirst)
	ON_BN_CLICKED(IDC_RADIO_EXIST, OnRadioExist)
	ON_BN_CLICKED(IDC_RADIO_CONV, OnRadioConv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MWAddAnimFirst message handlers

void MWAddAnimFirst::OnRadioExist() 
{
	static_cast<CPropertySheet*>(GetParent())->SetWizardButtons(PSWIZB_NEXT);
}

void MWAddAnimFirst::OnRadioConv() 
{
	static_cast<CPropertySheet*>(GetParent())->SetWizardButtons(PSWIZB_NEXT);
}

BOOL MWAddAnimFirst::OnSetActive() 
{
	ASSERT(m_ptrPageForConvert&&m_ptrPageForSelExisting);

	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* data = pw->data();
	m_iway = static_cast<int>(data->add_anim_way_);

	UpdateData(FALSE);
	if (m_iway == static_cast<int>(AAW_NONE)) 
		static_cast<CPropertySheet*>(GetParent())->SetWizardButtons(0);
	else
		static_cast<CPropertySheet*>(GetParent())->SetWizardButtons(PSWIZB_NEXT);
	return WizardBasePage::OnSetActive();
}

LRESULT MWAddAnimFirst::OnWizardNext() 
{
	UpdateData();
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());
	add_anim_wiz_data* data = pw->data();
	data->add_anim_way_ = static_cast<ADD_ANIM_WAY>(m_iway);
	if (m_iway == AAW_SELECT_EXISTING) 
	{
		m_ptrPageForSelExisting->set_prev_page(this);
		static_cast<CPropertySheet*>(GetParent())->
			SetActivePage(m_ptrPageForSelExisting);
	}
	else if (m_iway == AAW_CONVERT) 
	{
		static_cast<CPropertySheet*>(GetParent())->
			SetActivePage(m_ptrPageForConvert);
	}
	return 1;
}
