#pragma once

#include "wizardbasepage.h"
// DoneMDSPP dialog
/**
	��������� ��������� � ������� �������� ��������� ���������
 */
class DoneMDSPP : public WizardBasePage
{
	DECLARE_DYNAMIC(DoneMDSPP)

public:
	DoneMDSPP();
	virtual ~DoneMDSPP();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MDS_DONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnSetActive();
};
