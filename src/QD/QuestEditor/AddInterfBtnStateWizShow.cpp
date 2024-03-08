#include "stdafx.h"
#include "resource.h"

#include "addinterfbtnstatewizshow.h"
#include "AddInterfBtnStateWizData.h"
#include "AddInterfBtnStateWiz.h"
#include "MultiWizard/MultiWizard.h"

AddInterfBtnStateWizShow::AddInterfBtnStateWizShow(void)
{
}

AddInterfBtnStateWizShow::~AddInterfBtnStateWizShow(void)
{
}

bool AddInterfBtnStateWizShow::show(CWnd *parent, class qdInterfaceButton* pbtn)
{
	AddInterfBtnStateWiz wizard;
	if(!wizard.Construct(pbtn))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_INTERF_BUTTON_STATE);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_INTERF_BUTTON_STATE, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}
