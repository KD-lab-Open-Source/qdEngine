// MWAACreateObjPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MWAACreateObjPage.h"

#include "addanimwizdata.h"
#include "AddAnimMWWizard.h"


// MWAACreateObjPage dialog

IMPLEMENT_DYNAMIC(MWAACreateObjPage, WizardBasePage)
MWAACreateObjPage::MWAACreateObjPage()
	: WizardBasePage(MWAACreateObjPage::IDD)
	, m_bCreate(FALSE)
{
}

MWAACreateObjPage::~MWAACreateObjPage()
{
}

void MWAACreateObjPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NOT_CREATE, m_bCreate);
}


BEGIN_MESSAGE_MAP(MWAACreateObjPage, WizardBasePage)
	ON_BN_CLICKED(IDC_RADIO_NOT_CREATE, OnBnClickedRadioNotCreate)
	ON_BN_CLICKED(IDC_RADIO_CREATE, OnBnClickedRadioCreate)
END_MESSAGE_MAP()


// MWAACreateObjPage message handlers

BOOL MWAACreateObjPage::OnSetActive()
{
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());	
	add_anim_wiz_data* data = pw->data();
	if (data->m_bUseGlobalCatalog){
		//если добавляем существующие или конвертируем папки,
		//то не можем добавить объекты, потому как сцена неизвестна
		GetDlgItem(IDC_RADIO_CREATE)->EnableWindow(TRUE);
		if (data->add_anim_way_ == AAW_SELECT_EXISTING
			||data->convert_way_ == CW_FOLDERS) 
		{
			GetDlgItem(IDC_RADIO_CREATE)->EnableWindow(FALSE);
			data->post_add_way_ = PAW_NOT_CREATE_OBJ;
		}
	}

	m_bCreate = static_cast<BOOL>(data->post_add_way_);
	UpdateData(FALSE);

	if (data->post_add_way_ == PAW_NONE)
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	return WizardBasePage::OnSetActive();
}

LRESULT MWAACreateObjPage::OnWizardNext()
{
	CPropertyPage *p = GET_PSHEET()->GetPage(GET_PSHEET()->GetActiveIndex()+1);

	static_cast<WizardBasePage*>(p)->set_prev_page(this);
	return WizardBasePage::OnWizardNext();
}

LRESULT MWAACreateObjPage::OnWizardBack()
{
	GET_PSHEET()->SetActivePage(m_ptrPrevPage);
	return 1;
}

void MWAACreateObjPage::OnBnClickedRadioNotCreate()
{
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());	
	add_anim_wiz_data* data = pw->data();
	data->post_add_way_ = PAW_NOT_CREATE_OBJ;
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

void MWAACreateObjPage::OnBnClickedRadioCreate()
{
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());	
	add_anim_wiz_data* data = pw->data();
	data->post_add_way_ = PAW_CREATE_OBJ ;

	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}
