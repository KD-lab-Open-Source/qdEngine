// ScreenTextPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "ScreenTextPropPage.h"
#include ".\screentextproppage.h"
#include "qd_screen_text_set.h"


// ScreenTextPropPage dialog

IMPLEMENT_DYNAMIC(ScreenTextPropPage, CBasePage)
ScreenTextPropPage::ScreenTextPropPage()
	: CBasePage(ScreenTextPropPage::IDD)
	, iHorzAlign_(FALSE)
	, iVertAlign_(FALSE)
	, iLeft_(0)
	, iTop_(0)
	, iSX_(0)
	, iSY_(0)
	, iOrientation_(FALSE)
{
}

ScreenTextPropPage::~ScreenTextPropPage()
{
}

void ScreenTextPropPage::set_data(qdScreenTextDispatcher* pstdisp, qdScreenTextSet* pset)
{
	screenTextSetCopy_.reset(new qdScreenTextSet(*pset));
	if (!screenTextSetCopy_)
		return;

	pScreenTextSet_ = pset;
	pScreenTextDisp_ = pstdisp;
}

void ScreenTextPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LEFT_ALIGN, iHorzAlign_);
	DDX_Radio(pDX, IDC_RADIO_TOP_ALIGN, iVertAlign_);
	DDX_Text(pDX, IDC_EDIT_X, iLeft_);
	DDX_Text(pDX, IDC_EDIT_Y, iTop_);
	DDX_Text(pDX, IDC_EDIT_SX, iSX_);
	DDX_Text(pDX, IDC_EDIT_SY, iSY_);
	DDX_Radio(pDX, IDC_RADIO_HORZ, iOrientation_);
}


BEGIN_MESSAGE_MAP(ScreenTextPropPage, CBasePage)
	ON_BN_CLICKED(IDC_RADIO_HORZ, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_VERT, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_LEFT_ALIGN, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_RIGHT_ALIGN, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_TO_CENTER, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_TOP_ALIGN, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_BOTTOM_ALIGN, OnRadioBtnClick)
	ON_BN_CLICKED(IDC_RADIO_VERT_TO_CENTER, OnRadioBtnClick)
END_MESSAGE_MAP()


// ScreenTextPropPage message handlers

BOOL ScreenTextPropPage::OnApply()
{
	if (!UpdateData())
		return FALSE;
	wnd2set();
	*screenTextSetCopy_ = *pScreenTextSet_;
	return CBasePage::OnApply();
}

BOOL ScreenTextPropPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	int const MIN_SPIN_VALUE = -13000;
	int const MAX_SPIN_VALUE = 13000;
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_X))->
		SetRange(MIN_SPIN_VALUE, MAX_SPIN_VALUE);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_Y))->
		SetRange(MIN_SPIN_VALUE, MAX_SPIN_VALUE);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPINSX))->
		SetRange(MIN_SPIN_VALUE, MAX_SPIN_VALUE);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPINSY))->
		SetRange(MIN_SPIN_VALUE, MAX_SPIN_VALUE);
	set2wnd();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void ScreenTextPropPage::wnd2set()
{
	setAlignment();
	pScreenTextSet_->set_screen_pos(Vect2i(iLeft_, iTop_));
	pScreenTextSet_->set_screen_size(Vect2i(iSX_, iSY_));
//	pScreenTextSet_->set_orientation(
//		static_cast<qdScreenTextSet::orientation_t>(iOrientation_));
}

void ScreenTextPropPage::set2wnd()
{
	iLeft_ = pScreenTextSet_->screen_pos().x;
	iTop_ = pScreenTextSet_->screen_pos().y;
	iSX_ = pScreenTextSet_->screen_size().x;
	iSY_ = pScreenTextSet_->screen_size().y;
//	iOrientation_ = static_cast<int>(pScreenTextSet_->orientation());
//	iHorzAlign_ = getHorzAlignmentNum(pScreenTextSet_->alignment());
//	iVertAlign_ = getVertAlignmentNum(pScreenTextSet_->alignment());
}

BOOL ScreenTextPropPage::PreTranslateMessage(MSG* pmsg)
{
	if (pmsg->message == WM_KEYDOWN&&pmsg->wParam == VK_RETURN)
	{
		if (UpdateData())
		{
			wnd2set();
			redraw_scene(true);
			SetModified();
		}
		return TRUE;
	}
	return CBasePage::PreTranslateMessage(pmsg);
}

void ScreenTextPropPage::OnRadioBtnClick()
{
	if (!UpdateData())
		return;

	SetModified();
	wnd2set();
	redraw_scene(true);
}

int ScreenTextPropPage::getHorzAlignmentNum(int algnement)
{
//	switch(algnement&
//		(qdScreenTextSet::SET_ALIGN_LEFT
//		|qdScreenTextSet::SET_ALIGN_RIGHT
//		|qdScreenTextSet::SET_ALIGN_CENTER_H)) 
//	{
//	case qdScreenTextSet::SET_ALIGN_LEFT:
//		return 0;
//	case qdScreenTextSet::SET_ALIGN_RIGHT:
//		return 1;
//	case qdScreenTextSet::SET_ALIGN_CENTER_H:
//		return 2;
//	default:
//		assert(0);
//	}
	return -1;
}

int ScreenTextPropPage::getVertAlignmentNum(int algnement)
{
//	switch(algnement&
//		(qdScreenTextSet::SET_ALIGN_TOP
//		|qdScreenTextSet::SET_ALIGN_BOTTOM
//		|qdScreenTextSet::SET_ALIGN_CENTER_V)) 
//	{
//	case qdScreenTextSet::SET_ALIGN_TOP:
//		return 0;
//	case qdScreenTextSet::SET_ALIGN_BOTTOM:
//		return 1;
//	case qdScreenTextSet::SET_ALIGN_CENTER_V:
//		return 2;
//	default:
//		assert(0);
//	}
	return -1;
}

void ScreenTextPropPage::setAlignment()
{
//	int alignment = 0;
//	switch(iHorzAlign_) {
//	case 0:
//		alignment |= qdScreenTextSet::SET_ALIGN_LEFT;
//		break;
//	case 1:
//		alignment |= qdScreenTextSet::SET_ALIGN_RIGHT;
//		break;
//	case 2:
//		alignment |= qdScreenTextSet::SET_ALIGN_CENTER_H;
//		break;
//	}
//
//	switch(iVertAlign_) {
//	case 0:
//		alignment |= qdScreenTextSet::SET_ALIGN_TOP;
//		break;
//	case 1:
//		alignment |= qdScreenTextSet::SET_ALIGN_BOTTOM;
//		break;
//	case 2:
//		alignment |= qdScreenTextSet::SET_ALIGN_CENTER_V;
//		break;
//	}
//	pScreenTextSet_->set_alignment(alignment);
}

void ScreenTextPropPage::OnCancel()
{
	*pScreenTextSet_ = *screenTextSetCopy_;
	CBasePage::OnCancel();
}
