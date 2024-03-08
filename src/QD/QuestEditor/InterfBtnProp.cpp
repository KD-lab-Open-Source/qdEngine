// InterfBtnProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfBtnProp.h"
#include "qd_interface_button.h"

// InterfBtnProp dialog

IMPLEMENT_DYNAMIC(InterfBtnProp, InterfElePosPage)
InterfBtnProp::InterfBtnProp()
	: InterfElePosPage(InterfBtnProp::IDD)
	, m_pSourceBtn(NULL)
{
}

InterfBtnProp::~InterfBtnProp()
{
}

void InterfBtnProp::DoDataExchange(CDataExchange* pDX)
{
	InterfElePosPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPTIONS, m_wndOptions);
}


BEGIN_MESSAGE_MAP(InterfBtnProp, InterfElePosPage)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTIONS, OnCbnSelchangeComboOptions)
END_MESSAGE_MAP()

void InterfBtnProp::set_data(qdInterfaceDispatcher* pd, qdInterfaceButton* pbtn)
{
	m_pSourceBtn = pbtn;
	m_pBackBtn.reset(new qdInterfaceButton(*m_pSourceBtn));
	InterfElePosPage::set_data(pd, m_pSourceBtn);
}

// InterfBtnProp message handlers

BOOL InterfBtnProp::OnInitDialog()
{
	InterfElePosPage::OnInitDialog();

	LoadOptions();
	
	btn2wnd();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL InterfBtnProp::OnApply()
{
	BOOL res = InterfElePosPage::OnApply();
	wnd2btn();
	*m_pBackBtn.get() = *m_pSourceBtn;
	return res;
}

void InterfBtnProp::OnCancel()
{
	InterfElePosPage::OnCancel();
	*m_pSourceBtn = *m_pBackBtn.get();
}

bool InterfBtnProp::LoadOptions()
{
	size_t const count = m_gameOptions.size();
	for(size_t i = 0; i < count; ++i)
	{
		IGameOptionRepres* const pi = m_gameOptions[i];
		int const iItem = m_wndOptions.AddString(pi->description());
		if (iItem != CB_ERR)
			m_wndOptions.SetItemDataPtr(iItem, pi);
	}
	m_wndOptions.SetCurSel(0);
	return true;
}

void InterfBtnProp::OnCbnSelchangeComboOptions()
{
	SetModified();
}

void InterfBtnProp::btn2wnd()
{
	m_wndOptions.SelectString(-1, 
		m_gameOptions[m_pSourceBtn->option_ID()]->description());
}

void InterfBtnProp::wnd2btn()
{
	m_pSourceBtn->set_option_ID(getOptionId());
}

qdInterfaceElement::option_ID_t InterfBtnProp::getOptionId() const{
	int const iCurSel = m_wndOptions.GetCurSel();
	ASSERT(iCurSel != CB_ERR);
	return static_cast<IGameOptionRepres*>
		(m_wndOptions.GetItemDataPtr(iCurSel))->optionId();
}