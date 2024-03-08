#include "stdafx.h"
#include "resource.h"
#include "addintrfbackgrndwizardshow.h"
#include "AddInterfBackgrnd.h"
#include "multiwizard/MultiWizard.h"


AddIntrfBackgrndWizardShow::AddIntrfBackgrndWizardShow(void)
{
}

AddIntrfBackgrndWizardShow::~AddIntrfBackgrndWizardShow(void)
{
}

bool AddIntrfBackgrndWizardShow::show(CWnd* parent, qdInterfaceScreen* pscr, UINT idCap)
{
	AddInterfBackgrnd wizard;
	if(!wizard.Construct(pscr))
		return false;
	const CString cap((LPTSTR)IDS_WIZARD_CAP_ADD_INTERF_BACK);
	wizard.set_caption(cap);

	MultiWizard MultiWizard(IDS_WIZARD_CAP_ADD_INTERF_BACK, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}