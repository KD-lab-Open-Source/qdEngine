// InterfCounterPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfCounterPP.h"
#include "custom_ddx.h"
#include "interf_disk_op.h"
#include "file_tests.h"

#include <Shlwapi.h>

#include <boost/rtl.hpp>

#include "qd_interface_counter.h"
#include "qd_interface_screen.h"
#include "qd_counter.h"

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

/*
namespace {
	//! Устанавливает/удаляет указанный флаг для фона
	void SetFlag(qdInterfaceCounter* pb, BOOL bSet, int iFlag)
	{
		if (bSet)
			pb->set_animation_flag(iFlag);
		else
			pb->drop_animation_flag(iFlag);
	}
}
*/

// InterfCounterPP dialog

IMPLEMENT_DYNAMIC(InterfCounterPP, CBasePage)
InterfCounterPP::InterfCounterPP()
	: CBasePage(InterfCounterPP::IDD)
	, m_iX(0)
	, m_iY(0)
	, m_iD(0)
	, m_ptrCounter(NULL)
{
	digits_ = 4;
}

InterfCounterPP::~InterfCounterPP()
{
}

bool InterfCounterPP::set_data(qdInterfaceCounter* ptrCounter)
{
	m_ptrCounter = ptrCounter;
	m_ptrCopy.reset(new qdInterfaceCounter(*ptrCounter));
	return true;
}

void InterfCounterPP::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_iX);
	DDX_Text(pDX, IDC_EDIT_Y, m_iY);
	DDX_Text(pDX, IDC_EDIT_D, m_iD);
	DDX_Text(pDX, IDC_EDIT_DIGITS, digits_);
	DDX_Control(pDX, IDC_COMBO_COUNTER, counters_);
	DDX_Control(pDX, IDC_BUTTON_COLOR, textColorWnd_);
	DDX_Control(pDX, IDC_COMBO_FONT, fontsWnd_);
}


BEGIN_MESSAGE_MAP(InterfCounterPP, CBasePage)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


BOOL InterfCounterPP::OnApply()
{
	if (!UpdateData())
		return FALSE;
	wnd2ele();
	*m_ptrCopy.get() = *m_ptrCounter;
	redraw_scene(true);
	return CBasePage::OnApply();
}

void InterfCounterPP::OnCancel()
{
	*m_ptrCounter = *m_ptrCopy.get();
	m_ptrCounter->init();
	redraw_scene(true);
	CBasePage::OnCancel();
}

BOOL InterfCounterPP::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_X)
				||pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_Y)
				||pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_D)) 
			{
				OnChangePos();
				return TRUE;
			}
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

BOOL InterfCounterPP::OnInitDialog()
{
	CBasePage::OnInitDialog();

	loadCounters(counters_);
	loadFonts(fontsWnd_);

	const int SPIN_MAX = 10000;
	const int SPIN_MIN = -10000;
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(SPIN_MIN, SPIN_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_D))->SetRange(SPIN_MIN, SPIN_MAX);

	const int SPIN_MAX1 = 16;
	const int SPIN_MIN1 = 0;
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_DIGITS))->SetRange(SPIN_MIN1, SPIN_MAX1);

	ele2wnd();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// InterfCounterPP message handlers

void InterfCounterPP::ele2wnd()
{
	const Vect2i& vPos = m_ptrCounter->r();
	m_iX = vPos.x;
	m_iY = vPos.y;
	m_iD = m_ptrCounter->screen_depth();

	digits_ = m_ptrCounter->digits();

	qdScreenTextFormat format = m_ptrCounter->textFormat();

	textColorWnd_.SetColor(static_cast<COLORREF>(format.color()));
	if (format.font_type() == QD_FONT_TYPE_NONE)
		fontsWnd_.SetCurSel(0);
	else
	{
		qdGameDispatcher* dsp = qdGameDispatcher::get_dispatcher();
		ASSERT(dsp);
		qdFontInfo const* fi = dsp->get_font_info(format.font_type());
		if(!fi||fontsWnd_.SelectString(-1, fi->name()) == CB_ERR)
			fontsWnd_.SetCurSel(0);
	}

	if(counters_.SelectString(-1, m_ptrCounter->counterName()) == CB_ERR)
		counters_.SetCurSel(0);
}

void InterfCounterPP::wnd2ele()
{
	m_ptrCounter->set_screen_depth(m_iD);
	m_ptrCounter->set_r(Vect2i(m_iX, m_iY));

	m_ptrCounter->setDigits(digits_);

	qdScreenTextFormat format;
	format.toggle_global_depend(false);
	format.set_color(textColorWnd_.GetColor());

	int font_type;
	int curSel = fontsWnd_.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		font_type =  QD_FONT_TYPE_NONE;
	else
	{
		qdFontInfo const* fi = 
			reinterpret_cast<qdFontInfo*>(fontsWnd_.GetItemDataPtr(curSel));
		font_type =  fi->type();
	}
	format.set_font_type(font_type);
	m_ptrCounter->setTextFormat(format);

	int const cur_sel = counters_.GetCurSel();
	if(cur_sel == CB_ERR || !cur_sel)
		m_ptrCounter->setCounterName("");
	else {
		CString str;
		counters_.GetLBText(cur_sel, str);
		m_ptrCounter->setCounterName(str);
	}

	m_ptrCounter->init();
	static_cast<qdInterfaceScreen*>(m_ptrCounter->owner())->build_visible_elements_list();
}

bool InterfCounterPP::loadCounters(CCJFlatComboBox& box)
{
	qdGameDispatcher* dsp = qdGameDispatcher::get_dispatcher();
	ASSERT(dsp);
	qdCounterList const& lst = dsp->counter_list();

	box.EnableWindow(!lst.empty());

	for(qdCounterList::const_iterator it = lst.begin(); it != lst.end(); ++it){
		box.AddString((*it)->name());
	}

	box.InsertString(0, CString((LPTSTR)IDS_NOTHING));

	AdjustListWidth(&box);
	box.SetCurSel(0);
	return true;
}

bool InterfCounterPP::loadFonts(CCJFlatComboBox& box)
{
	box.Clear();
	qdGameDispatcher* dsp = qdGameDispatcher::get_dispatcher();
	ASSERT(dsp);

	boost::rtl::crange<qdFontInfoList const> r(dsp->fonts_list());
	for(; r; ++r)
	{
		int item = box.AddString((*r)->name());
		if (item != CB_ERR)
			box.SetItemDataPtr(item, *r);
	}
	CString str;
	str.LoadString(IDS_DEFAULT_FONT);
	box.InsertString(0, str);

	return true;
}

void InterfCounterPP::OnChangePos()
{
	if (!UpdateData())
		return;
	SetModified();
	wnd2ele();
	redraw_scene(true);
}

void InterfCounterPP::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int id = pScrollBar->GetDlgCtrlID();
	if (id == IDC_SPIN_X||id == IDC_SPIN_Y||id == IDC_SPIN_D)
	{
		if (nSBCode == SB_ENDSCROLL) 
			OnChangePos();
	}
	CBasePage::OnVScroll(nSBCode, nPos, pScrollBar);
}

