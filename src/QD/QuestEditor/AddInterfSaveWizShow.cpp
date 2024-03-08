#include "stdafx.h"
#include "resource.h"
#include "addinterfsavewizshow.h"

#include "addinterfsavewiz.h"
#include "MultiWizard/MultiWizard.h"

AddInterfSaveWizShow::AddInterfSaveWizShow()
{
}

AddInterfSaveWizShow::~AddInterfSaveWizShow()
{
}

bool AddInterfSaveWizShow::show(CWnd *parent, class qdInterfaceScreen* pscr)
{
	AddInterfSaveWiz wizard;
	if(!wizard.Construct(pscr))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_INTERF_SAVE);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_INTERF_SAVE, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}