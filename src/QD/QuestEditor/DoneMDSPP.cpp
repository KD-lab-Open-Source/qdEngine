// DoneMDSPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "DoneMDSPP.h"


// DoneMDSPP dialog

IMPLEMENT_DYNAMIC(DoneMDSPP, WizardBasePage)
DoneMDSPP::DoneMDSPP()
	: WizardBasePage(DoneMDSPP::IDD)
{
}

DoneMDSPP::~DoneMDSPP()
{
}

void DoneMDSPP::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DoneMDSPP, WizardBasePage)
END_MESSAGE_MAP()


// DoneMDSPP message handlers

BOOL DoneMDSPP::OnSetActive()
{
	GET_PSHEET()->SetWizardButtons(PSWIZB_FINISH);
	return WizardBasePage::OnSetActive();
}
