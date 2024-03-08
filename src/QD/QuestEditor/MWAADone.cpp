// MWAADone.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MWAADone.h"
#include "addanimwizdata.h"
#include "AddAnimMWWizard.h"

// MWAADone dialog

IMPLEMENT_DYNAMIC(MWAADone, WizardBasePage)
MWAADone::MWAADone()
	: WizardBasePage(MWAADone::IDD)
{
}

MWAADone::~MWAADone()
{
}

void MWAADone::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MWAADone, WizardBasePage)
END_MESSAGE_MAP()


// MWAADone message handlers

BOOL MWAADone::OnSetActive()
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	static_cast<AddAnimMWWizard*>(wizard())->data()->files_.clear();
	static_cast<AddAnimMWWizard*>(wizard())->set_changes(true);
	return WizardBasePage::OnSetActive();
}

BOOL MWAADone::OnWizardFinish()
{
	return WizardBasePage::OnWizardFinish();
}

LRESULT MWAADone::OnWizardBack()
{
	GET_PSHEET()->SetActivePage(m_ptrPrevPage);
	return 1;
}
