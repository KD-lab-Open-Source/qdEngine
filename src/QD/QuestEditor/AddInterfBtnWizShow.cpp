#include "stdafx.h"
#include "resource.h"

#include "addinterfbtnwizshow.h"
#include "addinterfbtnwiz.h"
#include "MultiWizard/MultiWizard.h"

AddInterfBtnWizShow::AddInterfBtnWizShow(void)
{
}

AddInterfBtnWizShow::~AddInterfBtnWizShow(void)
{
}

bool AddInterfBtnWizShow::show(CWnd *parent, class qdInterfaceScreen* pscr)
{
	AddInterfBtnWiz wizard;
	if(!wizard.Construct(pscr))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_INTERF_BUTTON);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_INTERF_BUTTON, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}
