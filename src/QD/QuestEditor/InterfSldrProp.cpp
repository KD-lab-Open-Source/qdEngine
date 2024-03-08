// InterfSldrProp.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfSldrProp.h"
#include "qd_interface_slider.h"
#include "qd_interface_screen.h"


// InterfSldrProp dialog

IMPLEMENT_DYNAMIC(InterfSldrProp, CBasePage)
InterfSldrProp::InterfSldrProp()
	: CBasePage(InterfSldrProp::IDD)
	, m_iX(0)
	, m_iY(0)
	, m_iD(0)
	, m_iOrient(FALSE)
	, inverseDirection_(FALSE)
	, m_pSlider(NULL)
	, m_iARSX(0)
	, m_iARSY(0)
{
}

InterfSldrProp::~InterfSldrProp()
{
}

void InterfSldrProp::set_data(class qdInterfaceSlider* pslider)
{
	m_pSlider = pslider;
}

void InterfSldrProp::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_X2, m_iY);
	DDX_Text(pDX, IDC_EDIT_D, m_iD);
	DDX_Radio(pDX, IDC_ORI_HORZ, m_iOrient);
	DDX_Check(pDX, IDC_CHECK_INVERSE_DIRECTION, inverseDirection_);
	DDX_Control(pDX, IDC_COMBO_TARGET, m_wndTarget);
	DDX_Text(pDX, IDC_EDIT_AR_SX, m_iARSX);
	DDX_Text(pDX, IDC_EDIT_AR_SY, m_iARSY);
}

BEGIN_MESSAGE_MAP(InterfSldrProp, CBasePage)
	ON_BN_CLICKED(IDC_ORI_HORZ, OnBnClickedOriHorz)
	ON_BN_CLICKED(IDC_ORI_VERT, OnBnClickedOriVert)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, OnDeltaposSpinX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpinY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_D, OnDeltaposSpinD)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGET, OnCbnSelchangeComboTarget)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AR_SX, OnDeltaposSpinArSx)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AR_SY, OnDeltaposSpinArSy)
END_MESSAGE_MAP()


// InterfSldrProp message handlers

void InterfSldrProp::OnBnClickedOriHorz()
{
	SetModified();

	m_pSlider->set_orientation(qdInterfaceSlider::SL_HORIZONTAL);
	m_pSlider->init();
	redraw_scene(true);
}

void InterfSldrProp::OnBnClickedOriVert()
{
	SetModified();

	m_pSlider->set_orientation(qdInterfaceSlider::SL_VERTICAL);
	m_pSlider->init();
	redraw_scene(true);
}

void InterfSldrProp::OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	m_iX += pNMUpDown->iDelta;
	update_pos();
	SetModified();
	UpdateData(FALSE);
	redraw_scene(true);
	*pResult = 0;
}

void InterfSldrProp::OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	m_iY += pNMUpDown->iDelta;
	update_pos();
	SetModified();
	UpdateData(FALSE);
	redraw_scene(true);
	*pResult = 0;
}

void InterfSldrProp::OnDeltaposSpinD(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	m_iD += pNMUpDown->iDelta;
	update_pos();
	SetModified();
	UpdateData(FALSE);
	redraw_scene(true);
	*pResult = 0;
}

void InterfSldrProp::OnDeltaposSpinArSx(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iARSX += pNMUpDown->iDelta;
	update_ar();
	SetModified();
	UpdateData(FALSE);
	redraw_scene(true);

	*pResult = 0;
}

void InterfSldrProp::OnDeltaposSpinArSy(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iARSY += pNMUpDown->iDelta;
	update_ar();
	SetModified();
	UpdateData(FALSE);
	redraw_scene(true);

	*pResult = 0;
}


void InterfSldrProp::bak2real()
{
	m_iX = m_iXBak;
	m_iY = m_iYBak;
	m_iD = m_iDBak;
	m_iOrient = m_iOrientBak;
	inverseDirection_ = inverseDirectionBak_;
	m_iARSY = m_iARSYBak;
	m_iARSX = m_iARSXBak;
	m_pSlider->set_option_ID(m_eOptionIdBak);
	purpose2wnd();//для того чтобы в OnCancel установилось нужное нам значение
				//можно было бы завести еще одну переменную(m_eOptionId),
				//но я счел это излишним
}

