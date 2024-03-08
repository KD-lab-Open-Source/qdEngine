// InterfSldrStateProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfSldrStateProp.h"
#include "qd_interface_slider.h"

#include "interf_disk_op.h"
#include "file_tests.h"
#include "custom_ddx.h"
#include "ConvertTGAtoQDA.h"

// InterfSldrStateProp dialog

IMPLEMENT_DYNAMIC(InterfSldrStateProp, CBasePage)
InterfSldrStateProp::InterfSldrStateProp(eStateChoose stc, UINT idCap)
	: CBasePage(InterfSldrStateProp::IDD, idCap)
	, m_strAnimFile(_T(""))
	, m_strSoundFile(_T(""))
	, m_eStateChoose(stc)
	, m_ptrSlider(NULL)
{
}

InterfSldrStateProp::~InterfSldrStateProp()
{
}

void InterfSldrStateProp::set_data(class qdInterfaceSlider* pslider)
{
	m_ptrSlider = pslider;
	if (m_eStateChoose == SC_BACK)
		m_ptrBack.reset(new qdInterfaceElementState(*pslider->background()));
	else
		m_ptrBack.reset(new qdInterfaceElementState(*pslider->slider()));

	m_ptrForChange.reset(new qdInterfaceElementState(*m_ptrBack));
}

void InterfSldrStateProp::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	if (m_eStateChoose == SC_SLIDER)
		custom_ddx::DDX_Text(pDX, IDC_EDIT_ANIM_FILE, m_strAnimFile,
			file_tests::strict_file_test());
	else
		custom_ddx::DDX_Text(pDX, IDC_EDIT_ANIM_FILE, m_strAnimFile,
			file_tests::non_strict_file_test());
	custom_ddx::DDX_Text(pDX, IDC_EDIT_SOUND_FILE, m_strSoundFile,
		file_tests::non_strict_file_test());

	DDX_Text(pDX, IDC_EDIT_OFFS_X, backgroundOffset_.x);
	DDX_Text(pDX, IDC_EDIT_OFFS_Y, backgroundOffset_.y);
}


BEGIN_MESSAGE_MAP(InterfSldrStateProp, CBasePage)
	ON_BN_CLICKED(IDC_BTN_SEL_ANIM, OnBnClickedBtnSelAnim)
	ON_BN_CLICKED(IDC_BTN_SEL_SOUND, OnBnClickedBtnSelSound)
	ON_BN_CLICKED(IDC_CHECK_HORZ_INV, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_VERT_INV, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheck2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFS_X, OnDeltaposSpinOffsX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFS_Y, OnDeltaposSpinOffsY)
END_MESSAGE_MAP()


// InterfSldrStateProp message handlers

void InterfSldrStateProp::OnBnClickedBtnSelAnim()
{
	CString res = GetIntrfAnimFileName();
	if (res.IsEmpty())
		return;

	ConvertTGAtoQDA convertor;
	res = convertor.convert(res);
	if (res.IsEmpty())
		return ;

	m_strAnimFile = res;
	reset_anim();
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	SetModified();
	redraw_scene(true);
	UpdateData(FALSE);
}

void InterfSldrStateProp::OnBnClickedBtnSelSound()
{
	CString const res = GetIntrfSoundFileName();
	if (res.IsEmpty())
		return;
	m_strSoundFile = res;
	reset_sound();
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	redraw_scene(true);
	UpdateData(FALSE);
}

void InterfSldrStateProp::OnBnClickedCheck()
{
	invert_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	SetModified();
	redraw_scene(true);
}

void InterfSldrStateProp::OnBnClickedCheck1()
{
	invert_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	SetModified();
	redraw_scene(true);
}

void InterfSldrStateProp::OnBnClickedCheck2()
{
	invert_flag(QD_ANIMATION_FLAG_LOOP);
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	SetModified();
	redraw_scene(true);
}

afx_msg void InterfSldrStateProp::OnDeltaposSpinOffsX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	backgroundOffset_.x += pNMUpDown->iDelta;
	UpdateData(FALSE);
	wnd2state();
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	SetModified();
	redraw_scene(true);

	*pResult = 0;
}

afx_msg void InterfSldrStateProp::OnDeltaposSpinOffsY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
	{
		*pResult = 1;
		return;
	}
	backgroundOffset_.y += pNMUpDown->iDelta;
	UpdateData(FALSE);
	wnd2state();
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	SetModified();
	redraw_scene(true);

	*pResult = 0;
}

