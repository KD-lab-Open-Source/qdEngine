#include "stdafx.h"
#include "resource.h"
#include "multiwizard/MultiWizard.h"
#include "dirmakewizard.h"
#include "dirmakewizardshower.h"

DirMakeWizardShower::DirMakeWizardShower(void)
{
}

DirMakeWizardShower::~DirMakeWizardShower(void)
{
}

bool DirMakeWizardShower::show(CWnd * parent)
{
	DirMakeWizard wizard;
	if(!wizard.Construct())
		return false;
	wizard.set_caption(CString((LPTSTR)IDS_MAKE_DIR_STRUCT_WIZARD_STRUCT));

	MultiWizard MultiWizard(IDS_MAKE_DIR_STRUCT_WIZARD_STRUCT, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}
