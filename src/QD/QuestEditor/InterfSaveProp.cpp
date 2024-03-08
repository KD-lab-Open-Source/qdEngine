// InterfSaveProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfSaveProp.h"
#include "interf_disk_op.h"
#include "sv_api.h"
#include "qd_interface_save.h"
#include "qd_interface_screen.h"

#include "custom_ddx.h"
#include "file_tests.h"
#include "ConvertTGAtoQDA.h"

// InterfSaveProp dialog

IMPLEMENT_DYNAMIC(InterfSaveProp, CBasePage)
InterfSaveProp::InterfSaveProp()
	: CBasePage(InterfSaveProp::IDD)
	, m_strAnimFile(_T(""))
	, m_strSoundFile(_T(""))
	, m_iX(0)
	, m_iY(0)
	, m_iD(0)
	, m_iSX(0)
	, m_iSY(0)
	, m_iSaveId(0)
	, m_iTextDx(0)
	, m_iTextDy(0)
	, isAutosaveSlot_(false)
{
}

InterfSaveProp::~InterfSaveProp()
{
}

void InterfSaveProp::set_data(qdInterfaceSave* psave)
{
	m_ptrSourceSave = psave;
	m_ptrBack.reset(new qdInterfaceSave(*psave));
}

void InterfSaveProp::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_ANIM_FILE, m_strAnimFile, 
		file_tests::non_strict_file_test());
	custom_ddx::DDX_Text(pDX, IDC_EDIT_SOUND_FILE, m_strSoundFile,
		file_tests::non_strict_file_test());
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_X2, m_iY);
	DDX_Text(pDX, IDC_EDIT_D, m_iD);
	DDX_Text(pDX, IDC_EDIT_SX, m_iSX);
	DDX_Text(pDX, IDC_EDIT_SY, m_iSY);
	DDX_Text(pDX, IDC_EDIT_SAVEID, m_iSaveId);
	DDX_Control(pDX, IDC_COMBO_MODES, m_wndModes);
	DDX_Text(pDX, IDC_EDIT_TEXT_DX, m_iTextDx);
	DDX_Text(pDX, IDC_EDIT_TEXT_DY, m_iTextDy);
	DDX_Check(pDX, IDC_CHECK_AUTOSAVE, isAutosaveSlot_);
}


BEGIN_MESSAGE_MAP(InterfSaveProp, CBasePage)
	ON_BN_CLICKED(IDC_BTN_SEL_SOUND, OnBnClickedBtnSelSound)
	ON_BN_CLICKED(IDC_BTN_SEL_ANIM, OnBnClickedBtnSelAnim)
	ON_BN_CLICKED(IDC_CHECK_HORZ_INV, OnBnClickedCheckHorzInv)
	ON_BN_CLICKED(IDC_CHECK_VERT_INV, OnBnClickedCheckVertInv)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnBnClickedCheckLoop)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_D, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TEXT_DX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TEXT_DY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAVEID, OnDeltaposSpin)
	ON_CBN_SELCHANGE(IDC_COMBO_MODES, OnCbnSelchangeComboModes)
END_MESSAGE_MAP()


BOOL InterfSaveProp::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();
	int const SPIN_MAX = 10000;
	int const SPIN_MIN = -10000;
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_D))->SetRange(SPIN_MIN, SPIN_MAX);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SX))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SY))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TEXT_DX))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TEXT_DY))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SAVEID))
					->SetRange(SPIN_MIN, SPIN_MAX);

	LoadModes();
	save2wnd();
	UpdateData(FALSE);
	return res;
}
// InterfSaveProp message handlers

void InterfSaveProp::OnBnClickedBtnSelSound()
{
	CString const res = GetIntrfSoundFileName();
	if (res.IsEmpty())
		return;
	m_strSoundFile = res;
	wnd2save();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	UpdateData(FALSE);
	SetModified();
}

void InterfSaveProp::OnBnClickedBtnSelAnim()
{
	CString res = GetIntrfAnimFileName();
	if (res.IsEmpty())
		return;
	ConvertTGAtoQDA convertor;
	res = convertor.convert(res);
	if (res.IsEmpty())
		return;
	m_strAnimFile = res;
	wnd2save();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	UpdateData(FALSE);
	SetModified();
}

void InterfSaveProp::OnBnClickedCheckHorzInv()
{
	qdInterfaceElementState::state_mode_t const md = GetCurMode();
	if (m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, md)) 
		m_ptrSourceSave->drop_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, md);
	else
		m_ptrSourceSave->set_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, md);

	static_cast<qdInterfaceSave*>(m_ptrSourceSave)->init();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	SetModified();
}

