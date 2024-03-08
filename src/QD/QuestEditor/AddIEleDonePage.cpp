// AddIEleDonePage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "AddIEleDonePage.h"
#include "MultiWizard/MWWizard.h"


// AddIEleDonePage dialog

IMPLEMENT_DYNAMIC(AddIEleDonePage, WizardBasePage)
AddIEleDonePage::AddIEleDonePage()
	: WizardBasePage(AddIEleDonePage::IDD)
	, m_strMessage(_T(""))
{
}

AddIEleDonePage::~AddIEleDonePage()
{
}

void AddIEleDonePage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC1, m_strMessage);
}


BEGIN_MESSAGE_MAP(AddIEleDonePage, WizardBasePage)
END_MESSAGE_MAP()


// AddIEleDonePage message handlers

LRESULT AddIEleDonePage::OnWizardBack()
{
	GET_PSHEET()->SetActivePage(m_ptrPrevPage);
	return 1;
}

BOOL AddIEleDonePage::OnWizardFinish()
{
	return WizardBasePage::OnWizardFinish();
}

BOOL AddIEleDonePage::OnSetActive()
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	if (m_ptrObjFabric->create_obj())
	{
		m_strMessage.LoadString(IDS_ADD_SUCCESSFUL);
		wizard()->set_changes(true);
	}
	else
		m_strMessage.LoadString(IDS_ADD_FAIL);
	UpdateData(FALSE);
	return WizardBasePage::OnSetActive();
}

void AddIEleDonePage::set_obj_fabric(obj_fabric* pof)
{
	m_ptrObjFabric = pof;
}
