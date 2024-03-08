#include "stdafx.h"
#include "questeditor.h"
#include "addanimmultiwizard.h"
#include "AddAnimMWWizard.h"
#include "multiwizard/MultiWizard.h"

AddAnimMultiWizard::AddAnimMultiWizard(void)
{
}

AddAnimMultiWizard::~AddAnimMultiWizard(void)
{
}

bool AddAnimMultiWizard::show(CWnd * parent, class qdGameDispatcher* ptrDisp, 
		  qdGameDispatcherBase* ptrGDB, UINT idCap)
{
	AddAnimMWWizard wizard;
	if(!wizard.Construct(ptrDisp, ptrGDB))
		return false;
	wizard.set_caption(CString((LPTSTR)idCap));

	MultiWizard MultiWizard(idCap, parent);
	if(!MultiWizard.AddWizard(&wizard))
		return false;

	MultiWizard.DoModal();

	return wizard.was_change();
}
