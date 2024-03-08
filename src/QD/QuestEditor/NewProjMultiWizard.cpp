// NewProjMultiWizard.cpp: implementation of the NewProjMultiWizard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "NewProjMultiWizard.h"


#include "multiwizard/MultiWizard.h"
#include "multiwizard/MWWizard.h"
#include "MWGlobalSettingPage.h"
#include "AddAnimMWWizard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AddAnimMWWizardPtr AddAnimationWizard(qdGameDispatcher* ptrDisp, 
										 qdGameDispatcherBase* ptrGDB);

NewProjMultiWizard::NewProjMultiWizard()
{
	
}

NewProjMultiWizard::~NewProjMultiWizard()
{

}

/**
	Создает визарды. Добавляет их в мульти-визард. И показывает это зрителям
	\return true - если были сделаны изменения
 */
bool NewProjMultiWizard::show(CWnd * parent, qdGameDispatcher* ptrDisp, UINT idCap)
{
	m_ptrMultiWizard.reset(new MultiWizard(idCap, parent));
	if (!m_ptrMultiWizard.get())
		return false;

	m_ptrGDisp = ptrDisp;
	m_ptrMultiWizard->SetWizardMode();

	MWWizardPtr ptrGlobalSettWiz(AddGameGlobalSettingWizard());
	if (!ptrGlobalSettWiz.get()) 
		return false;
	m_ptrMultiWizard->AddWizard(ptrGlobalSettWiz.get());

	MWWizardPtr ptrAddAnimWiz(AddAnimationWizard());
	m_ptrMultiWizard->AddWizard(ptrAddAnimWiz.get());

	m_ptrMultiWizard->DoModal();

	return (ptrGlobalSettWiz->was_change()|| ptrAddAnimWiz->was_change());
}

MWWizardPtr NewProjMultiWizard::AddGameGlobalSettingWizard(){

	MWWizardPtr wizard(new MWWizard);
	if (!wizard.get()) 
		return MWWizardPtr();

	wizard->set_caption(CString((LPTSTR)IDS_WIZARD_CAP_GAME_GLOBAL_SETTINGS));

	m_ptrGlobalGameSetting.reset(new MWGlobalSettingPage);
	if (!m_ptrGlobalGameSetting.get())
		return MWWizardPtr();

	m_ptrGlobalGameSetting->set_wizard(wizard.get());
	static_cast<MWGlobalSettingPage*>(m_ptrGlobalGameSetting.get())
													->set_dispatcher(m_ptrGDisp);
	wizard->add_page(m_ptrGlobalGameSetting.get());
	
	return wizard;
}

MWWizardPtr NewProjMultiWizard::AddAnimationWizard(){
	return MWWizardPtr(::AddAnimationWizard(m_ptrGDisp, m_ptrGDisp).release());
}
