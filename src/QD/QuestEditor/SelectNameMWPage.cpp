// SelectNameMWPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "SelectNameMWPage.h"
#include "AddInterfBackgrnd.h"
#include "AddBackData.h"
#include "AddInterfBackgrnd.h"
#include "name_op.h"
#include "name_interf.h"
#include "qd_interface_screen.h"
#include "qd_interface_element.h"


// SelectNameMWPage dialog

IMPLEMENT_DYNAMIC(SelectNameMWPage, WizardBasePage)
SelectNameMWPage::SelectNameMWPage()
	: WizardBasePage(SelectNameMWPage::IDD)
	, m_strName(_T(""))
	, m_pStringSetter(NULL)
	, m_pNameTester(NULL)
{
}

SelectNameMWPage::~SelectNameMWPage()
{
}

void SelectNameMWPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(SelectNameMWPage, WizardBasePage)
	ON_EN_CHANGE(IDC_EDIT_NAME, OnEnChangeEditName)
END_MESSAGE_MAP()


void SelectNameMWPage::SetNameSetter(INameSetter* psetter)
{
	m_pStringSetter = psetter;
}
void SelectNameMWPage::SetNameTester(INameTester* pTester)
{
	m_pNameTester = pTester;
}

void SelectNameMWPage::SetCaption(const CString& cap)
{
	m_strCaption = cap;
}

// SelectNameMWPage message handlers

BOOL SelectNameMWPage::OnSetActive()
{
	ASSERT(m_pStringSetter);
	m_strName = m_pStringSetter->get_string();

	UpdateData(FALSE);
	OnEnChangeEditName();
	if (!m_strCaption.IsEmpty())
	{
		SetWindowText(_T(""));
		GET_PSHEET()->SetWindowText(m_strCaption);
	}
	return WizardBasePage::OnSetActive();
}

BOOL SelectNameMWPage::OnKillActive()
{
	ASSERT(m_pStringSetter);
	m_pStringSetter->set_string(m_strName);
	return WizardBasePage::OnKillActive();
}

void SelectNameMWPage::OnEnChangeEditName()
{
	UpdateData();

	CString msg;
	if (m_strName.IsEmpty())
	{
		if (m_ptrPrevPage)
			GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
		else
			GET_PSHEET()->SetWizardButtons(0);
		msg.LoadString(IDS_STRING77);
	}
	else
	{
		if (m_ptrPrevPage)
			GET_PSHEET()->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
		else
			GET_PSHEET()->SetWizardButtons(PSWIZB_NEXT);

		if (m_pNameTester)
		{
			CString uniq_name = m_strName;
			if (m_pNameTester)
				uniq_name = m_pNameTester->get_unique_name(m_strName);
			UINT uId = IDS_STRING75;
			if (uniq_name != m_strName)
				uId = IDS_STRING76;

			msg.Format(uId, (LPCTSTR)uniq_name);
		}
		else
			msg.Format(IDS_STRING75, (LPCTSTR)m_strName);

	}
	GetDlgItem(IDC_STATIC_MSG)->SetWindowText(msg);
}

