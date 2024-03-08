#include "stdafx.h"
#include "resource.h"
#include "addinterfsldrwizardshow.h"
#include "multiwizard/MultiWizard.h"
#include "addinterfsldr.h"


AddInterfSldrWizardShow::AddInterfSldrWizardShow(void)
{
}

AddInterfSldrWizardShow::~AddInterfSldrWizardShow(void)
{
}

bool AddInterfSldrWizardShow::show(CWnd* parent, qdInterfaceScreen* pscr)
{
	AddInterfSlider wizard;
	if(!wizard.Construct(pscr))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_INTERF_SLIDER);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_INTERF_SLIDER, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}