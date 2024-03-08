#pragma once
#include "afxwin.h"
#include "WizardBasePage.h"

// SelectIScreenPage dialog

/**
	Страничка выбора интерфесного экрана для "последнего экрана"
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
	/// Загружает список экранов
	void LoadInterfScreens();
private:
	/// Содержит список экранов
	CCJFlatComboBox m_wndScreens;
};
