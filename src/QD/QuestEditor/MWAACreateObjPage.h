#pragma once


// MWAACreateObjPage dialog
#include "wizardbasepage.h"

/**
	�� ���� �������� ������ � ������������ ���� �� ��������� �������
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
	/// ������ ��������� ��������
	BOOL m_bCreate;
};
