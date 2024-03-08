// SceneCountMDSPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "SceneCountMDSPP.h"
#include "dirmakewizard.h"
#include "dirmakewizarddata.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// SceneCountMDSPP dialog

IMPLEMENT_DYNAMIC(SceneCountMDSPP, WizardBasePage)
SceneCountMDSPP::SceneCountMDSPP()
	: WizardBasePage(SceneCountMDSPP::IDD)
	, m_iCount(0)
{
}

SceneCountMDSPP::~SceneCountMDSPP()
{
}

void SceneCountMDSPP::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_iCount);
}


BEGIN_MESSAGE_MAP(SceneCountMDSPP, WizardBasePage)
	ON_EN_CHANGE(IDC_EDIT, OnEnChangeEdit)
END_MESSAGE_MAP()


// SceneCountMDSPP message handlers

BOOL SceneCountMDSPP::OnKillActive()
{
	DirMakeWizard* pw = static_cast<DirMakeWizard*>(wizard());
	DirMakeWizardData* pd = pw->data();
	pd->scenes_count = m_iCount;

	return WizardBasePage::OnKillActive();
}

BOOL SceneCountMDSPP::OnSetActive()
{
	DirMakeWizard* pw = static_cast<DirMakeWizard*>(wizard());
	DirMakeWizardData* pd = pw->data();
	m_iCount = pd->scenes_count;
	UpdateData(FALSE);
	check_next_btn_access();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN))->SetRange(0, 999);
	return WizardBasePage::OnSetActive();
}

void SceneCountMDSPP::check_next_btn_access()
{
	if (m_iCount)
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
}

void SceneCountMDSPP::OnEnChangeEdit()
{
	UpdateData();
	check_next_btn_access();
}