void InterfSldrProp::real2bak()
{
	m_iXBak = m_iX;
	m_iYBak = m_iY;
	m_iDBak = m_iD;
	m_iOrientBak = m_iOrient;
	inverseDirectionBak_ = inverseDirection_;
	m_iARSXBak = m_iARSX;
	m_iARSYBak = m_iARSY;
	m_eOptionIdBak = m_pSlider->option_ID();
}

void InterfSldrProp::update_pos()
{
	m_pSlider->set_r(Vect2i(m_iX, m_iY));
	m_pSlider->set_screen_depth(m_iD);
	static_cast<qdInterfaceScreen*>(m_pSlider->owner())->build_visible_elements_list();
}

void InterfSldrProp::update_ar()
{
	m_pSlider->set_active_rectangle(Vect2i(m_iARSX, m_iARSY));
}

void InterfSldrProp::wnd2slider()
{
	update_pos();
	update_ar();
	wnd2purpose();
	m_pSlider->set_orientation(static_cast<qdInterfaceSlider::orientation_t>(m_iOrient));
	m_pSlider->set_inverse_direction(inverseDirection_);
	m_pSlider->init();
}

void InterfSldrProp::slider2wnd()
{
	m_iX = m_pSlider->r().x;
	m_iY = m_pSlider->r().y;
	m_iD = m_pSlider->screen_depth();
	m_iOrient = m_pSlider->orientation();
	inverseDirection_ = m_pSlider->inverse_direction();

	m_iARSX = m_pSlider->active_rectangle().x;
	m_iARSY = m_pSlider->active_rectangle().y;
	purpose2wnd();
}

BOOL InterfSldrProp::OnInitDialog()
{
	CBasePage::OnInitDialog();
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(0, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(0, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_D))->SetRange(0, 100);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_AR_SX))->SetRange(0, 100);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_AR_SY))->SetRange(0, 100);
	LoadPurposes();

	slider2wnd();
	real2bak();

	UpdateData(FALSE);

	return TRUE;
}

BOOL InterfSldrProp::OnApply()
{
	if (!UpdateData())
		return FALSE;
	real2bak();
	wnd2slider();
	redraw_scene(true);
	return CBasePage::OnApply();
}

void InterfSldrProp::OnCancel()
{
	bak2real();
	wnd2slider();
	redraw_scene(true);
	CBasePage::OnCancel();
}

BOOL InterfSldrProp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		switch (::GetDlgCtrlID(pMsg->hwnd))
		{
		case IDC_EDIT_X:
		case IDC_EDIT_X2:
		case IDC_EDIT_D:
		case IDC_EDIT_AR_SX:
		case IDC_EDIT_AR_SY:
			if (UpdateData())
			{
				SetModified();
				update_pos();
				update_ar();
				redraw_scene(true);
			}
			return TRUE;
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

void InterfSldrProp::OnCbnSelchangeComboTarget()
{
	SetModified();
}

void InterfSldrProp::LoadPurposes(){
	int const count = m_Purposes.count();
	for(int i = 0; i < count; ++i)
	{
		int const item = m_wndTarget.AddString(m_Purposes.get_descr(i));
		if (item != CB_ERR)
			m_wndTarget.SetItemData(item, 
				static_cast<DWORD_PTR>(m_Purposes.get_opt_id(i)));
	}
	m_wndTarget.SetCurSel(0);
}

void InterfSldrProp::purpose2wnd(){
	m_wndTarget.SelectString(-1, m_Purposes.get_descr(m_pSlider->option_ID()));
}

void InterfSldrProp::wnd2purpose(){
	int const cur_sel = m_wndTarget.GetCurSel();
	if (cur_sel == CB_ERR)
	{
		m_pSlider->set_option_ID(qdInterfaceElement::OPTION_NONE);
		return;
	}
	qdInterfaceElement::option_ID_t id =
		static_cast<qdInterfaceElement::option_ID_t>(m_wndTarget.GetItemData(cur_sel));
	m_pSlider->set_option_ID(id);
}