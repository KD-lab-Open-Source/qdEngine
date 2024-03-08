#pragma once


// MWAACreateObjPage dialog
#include "wizardbasepage.h"

/**
	На этой сраничке узнаем у пользователя надо ли создавать объекты
 */
class MWAACreateObjPage : public WizardBasePage
{
	DECLARE_DYNAMIC(MWAACreateObjPage)

public:
	MWAACreateObjPage();
	virtual ~MWAACreateObjPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_CREATE_OBJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();

	afx_msg void OnBnClickedRadioNotCreate();
	afx_msg void OnBnClickedRadioCreate();
private:
	/// Хранит введенное значение
	BOOL m_bCreate;
};
