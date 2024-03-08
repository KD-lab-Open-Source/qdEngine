// SelectIScreenPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QuestEditor.h"
#include "SelectIScreenPage.h"
#include "qd_interface_screen.h"
#include "AddGameEndWizard.h"
#include "AddGameEndWizData.h"

#include "iscreen2combo.h"
#include <boost/function_output_iterator.hpp>

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

// SelectIScreenPage dialog

IMPLEMENT_DYNAMIC(SelectIScreenPage, WizardBasePage)
SelectIScreenPage::SelectIScreenPage()
	: WizardBasePage(SelectIScreenPage::IDD)
{
}

SelectIScreenPage::~SelectIScreenPage()
{
}

void SelectIScreenPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_wndScreens);
}


BEGIN_MESSAGE_MAP(SelectIScreenPage, WizardBasePage)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// SelectIScreenPage message handlers
BOOL SelectIScreenPage::OnInitDialog()
{
	BOOL bRes = __super::OnInitDialog();
	LoadInterfScreens();
	return bRes;
}

void SelectIScreenPage::LoadInterfScreens()
{
	qdInterfaceDispatcher* const pdisp = qdInterfaceDispatcher::get_dispatcher();

	const qdInterfaceDispatcher::screen_list_t::const_iterator 
		itr = pdisp->screen_list().begin(),  
		end = pdisp->screen_list().end();
	typedef boost::function_output_iterator<iscreens2combo> out_itr_t;

	if (pdisp->screen_list().empty())
		m_wndScreens.EnableWindow(FALSE);
	else
	{
		std::copy(itr, end, out_itr_t(iscreens2combo(m_wndScreens)));
		AdjustListWidth(&m_wndScreens);
		m_wndScreens.SetCurSel(0);
		OnCbnSelchangeCombo1();
	}
}

BOOL SelectIScreenPage::OnSetActive()
{
	if (m_wndScreens.GetCurSel()== CB_ERR)
		GET_PSHEET()->SetWizardButtons(0);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_NEXT);

	return WizardBasePage::OnSetActive();
}

/// Записывает имя экрана в данные визарда. 
void SelectIScreenPage::OnCbnSelchangeCombo1()
{
	int const idx = m_wndScreens.GetCurSel();
	if (idx == CB_ERR)
	{
		GET_PSHEET()->SetWizardButtons(0);
		return;
	}
	GET_PSHEET()->SetWizardButtons(PSWIZB_NEXT);

	CString res;
	m_wndScreens.GetLBText(idx, res);
	AddGameEndWizard* pwiz = static_cast<AddGameEndWizard*>(wizard());
	AddGameEndWizData* pdata = pwiz->data();
	pdata->setScrName(res);
	pdata->set_name(res);
}
