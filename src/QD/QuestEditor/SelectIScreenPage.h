#pragma once
#include "afxwin.h"
#include "WizardBasePage.h"

// SelectIScreenPage dialog

/**
	��������� ������ ������������ ������ ��� "���������� ������"
 */
class SelectIScreenPage : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectIScreenPage)

public:
	SelectIScreenPage();
	virtual ~SelectIScreenPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SELECT_INTERF_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnCbnSelchangeCombo1();

	DECLARE_MESSAGE_MAP()
	/// ��������� ������ �������
	void LoadInterfScreens();
private:
	/// �������� ������ �������
	CCJFlatComboBox m_wndScreens;
};
