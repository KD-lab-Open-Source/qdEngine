// InterfaceTextWindowPP.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InterfaceTextWindowPP.h"
#include ".\interfacetextwindowpp.h"

#include "disk_op.h"
#include "custom_ddx.h"
#include "file_tests.h"
#include "CurDirSafer.h"

#include "qd_interface_text_window.h"
#include "qd_interface_screen.h"
#include "ConvertTGAtoQDA.h"

#include <boost/rtl.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace{
	//! —охран€ет каталог дл€ файла фонового изображени€
	CurDirSafer& getCurDirSafer(){
		static CurDirSafer safer;
		return safer;
	}
	//! явл€етс€ ли элемент слайдером
	bool is_slider_element(qdInterfaceElement const* element){
		return (element->get_element_type() == qdInterfaceElement::EL_SLIDER);
	}
}

// InterfaceTextWindowPP dialog

IMPLEMENT_DYNAMIC(InterfaceTextWindowPP, CBasePage)
InterfaceTextWindowPP::InterfaceTextWindowPP()
	: CBasePage(InterfaceTextWindowPP::IDD)
	, x_(0)
	, y_(0)
	, z_(0)
	, cx_(0)
	, cy_(0)
	, backGround_(_T(""))
	, sliderName_(_T(""))
	, text_(_T(""))
{
	enableBackColor_ = FALSE;
	backColorAlpha_ = 0;

	stringLength_ = 0;
}

InterfaceTextWindowPP::~InterfaceTextWindowPP()
{
}

void InterfaceTextWindowPP::set_data(qdInterfaceTextWindow* obj){
	sourceObj_ = obj;
	backUp_ = *obj;
}
void InterfaceTextWindowPP::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, x_);
	DDX_Text(pDX, IDC_EDIT_Y, y_);
	DDX_Text(pDX, IDC_EDIT_D, z_);
	DDX_Text(pDX, IDC_EDIT_CX, cx_);
	DDX_Text(pDX, IDC_EDIT_CY, cy_);
	custom_ddx::DDX_Text(pDX, IDC_EDIT_BACK, 
		backGround_, file_tests::non_strict_file_test());
	DDX_CBString(pDX, IDC_COMBO_SLIDER_NAMES, sliderName_);
	DDX_Control(pDX, IDC_COMBO_SLIDER_NAMES, slidersCombo_);
	DDX_Control(pDX, IDC_BACK_COLOR, backColorWnd_);
	DDX_Check(pDX, IDC_CHECK_FILL, enableBackColor_);
	DDX_Slider(pDX, IDC_SLIDER_ALPHA, backColorAlpha_);
	DDX_Control(pDX, IDC_COMBO_SLIDER_TYPES, typesCombo_);
	DDX_CBIndex(pDX, IDC_COMBO_SLIDER_TYPES, windowType_);
	DDX_Control(pDX, IDC_BUTTON_COLOR, textColorWnd_);
	DDX_Control(pDX, IDC_BUTTON_HILITE_COLOR, hiliteColorWnd_);
	DDX_Control(pDX, IDC_COMBO_FONT, fontsWnd_);
	DDX_Control(pDX, IDC_COMBO_ALIGN, alignWnd_);
	DDX_Control(pDX, IDC_COMBO_VALIGN, dialogAlignWnd_);
	DDX_Text(pDX, IDC_EDIT_TEXT_LENGTH, stringLength_);
	DDX_Text(pDX, IDC_EDIT_TEXT, text_);
	DDX_Control(pDX, IDC_COMBO_INFO_TYPE, infoTypeWnd_);
	DDX_Text(pDX, IDC_EDIT_PLAYER_ID, playerID_);
}


BEGIN_MESSAGE_MAP(InterfaceTextWindowPP, CBasePage)
	ON_BN_CLICKED(IDC_BUTTON_SEL_BACKGROUND, OnBnClickedButtonSelBackground)
	ON_CBN_SELCHANGE(IDC_COMBO_SLIDER_NAMES, OnCbnSelchangeComboSliderNames)
	ON_EN_KILLFOCUS(IDC_EDIT_X, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_Y, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_D, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_CX, OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_CY, OnEnKillfocusEdit)
//	ON_EN_KILLFOCUS(IDC_EDIT_BACK, OnEnKillfocusEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_X, OnDeltaposSpinX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_Y, OnDeltaposSpinY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_D, OnDeltaposSpinD)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CX, OnDeltaposSpinCX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CY, OnDeltaposSpinCY)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TEXT_LENGTH, OnDeltaposSpinLength)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PLAYER_ID, OnDeltaposSpinPlayer)
	ON_BN_CLICKED(IDC_CHECK_FILL, OnBnClickedCheckFill)
END_MESSAGE_MAP()


// InterfaceTextWindowPP message handlers

BOOL InterfaceTextWindowPP::OnApply()
{
	if (!UpdateData())
		return FALSE;
	wnd2obj();
	backUp_ = *sourceObj_;
	return CBasePage::OnApply();
}

