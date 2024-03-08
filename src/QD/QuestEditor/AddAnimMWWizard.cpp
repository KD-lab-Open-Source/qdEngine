#include "stdafx.h"
#include "questeditor.h"

#include "addanimmwwizard.h"
#include "addanimwizdata.h"

#include "MWAddAnimFirst.h"
#include "MWSelectExstFilePage.h"
#include "MWConvertWayPage.h"
#include "MWAAProgressPage.h"
#include "MWAADone.h"
#include "MWAACreateObjPage.h"
#include "MWAACadrDuration.h"


/// Создает визрад в дин. памяти и возвращает на него указатель
AddAnimMWWizardPtr AddAnimationWizard(qdGameDispatcher* ptrDisp, 
										 qdGameDispatcherBase* ptrGDB)
{
	AddAnimMWWizardPtr wizard(new AddAnimMWWizard);
	if (!wizard.get()) 
		return wizard;
	wizard->set_caption(CString((LPTSTR)IDS_WIZARD_CAP_ANIMATION_ADD));
	wizard->Construct(ptrDisp, ptrGDB);
	return wizard;
}

AddAnimMWWizard::AddAnimMWWizard(void)
{
}

AddAnimMWWizard::~AddAnimMWWizard(void)
{
}

void AddAnimMWWizard::reset_data(){
	m_ptrData->files_.clear();
}
/*
	Если идет процесс сборки, то прервать визард нельзя
*/
bool AddAnimMWWizard::SuspendWizard()
{
	if (cur_page() == m_ptrAAProgress.get())
		return false;
	return true;
}


bool AddAnimMWWizard::ResumeWizard()
{
	if (!cur_page()) 
	{
		if (pages_list().empty()) 
			return false;

		set_cur_page(pages_list().front());
	}
	if (cur_page() == m_ptrAADone.get()) 
	{
		set_cur_page(pages_list().front());
		reset_data();
	}
	return true;
}

/**
	Создает данные
	Создает странички и увязывает их в нужном порядке.
	Затем добавляет страчики  в визард
	@param ptrDisp	- диспертчер проекта
	@param ptrGDB	- диспетчер, в который будем добавлять анимацию. 
						Может совпадать с ptrDisp
 */
bool AddAnimMWWizard::Construct(qdGameDispatcher* ptrDisp, qdGameDispatcherBase* ptrGDB)
{
	m_ptrData.reset(new add_anim_wiz_data);
	if (!m_ptrData.get())
		return false;

	m_ptrAAFirst.reset(new MWAddAnimFirst);
	if (!m_ptrAAFirst.get())
		return false;

	m_ptrAAFirst->set_wizard(this);

	m_ptrAAConvertWay.reset(new MWConvertWayPage);
	if (!m_ptrAAConvertWay.get()) 
		return false;

	m_ptrAAConvertWay->set_wizard(this);
	m_ptrAAConvertWay->set_prev_page(m_ptrAAFirst.get());

	m_ptrAACrtObj.reset(new MWAACreateObjPage);
	if (!m_ptrAACrtObj.get())
		return false;

	m_ptrAACrtObj->set_wizard(this);
	m_ptrAACrtObj->set_prev_page(m_ptrAAConvertWay.get());

	m_ptrAASelExistFiles.reset(new MWSelectExstFilePage);
	if (!m_ptrAASelExistFiles.get()) 
		return false;

	m_ptrAASelExistFiles->set_wizard(this);
	m_ptrAASelExistFiles->set_prev_page(m_ptrAACrtObj.get());

	m_ptrAAProgress.reset(new MWAAProgressPage);
	if (!m_ptrAAProgress.get())
		return false;

	m_ptrAACadrDur.reset(new MWAACadrDuration);
	if (!m_ptrAACadrDur.get())
		return false;
	m_ptrAACadrDur->set_wizard(this);
	m_ptrAACadrDur->set_prev_page(m_ptrAASelExistFiles.get());

	m_ptrAAProgress->set_wizard(this);
	m_ptrAAProgress->set_prev_page(m_ptrAACadrDur.get());
	m_ptrAAProgress->set_disps(ptrDisp, ptrGDB);

	m_ptrAADone.reset(new MWAADone);
	if (!m_ptrAADone.get())
		return false;

	m_ptrAADone->set_wizard(this);
	m_ptrAADone->set_prev_page(m_ptrAASelExistFiles.get());

	m_ptrAAFirst->set_page_for_sel_exist(m_ptrAACrtObj.get());
	m_ptrAAFirst->set_page_for_convert(m_ptrAAConvertWay.get());

	m_ptrAAConvertWay->set_next_page(m_ptrAACrtObj.get());

	add_page(m_ptrAAFirst.get());
	add_page(m_ptrAAConvertWay.get());
	add_page(m_ptrAACrtObj.get());
	add_page(m_ptrAASelExistFiles.get());
	add_page(m_ptrAACadrDur.get());
	add_page(m_ptrAAProgress.get());
	add_page(m_ptrAADone.get());

	m_ptrData->m_bUseGlobalCatalog = (ptrDisp == ptrGDB);
	return true;
}