void InterfSaveProp::OnBnClickedCheckVertInv()
{
	qdInterfaceElementState::state_mode_t const md = GetCurMode();
	if (m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, md)) 
		m_ptrSourceSave->drop_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, md);
	else
		m_ptrSourceSave->set_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, md);

	static_cast<qdInterfaceSave*>(m_ptrSourceSave)->init();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	SetModified();
}

void InterfSaveProp::OnBnClickedCheckLoop()
{
	qdInterfaceElementState::state_mode_t const md = GetCurMode();
	if (m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_LOOP, md)) 
		m_ptrSourceSave->drop_frame_animation_flag(QD_ANIMATION_FLAG_LOOP, md);
	else
		m_ptrSourceSave->set_frame_animation_flag(QD_ANIMATION_FLAG_LOOP, md);

	static_cast<qdInterfaceSave*>(m_ptrSourceSave)->init();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);

	SetModified();
}

void InterfSaveProp::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	switch(pNMUpDown->hdr.idFrom)
	{
	case IDC_SPIN_X:
		m_iX += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_Y:
		m_iY += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_D:
		m_iD += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_SX:
		m_iSX += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_SY:
		m_iSY += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_TEXT_DX:
		m_iTextDx += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_TEXT_DY:
		m_iTextDy += pNMUpDown->iDelta;
		break;
	case IDC_SPIN_SAVEID:
		m_iSaveId += pNMUpDown->iDelta;
		break;
	}
	OnChangePosSize();
	*pResult = 0;
}

BOOL InterfSaveProp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		switch(::GetDlgCtrlID(pMsg->hwnd)) {
		case IDC_EDIT_ANIM_FILE:
		case IDC_EDIT_SOUND_FILE:
			if (UpdateData())
			{
				reset_files();
				SetModified();
			}
			return TRUE;
		case IDC_EDIT_X:
		case IDC_EDIT_X2:
		case IDC_EDIT_D:
		case IDC_EDIT_SX:
		case IDC_EDIT_SY:
		case IDC_EDIT_TEXT_DX:
		case IDC_EDIT_TEXT_DY:
		case IDC_EDIT_SAVEID:
			if (!UpdateData())
				return TRUE;
			m_ptrSourceSave->set_save_ID(m_iSaveId);
			OnChangePosSize();
		return TRUE;
		} 
	}
	return __super::PreTranslateMessage(pMsg);
}

BOOL InterfSaveProp::OnApply()
{
	if (!UpdateData())
		return FALSE;
	wnd2save();
	*m_ptrBack.get() = *m_ptrSourceSave;
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	return __super::OnApply();
}

void InterfSaveProp::OnCancel()
{
	*m_ptrSourceSave = *m_ptrBack.get();
	m_ptrSourceSave->init();
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	__super::OnCancel();
}

void InterfSaveProp::wnd2save()
{
	m_ptrSourceSave->set_r(Vect2i(m_iX, m_iY));
	m_ptrSourceSave->set_screen_depth(m_iD);
	static_cast<qdInterfaceScreen*>(m_ptrSourceSave->owner())
		->build_visible_elements_list();
	m_ptrSourceSave->set_thumbnail_size_x(m_iSX);
	m_ptrSourceSave->set_thumbnail_size_y(m_iSY);
	m_ptrSourceSave->set_test_dx(m_iTextDx);
	m_ptrSourceSave->set_test_dy(m_iTextDy);
//	m_ptrSourceSave->set_frame_animation_file(m_strAnimFile);
//	m_ptrSourceSave->set_frame_sound_file(m_strSoundFile);
	m_ptrSourceSave->setAutosaveSlot(isAutosaveSlot_);
	wnd2mode(GetCurMode());
	m_ptrSourceSave->set_save_ID(m_iSaveId);
	m_ptrSourceSave->init();
}

void InterfSaveProp::save2wnd()
{
	m_iX = m_ptrSourceSave->r().x;
	m_iY = m_ptrSourceSave->r().y;
	m_iD = m_ptrSourceSave->screen_depth();

	m_iSX = m_ptrSourceSave->thumbnail_size_x();
	m_iSY = m_ptrSourceSave->thumbnail_size_y();

	m_iTextDx = m_ptrSourceSave->text_dx();
	m_iTextDy = m_ptrSourceSave->text_dy();

	isAutosaveSlot_ = m_ptrSourceSave->isAutosaveSlot();

/*
	m_strAnimFile = m_ptrSourceSave->frame_animation_file();
	m_strSoundFile = m_ptrSourceSave->frame_sound_file();

	CButton* pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_HORZ_INV));

	ASSERT(BST_UNCHECKED == static_cast<int>(false));
	ASSERT(BST_CHECKED == static_cast<int>(true));

	pb->SetCheck(
		m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_VERT_INV));
	pb->SetCheck(
		m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_LOOP));
	pb->SetCheck(
		m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_LOOP)); 
*/
	mode2wnd(GetCurMode());
	m_iSaveId = m_ptrSourceSave->save_ID();
}