void InterfaceTextWindowPP::OnCancel()
{
	*sourceObj_ = backUp_;
	CBasePage::OnCancel();
}

BOOL InterfaceTextWindowPP::OnInitDialog()
{
	CBasePage::OnInitDialog();

	CString str;
	str.LoadString(IDS_TEXT_ALIGN_LEFT);
	alignWnd_.AddString(str);
	str.LoadString(IDS_TEXT_ALIGN_CENTER);
	alignWnd_.AddString(str);
	str.LoadString(IDS_TEXT_ALIGN_RIGHT);
	alignWnd_.AddString(str);

	str.LoadString(IDS_INFO_NONE);
	infoTypeWnd_.AddString(str);
	str.LoadString(IDS_INFO_PLAYER_NAME);
	infoTypeWnd_.AddString(str);
	str.LoadString(IDS_INFO_PLAYER_SCORE);
	infoTypeWnd_.AddString(str);

	str.LoadString(IDS_VALIGN_TOP);
	dialogAlignWnd_.AddString(str);
	str.LoadString(IDS_VALIGN_CENTER);
	dialogAlignWnd_.AddString(str);
	str.LoadString(IDS_VALIGN_BOTTOM);
	dialogAlignWnd_.AddString(str);

	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->SetRange(- 10000, 1000);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->SetRange(- 10000, 1000);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_D))->SetRange(- 10000, 1000);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_CX))->SetRange(- 10000, 1000);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_CY))->SetRange(- 10000, 1000);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TEXT_LENGTH))->SetRange(-10000, 10000);
	reinterpret_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_PLAYER_ID))->SetRange(-10000, 10000);
	loadSliderList();

	typesCombo_.AddString(CString((LPTSTR)IDS_WINDOW_DIALOGS));
	typesCombo_.AddString(CString((LPTSTR)IDS_WINDOW_EDIT));
	typesCombo_.AddString(CString((LPTSTR)IDS_WINDOW_TEXT));

	InitFontBox(fontsWnd_);

	obj2wnd();

	if(!enableBackColor_){
		backColorWnd_.EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_ALPHA)->EnableWindow(FALSE);
	}
	else {
		backColorWnd_.EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_ALPHA)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void InterfaceTextWindowPP::OnBnClickedButtonSelBackground()
{
	CString filter;
	filter.LoadString(IDS_OPEN_QDA_TGA_FILTER);
	CString res = disk_op::Get1DiskFileName(getCurDirSafer, filter, NULL, NULL);
	if (res.IsEmpty())
		return;

	ConvertTGAtoQDA convertor;
	res = convertor.convert(res);
	if (res.IsEmpty())
		return;

	backGround_ = res;
	sourceObj_->set_border_background_file(backGround_);
	sourceObj_->init();

	SetModified();
	UpdateData(FALSE);
	redraw_scene(true);
}

void InterfaceTextWindowPP::OnCbnSelchangeComboSliderNames()
{
	SetModified();
}

void InterfaceTextWindowPP::wnd2obj()
{
	sourceObj_->set_input_string(text_);

	sourceObj_->set_r(Vect2i(x_, y_));
	sourceObj_->set_screen_depth(z_);
	sourceObj_->set_text_size(Vect2i(cx_, cy_));

	sourceObj_->set_border_background_file(backGround_);
	sourceObj_->set_background_color(backColorWnd_.GetColor());
	sourceObj_->toggle_background_color(enableBackColor_);
	sourceObj_->set_background_alpha(backColorAlpha_ * 255 / 100);
	sourceObj_->set_slider_name(sliderName_);

	sourceObj_->setInputStringLimit(stringLength_);

	sourceObj_->setWindowType(qdInterfaceTextWindow::WindowType(windowType_));

	sourceObj_->set_text_valign(qdInterfaceTextWindow::TextVAlign(dialogAlignWnd_.GetCurSel()));

	qdScreenTextFormat format;
	format.toggle_global_depend(false);

	format.set_color(textColorWnd_.GetColor());
	format.set_hover_color(hiliteColorWnd_.GetColor());

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
	format.set_alignment(qdScreenTextFormat::alignment_t(alignWnd_.GetCurSel()));

	sourceObj_->setInfoType(qdInterfaceTextWindow::InfoType(infoTypeWnd_.GetCurSel()));
	sourceObj_->setPlayerID(playerID_);

	sourceObj_->setTextFormat(format);
	sourceObj_->init();
}

