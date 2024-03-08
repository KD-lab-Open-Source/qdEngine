#include "stdafx.h"
#include "resource.h"
#include "dirmakewizard.h"

#include "SelectFolderMDSPP.h"
#include "SceneCountMDSPP.h"
#include "dirmakewizarddata.h"
#include "ProgressMDSPP.h"
#include "DoneMDSPP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DirMakeWizard::DirMakeWizard(void)
{
}

DirMakeWizard::~DirMakeWizard(void)
{
}

bool DirMakeWizard::Construct()
{
	m_ptrData.reset(new DirMakeWizardData);
	if (!m_ptrData.get())
		return false;

	m_ptrSelFolderPage.reset(new SelectFolderMDSPP);
	if (!m_ptrSelFolderPage.get())
		return false;
	m_ptrSelFolderPage->set_wizard(this);

	m_ptrSceneCountPage.reset(new SceneCountMDSPP);
	if (!m_ptrSceneCountPage.get())
		return false;
	m_ptrSceneCountPage->set_wizard(this);

	m_ptrProgresPage.reset(new ProgressMDSPP);
	if (!m_ptrProgresPage.get())
		return false;
	m_ptrProgresPage->set_wizard(this);

	m_ptrDonePage.reset(new DoneMDSPP);
	if (!m_ptrDonePage.get())
		return false;
	m_ptrDonePage->set_wizard(this);

	add_page(m_ptrSelFolderPage.get());
	add_page(m_ptrSceneCountPage.get());
	add_page(m_ptrProgresPage.get());
	add_page(m_ptrDonePage.get());
	return true;
}

bool DirMakeWizard::SuspendWizard()
{
	return true;
}
//запускает по новой. возвращаемое значение не используется(пока)
bool DirMakeWizard::ResumeWizard()
{
	return true;
}