void InterfSaveProp::OnChangePosSize()
{
	wnd2save();
	UpdateData(FALSE);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	SetModified();
}

void InterfSaveProp::reset_files()
{
	m_ptrSourceSave->set_frame_animation_file(m_strAnimFile);
	m_ptrSourceSave->set_frame_sound_file(m_strSoundFile);
	m_ptrSourceSave->init();
}

qdInterfaceElementState::state_mode_t InterfSaveProp::GetAnotherMode(
	qdInterfaceElementState::state_mode_t const m)
{
	if (m == qdInterfaceElementState::MOUSE_HOVER_MODE)
		return qdInterfaceElementState::DEFAULT_MODE;
	else
		return qdInterfaceElementState::MOUSE_HOVER_MODE;
}
qdInterfaceElementState::state_mode_t InterfSaveProp::GetCurMode()
{
	ASSERT(m_wndModes.GetCurSel() != CB_ERR);
	return static_cast<qdInterfaceElementState::state_mode_t>
		(m_wndModes.GetItemData(m_wndModes.GetCurSel()));
}

void InterfSaveProp::OnCbnSelchangeComboModes()
{
	const int iCurSel = m_wndModes.GetCurSel();
	if (iCurSel == CB_ERR)
		return;
	qdInterfaceElementState::state_mode_t const md = GetCurMode();
	wnd2mode(GetAnotherMode(md));
	mode2wnd(md);

	m_ptrSourceSave->init();
	UpdateData(FALSE);
	sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
}
void InterfSaveProp::mode2wnd(qdInterfaceElementState::state_mode_t const m)
{
	m_strAnimFile = m_ptrSourceSave->frame_animation_file(m);
	m_strSoundFile = m_ptrSourceSave->frame_sound_file(m);

	CButton* pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_HORZ_INV));

	ASSERT(BST_UNCHECKED == static_cast<int>(false));
	ASSERT(BST_CHECKED == static_cast<int>(true));

	pb->SetCheck(
		m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL, m)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_VERT_INV));
	pb->SetCheck(
		m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL, m)); 

	pb = static_cast<CButton*>(GetDlgItem(IDC_CHECK_LOOP));
	pb->SetCheck(
		m_ptrSourceSave->check_frame_animation_flag(QD_ANIMATION_FLAG_LOOP, m)); 

}

void InterfSaveProp::wnd2mode(qdInterfaceElementState::state_mode_t const md)
{
	m_strAnimFile.Trim();
	if (m_strAnimFile.IsEmpty())
		m_ptrSourceSave->set_frame_animation_file(NULL, md);
	else
		m_ptrSourceSave->set_frame_animation_file(m_strAnimFile, md);

	m_strSoundFile.Trim();
	if (m_strSoundFile.IsEmpty())
		m_ptrSourceSave->set_frame_sound_file(NULL, md);
	else
		m_ptrSourceSave->set_frame_sound_file(m_strSoundFile, md);
}

// InterfSaveProp message handlers

void InterfSaveProp::LoadModes()
{
	CString str((LPTSTR)IDS_INTRF_ELE_STATE_MODE_DEFAULT);
	int iItem = m_wndModes.AddString(str);
	if (iItem != CB_ERR)
		m_wndModes.SetItemData(iItem, 
			static_cast<DWORD_PTR>(qdInterfaceElementState::DEFAULT_MODE));
	str.LoadString(IDS_INTRF_ELE_STATE_MODE_MOUSE_OVER);
	iItem = m_wndModes.AddString(str);
	if (iItem != CB_ERR)
		m_wndModes.SetItemData(iItem, 
		static_cast<DWORD_PTR>(qdInterfaceElementState::MOUSE_HOVER_MODE));

/*
	str.LoadString(IDS_INTRF_ELE_STATE_MODE_EVENT);
	iItem = m_wndModes.AddString(str);
	if (iItem != CB_ERR)
		m_wndModes.SetItemData(iItem, 
		static_cast<DWORD_PTR>(qdInterfaceElementState::EVENT_MODE));
*/
	m_wndModes.SetCurSel(0);
}
