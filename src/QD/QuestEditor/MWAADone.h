#pragma once


// MWAADone dialog

#include "wizardbasepage.h"
/**
	��������� �������� � �������
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

	/// �������� ������ ������, ����� ����� ���� ��������� ������ ��� ���
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
};