BOOL InterfSldrStateProp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		switch(::GetDlgCtrlID(pMsg->hwnd)) {
		case IDC_EDIT_ANIM_FILE:
		case IDC_EDIT_SOUND_FILE:
			if (UpdateData())
			{
				wnd2state();
				update_state(*m_ptrForChange);
				m_ptrSlider->set_background_offset(backgroundOffset_);
				SetModified();
				redraw_scene(true);
			}
			return TRUE;
		} 
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

BOOL InterfSldrStateProp::OnApply()
{
	if (!UpdateData())
		return FALSE;

	reset_sound();
	reset_anim();
	update_state(*m_ptrForChange);
	m_ptrSlider->set_background_offset(backgroundOffset_);
	*m_ptrBack = *m_ptrForChange;
	backgroundOffsetBack_ = backgroundOffset_;
	redraw_scene(true);
	return CBasePage::OnApply();
}

void InterfSldrStateProp::OnCancel()
{
	if (GetModified())
	{
		update_state(*m_ptrBack);
		m_ptrSlider->set_background_offset(backgroundOffsetBack_);
		backgroundOffset_ = backgroundOffsetBack_;
		redraw_scene(true);
	}
	CBasePage::OnCancel();
}

BOOL InterfSldrStateProp::OnInitDialog()
{
	CBasePage::OnInitDialog();

	GetDlgItem(IDC_EDIT_SOUND_FILE)->EnableWindow(m_eStateChoose==SC_SLIDER);
	GetDlgItem(IDC_BTN_SEL_SOUND)->EnableWindow(m_eStateChoose==SC_SLIDER);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_OFFS_X))->SetRange(-4000, 4000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_OFFS_Y))->SetRange(-4000, 4000);

	if(m_eStateChoose==SC_BACK){
		GetDlgItem(IDC_EDIT_OFFS_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SPIN_OFFS_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_OFFS_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SPIN_OFFS_Y)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OFFSET)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OFFSET_X)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OFFSET_Y)->ShowWindow(SW_SHOW);
	}
	else {
		GetDlgItem(IDC_EDIT_OFFS_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN_OFFS_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_OFFS_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN_OFFS_Y)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_OFFSET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_OFFSET_X)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_OFFSET_Y)->ShowWindow(SW_HIDE);
	}

	state2wnd();
	backgroundOffsetBack_ = backgroundOffset_;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void InterfSldrStateProp::state2wnd()
{
	m_strAnimFile= m_ptrForChange->animation_file();
	m_strSoundFile = m_ptrForChange->sound_file();

	CButton* pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_HORZ_INV));

	ASSERT(BST_UNCHECKED == static_cast<int>(false));
	ASSERT(BST_CHECKED == static_cast<int>(true));

	pb->SetCheck(
		m_ptrForChange->check_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_VERT_INV));
	pb->SetCheck(m_ptrForChange->check_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL));

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_LOOP));
	pb->SetCheck(m_ptrForChange->check_animation_flag(QD_ANIMATION_FLAG_LOOP));

	backgroundOffset_ = m_ptrSlider->background_offset();
}

void InterfSldrStateProp::wnd2state()
{
	reset_anim();
	reset_sound();
}

void InterfSldrStateProp::invert_flag(int flag)
{
	if (m_ptrForChange->check_animation_flag(flag))
		m_ptrForChange->drop_animation_flag(flag);
	else
		m_ptrForChange->set_animation_flag(flag);
}

void InterfSldrStateProp::reset_anim()
{
	m_strAnimFile.Trim();
	if (m_strAnimFile.IsEmpty()) 
		m_ptrForChange->set_animation_file(NULL);
	else 
		m_ptrForChange->set_animation_file(m_strAnimFile);
}

void InterfSldrStateProp::reset_sound()
{
	m_strSoundFile.Trim();
	if (m_strSoundFile.IsEmpty())
		m_ptrForChange->set_sound_file(NULL);
	else
		m_ptrForChange->set_sound_file(m_strSoundFile);
}

void InterfSldrStateProp::update_state(const qdInterfaceElementState& state)
{
	if (m_eStateChoose == SC_BACK)
		m_ptrSlider->update_background(state);
	else 
		m_ptrSlider->update_slider(state);
	m_ptrSlider->init();
}
