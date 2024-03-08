#pragma once


// MWAADone dialog

#include "wizardbasepage.h"
/**
	Последняя страница в визарде
 */
class MWAADone : public WizardBasePage
{
	DECLARE_DYNAMIC(MWAADone)

public:
	MWAADone();
	virtual ~MWAADone();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_AA_DONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	/// Зачищает список файлов, чтобы можно было запустить визард еще раз
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
};
