// WizardBasePage.h: interface for the WizardBasePage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIZARDBASEPAGE_H__8EAE79F0_C46A_4014_9E14_CA4ADE60F005__INCLUDED_)
#define AFX_WIZARDBASEPAGE_H__8EAE79F0_C46A_4014_9E14_CA4ADE60F005__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Multiwizard\MWBasePage.h"

#include "mwdatabase.h"
#define GET_PSHEET() static_cast<CPropertySheet*>(GetParent())

/**
	Еще одно обобщение базовой страницы для мультивизарда.
	Нужна для тех визардов, которые имеют несколько путей подхода
	к одной странице. Хранит страницу, на которую надо вернуться
 */
class WizardBasePage : public MWBasePage  
{
	DECLARE_DYNCREATE(WizardBasePage);

	/// Предыдущая страница
	WizardBasePage* m_ptrPrevPage;
public:
	WizardBasePage();
	WizardBasePage(UINT dlgIDD);
	virtual ~WizardBasePage();
	
	VOID set_prev_page(WizardBasePage* ptr);
};

#endif //!defined(AFX_WIZARDBASEPAGE_H__8EAE79F0_C46A_4014_9E14_CA4ADE60F005__INCLUDED_)
