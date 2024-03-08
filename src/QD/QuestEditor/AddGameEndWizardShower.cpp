#include "stdafx.h"
#include "resource.h"
#include "addgameendwizardshower.h"
#include "AddGameEndWizard.h"
#include "MultiWizard/MultiWizard.h"

AddGameEndWizardShower::AddGameEndWizardShower(void)
{
}

AddGameEndWizardShower::~AddGameEndWizardShower(void)
{
}

bool AddGameEndWizardShower::show(CWnd *parent, class qdGameDispatcher* pdisp, 
								  ITreeNode* parentNode)
{
	AddGameEndWizard wizard;
	if(!wizard.Construct(pdisp, parentNode))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_GAME_END);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_GAME_END, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}

bool AddGameEndWizardShower::show(CWnd *parent, class qdGameDispatcher* pdisp, 
								  HTREEITEM hParent)
{
	AddGameEndWizard wizard;
	if(!wizard.Construct(pdisp, hParent))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_GAME_END);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_GAME_END, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}