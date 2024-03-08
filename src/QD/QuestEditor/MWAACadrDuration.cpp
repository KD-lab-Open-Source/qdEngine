// MWAACadrDuration.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MWAACadrDuration.h"

#include "addanimwizdata.h"
#include "AddAnimMWWizard.h"


// MWAACadrDuration dialog

IMPLEMENT_DYNAMIC(MWAACadrDuration, WizardBasePage)
MWAACadrDuration::MWAACadrDuration()
	: WizardBasePage(MWAACadrDuration::IDD)
	, m_fDuration(0.f)
{
}

MWAACadrDuration::~MWAACadrDuration()
{
}

void MWAACadrDuration::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PAUSE, m_fDuration);
}


BEGIN_MESSAGE_MAP(MWAACadrDuration, WizardBasePage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DURATION, OnDeltaposSpinDuration)
	ON_EN_CHANGE(IDC_EDIT_PAUSE, OnEnChangeEditPause)
END_MESSAGE_MAP()


// MWAACadrDuration message handlers

BOOL MWAACadrDuration::OnKillActive()
{
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());	
	add_anim_wiz_data* data = pw->data();
	data->cadr_duration_ = m_fDuration;
	return WizardBasePage::OnKillActive();
}

BOOL MWAACadrDuration::OnSetActive()
{
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_DURATION))->SetRange(0, 10000);
	AddAnimMWWizard* pw = static_cast<AddAnimMWWizard*>(wizard());	
	add_anim_wiz_data* data = pw->data();
	if (data->cadr_duration_ == INVALID_CADR_DUR){
		ReadDurFromIni();
	} 
	if (m_fDuration == INVALID_CADR_DUR) 
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	
	UpdateData(FALSE);
	return WizardBasePage::OnSetActive();
}

LRESULT MWAACadrDuration::OnWizardNext()
{
	WriteDurToIni();
	return WizardBasePage::OnWizardNext();
}

void MWAACadrDuration::ReadDurFromIni()
{
	CQuestEditorApp* papp = GetMyApp();
//	CString dur_val = GetMyApp()->GetStrSetting(IDS_SETTING_NAME_ANIM_CADR_DUR);
//	m_fDuration = static_cast<float>(_ttof(dur_val));
	IProjectSettings* pps = papp->GetProjectSettings();
//	CString strIniFileName(papp->GetProjIniFileName());
	CString strIniKeyName(reinterpret_cast<LPTSTR>(IDS_INI_KEY_NAME_CADR_DUR));
	CString strSecName(reinterpret_cast<LPTSTR>(IDS_INI_SEC_NAME_ANIM_ASSEMBLING));
//	if (!GetPrivateProfileStruct(strIniKeyName, strSecName, &m_fDuration, 
//												sizeof(m_fDuration), strIniFileName))
//	{
//		m_fDuration = 0.1f;
//	}
	m_fDuration = pps->GetFloatSetting(strSecName, strIniKeyName, 0.1f);
}

void MWAACadrDuration::WriteDurToIni()
{
	CQuestEditorApp* papp = GetMyApp();
//	CString strIniFileName(papp->GetProjIniFileName());
	CString strIniKeyName(reinterpret_cast<LPTSTR>(IDS_INI_KEY_NAME_CADR_DUR));
	CString strSecName(reinterpret_cast<LPTSTR>(IDS_INI_SEC_NAME_ANIM_ASSEMBLING));
//	WritePrivateProfileStruct(strIniKeyName, strSecName, &m_fDuration, 
//												sizeof(m_fDuration), strIniFileName);
//	papp->WriteStrSetting(IDS_SETTING_NAME_ANIM_CADR_DUR)
	IProjectSettings* pps = papp->GetProjectSettings();
	pps->WriteFloatSetting(strSecName, strIniKeyName, m_fDuration);
}

void MWAACadrDuration::OnDeltaposSpinDuration(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}
	m_fDuration += pNMUpDown->iDelta/100.f;
	if (m_fDuration < 0.f)
		m_fDuration = 0.f;

	if (m_fDuration == INVALID_CADR_DUR) 
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	UpdateData(FALSE);
	*pResult = 0;
}

void MWAACadrDuration::OnEnChangeEditPause()
{
	if (!UpdateData())
		return;

	if (m_fDuration == INVALID_CADR_DUR) 
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}