void InterfaceTextWindowPP::obj2wnd()
{
	text_ = sourceObj_->input_string();

	Vect2i const& r = sourceObj_->r();
	x_ = r.x;
	y_ = r.y;
	z_ = sourceObj_->screen_depth();
	cx_ = sourceObj_->text_size().x;
	cy_ = sourceObj_->text_size().y;

	backGround_			 = sourceObj_->border_background_file();
	sliderName_			 = sourceObj_->slider_name();

	windowType_ = sourceObj_->windowType();

	backColorWnd_.SetColor(sourceObj_->background_color());
	enableBackColor_ = sourceObj_->has_background_color();
	backColorAlpha_ = sourceObj_->background_alpha() * 100 / 255;

	stringLength_ = sourceObj_->inputStringLimit();

	qdScreenTextFormat format = sourceObj_->textFormat();

	textColorWnd_.SetColor(static_cast<COLORREF>(format.color()));
	hiliteColorWnd_.SetColor(static_cast<COLORREF>(format.hover_color()));
	if (format.font_type() == QD_FONT_TYPE_NONE)
		fontsWnd_.SetCurSel(0);
	else
	{
		if(!qdGameDispatcher::get_dispatcher())
		{
			ASSERT(0);
			return;
		}
		qdFontInfo const* fi = qdGameDispatcher::get_dispatcher()->get_font_info(format.font_type());
		if (!fi||fontsWnd_.SelectString(-1, fi->name()) == CB_ERR)
			fontsWnd_.SetCurSel(0);
	}
	alignWnd_.SetCurSel(format.alignment());

	dialogAlignWnd_.SetCurSel(sourceObj_->text_valign());

	infoTypeWnd_.SetCurSel(sourceObj_->infoType());
	playerID_ = sourceObj_->playerID();
}

BOOL InterfaceTextWindowPP::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		switch(::GetDlgCtrlID(pMsg->hwnd)) {
		case IDC_EDIT_X:
		case IDC_EDIT_Y:
		case IDC_EDIT_D:
		case IDC_EDIT_CX:
		case IDC_EDIT_CY:
			if (UpdateData())
			{
				wnd2obj();
				SetModified();
				redraw_scene(true);
				return TRUE;
			}
		}
	}
	return CBasePage::PreTranslateMessage(pMsg);
}

void InterfaceTextWindowPP::OnEnKillfocusEdit()
{
	if (!UpdateData())
		return;
	wnd2obj();
	SetModified();
	redraw_scene(true);
}

void InterfaceTextWindowPP::OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	x_ += pNMUpDown->iDelta;
	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	redraw_scene(true);
	*pResult = 0;
}

void InterfaceTextWindowPP::OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	y_ += pNMUpDown->iDelta;
	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	redraw_scene(true);
	*pResult = 0;
}

void InterfaceTextWindowPP::OnDeltaposSpinD(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	z_ += pNMUpDown->iDelta;
	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	redraw_scene(true);
	*pResult = 0;
}

void InterfaceTextWindowPP::OnDeltaposSpinCX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	cx_ += pNMUpDown->iDelta;
	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	redraw_scene(true);
	*pResult = 0;
}

void InterfaceTextWindowPP::OnDeltaposSpinCY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	cy_ += pNMUpDown->iDelta;
	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	redraw_scene(true);
	*pResult = 0;
}

void InterfaceTextWindowPP::OnDeltaposSpinLength(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	stringLength_ += pNMUpDown->iDelta;

	if(stringLength_ < 0)
		stringLength_ = 0;

	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void InterfaceTextWindowPP::OnDeltaposSpinPlayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	playerID_ += pNMUpDown->iDelta;

	if(playerID_ < 0)
		playerID_ = 0;

	wnd2obj();
	UpdateData(FALSE);
	SetModified();
	*pResult = 0;
}

void InterfaceTextWindowPP::loadSliderList()
{
	qdInterfaceScreen* screen = static_cast<qdInterfaceScreen*>(sourceObj_->owner());
	boost::rtl::filtered_range
		<
		const qdInterfaceScreen::element_list_t, 
		bool (*)(qdInterfaceElement const*)
		> range(screen->element_list(), is_slider_element);
	for(; range; ++range)
		slidersCombo_.AddString((*range)->name());

	if (!slidersCombo_.GetCount())
		slidersCombo_.EnableWindow(FALSE);
}

void InterfaceTextWindowPP::InitFontBox(CCJFlatComboBox& box)
{
	box.Clear();
	if (!qdGameDispatcher::get_dispatcher())
	{
		ASSERT(0);
		return;
	}
	boost::rtl::crange<qdFontInfoList const> r(qdGameDispatcher::get_dispatcher()->fonts_list());
	for(; r; ++r)
	{
		int item = box.AddString((*r)->name());
		if (item != CB_ERR)
			box.SetItemDataPtr(item, *r);
	}
	CString str;
	str.LoadString(IDS_DEFAULT_FONT);
	box.InsertString(0, str);
}

void InterfaceTextWindowPP::OnBnClickedCheckFill()
{
	UpdateData();
	SetModified();

	if(!enableBackColor_){
		backColorWnd_.EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_ALPHA)->EnableWindow(FALSE);
	}
	else {
		backColorWnd_.EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_ALPHA)->EnableWindow(TRUE);
	}
}
