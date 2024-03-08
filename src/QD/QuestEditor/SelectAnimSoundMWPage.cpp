// SelectAnimSoundMWPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "SelectAnimSoundMWPage.h"

const CString GetIntrfSoundFileName();
const CString GetIntrfAnimFileName();

// SelectAnimSoundMWPage dialog

IMPLEMENT_DYNAMIC(SelectAnimSoundMWPage, WizardBasePage)
SelectAnimSoundMWPage::SelectAnimSoundMWPage()
	: WizardBasePage(SelectAnimSoundMWPage::IDD)
	, m_pSoundSetter(NULL)
	, m_pAnimSetter(NULL)
	, m_strAnimFile(_T(""))
	, m_strSoundFile(_T(""))
{
}

SelectAnimSoundMWPage::~SelectAnimSoundMWPage()
{
}

void SelectAnimSoundMWPage::DoDataExchange(CDataExchange* pDX)
{
	WizardBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ANIM, m_strAnimFile);
	DDX_Text(pDX, IDC_EDIT_SOUND, m_strSoundFile);
}

void SelectAnimSoundMWPage::SetCaption(const CString& str)
{
	m_strCaption = str;
}

void SelectAnimSoundMWPage::set_anim_setter(text_set* pas)
{
	m_pAnimSetter = pas;
}

void SelectAnimSoundMWPage::set_sound_setter(text_set* pss)
{
	m_pSoundSetter = pss;
}

BEGIN_MESSAGE_MAP(SelectAnimSoundMWPage, WizardBasePage)
	ON_BN_CLICKED(IDC_BTN_SEL_ANIM, OnBnClickedBtnSelAnim)
	ON_BN_CLICKED(IDC_BTN_SEL_SOUND, OnBnClickedBtnSelSound)
	ON_EN_CHANGE(IDC_EDIT_SOUND, OnEnChangeEditSound)
	ON_EN_CHANGE(IDC_EDIT_ANIM, OnEnChangeEditAnim)
END_MESSAGE_MAP()


// SelectAnimSoundMWPage message handlers

BOOL SelectAnimSoundMWPage::OnSetActive()
{
	m_strAnimFile = m_pAnimSetter->get_text();
	if (m_pSoundSetter)
		m_strSoundFile = m_pSoundSetter->get_text();
	else
	{
		GetDlgItem(IDC_BTN_SEL_SOUND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SOUND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SOUND)->ShowWindow(SW_HIDE);
	}

	/// Назначаем имя файла, чтобы проверить подходит ли оно
	if (m_pAnimSetter->set_text(m_strAnimFile))
		GET_PSHEET()->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);

	GET_PSHEET()->SetWindowText(m_strCaption);

	return WizardBasePage::OnSetActive();
}

LRESULT SelectAnimSoundMWPage::OnWizardNext()
{
	return WizardBasePage::OnWizardNext();
}

void SelectAnimSoundMWPage::OnBnClickedBtnSelAnim()
{
	CString const res = GetIntrfAnimFileName();
	if (!res.IsEmpty())
		m_strAnimFile = res;
	if (m_pAnimSetter->set_text(m_strAnimFile))
		GET_PSHEET()->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
	UpdateData(FALSE);
}

void SelectAnimSoundMWPage::OnBnClickedBtnSelSound()
{
	CString const res = GetIntrfSoundFileName();
	if (!res.IsEmpty())
		m_strSoundFile = res;
	UpdateData(FALSE);
}

void SelectAnimSoundMWPage::OnEnChangeEditSound()
{
	UpdateData();
	m_strSoundFile.Trim();
	m_pSoundSetter->set_text(m_strSoundFile);
}

void SelectAnimSoundMWPage::OnEnChangeEditAnim()
{
	UpdateData();
	m_strAnimFile.Trim();

	if (m_pAnimSetter->set_text(m_strAnimFile))
		GET_PSHEET()->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);
	else
		GET_PSHEET()->SetWizardButtons(PSWIZB_BACK);
}
