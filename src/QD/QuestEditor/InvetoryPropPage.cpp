// InvetoryPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "InvetoryPropPage.h"
#include "qd_inventory.h"
#include "qd_game_object_mouse.h"
#include <algorithm>
#include ".\invetoryproppage.h"

#include <boost/rtl.hpp>

const int QD_ADDITIONAL_CELLS_MIN   = 0;
const int QD_ADDITIONAL_CELLS_MAX   = 255;

// InvetoryPropPage dialog

IMPLEMENT_DYNAMIC(InventoryPropPage, CBasePage)
InventoryPropPage::InventoryPropPage()
	: CBasePage(InventoryPropPage::IDD)
	, m_iCursorPolicy(0)
	, m_ptrGameDisp(NULL)
	, m_pInv(NULL)
	, m_bTakeToMouse(FALSE)
	, m_bDontOpenAfterTake(FALSE)
	, m_bPutObjectAfterClick(FALSE)
	, m_bPutToInventoryByRClick(FALSE)
	, visibleWhenInactive_(FALSE)
	, transparency_(-1)
	, m_bInitDialog(true)
	, additionalXCells_(0)
	, additionalYCells_(0)
{
}

InventoryPropPage::~InventoryPropPage()
{
}

void InventoryPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_TAKE_TO_MOUSE, m_bTakeToMouse);
	DDX_Check(pDX, IDC_CHECK_PUT_OBJECT_AFTER_CLICK, m_bPutObjectAfterClick);
	DDX_Check(pDX, IDC_CHECK_PUT_TO_INVERNTORY_BY_RCLICK, m_bPutToInventoryByRClick);
	DDX_Check(pDX, IDC_CHECK_VISIBLE_WHEN_INACTIVE, visibleWhenInactive_);
	DDX_Control(pDX, IDC_BUTTON_COLOR, shadowColor_);

	DDX_Text(pDX, IDC_EDIT_TRANSPARENCY, transparency_);
	DDX_Text(pDX, IDC_EDIT_ADDIT_X, additionalXCells_);
	DDX_Text(pDX, IDC_EDIT_ADDIT_Y, additionalYCells_);
}


BEGIN_MESSAGE_MAP(InventoryPropPage, CBasePage)
	ON_BN_CLICKED(IDC_CHECK_TAKE_TO_MOUSE, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_DONT_OPEN_AFTER_TAKE, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_PUT_OBJECT_AFTER_CLICK, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_PUT_TO_INVERNTORY_BY_RCLICK, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_VISIBLE_WHEN_INACTIVE, OnBnClickedCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TRANSPARENCY, OnDeltaposSpinTransparency)
	ON_MESSAGE(CPN_SELCHANGE, OnColorPicSelChange)
	ON_EN_CHANGE(IDC_EDIT_TRANSPARENCY, OnEnChangeEditTransparency)
	ON_EN_CHANGE(IDC_EDIT_ADDIT_X, OnEnChangeEditAdditX)
	ON_EN_CHANGE(IDC_EDIT_ADDIT_Y, OnEnChangeEditAdditY)
END_MESSAGE_MAP()


// InvetoryPropPage message handlers

BOOL InventoryPropPage::OnApply()
{
	if(!UpdateData())
		return FALSE;
	wnd2inv();
	return CBasePage::OnApply();
}

BOOL InventoryPropPage::OnInitDialog()
{
	m_bInitDialog = true;
	CBasePage::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_TRANSPARENCY))->
		SetRange(QD_NO_SHADOW_ALPHA, QD_SHADOW_ALPHA_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_ADDIT_X))->
		SetRange(QD_ADDITIONAL_CELLS_MIN, QD_ADDITIONAL_CELLS_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_ADDIT_Y))->
		SetRange(QD_ADDITIONAL_CELLS_MIN, QD_ADDITIONAL_CELLS_MAX);

	inv2wnd();
	shadowColor_.EnableWindow(transparency_ != QD_NO_SHADOW_ALPHA);
	UpdateData(FALSE);
	m_bInitDialog = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

//void InventoryPropPage::set_data(qdGameDispatcher* ptrDisp, qdInventory* pInv)
void InventoryPropPage::set_data(qdGameDispatcher* ptrDisp, qdInventory* ptrSource){
	m_ptrGameDisp = ptrDisp;
	m_pInv = ptrSource;
}

void InventoryPropPage::inv2wnd()
{
	if (m_pInv)
	{
		qdInventory* const pi = m_pInv;
		m_bTakeToMouse = pi->check_flag(qdInventory::INV_TAKE_TO_MOUSE);
		m_bDontOpenAfterTake = pi->check_flag(qdInventory::INV_DONT_OPEN_AFTER_TAKE );
		m_bPutObjectAfterClick = pi->check_flag(qdInventory::INV_PUT_OBJECT_AFTER_CLICK);
		m_bPutToInventoryByRClick = pi->check_flag(qdInventory::INV_ENABLE_RIGHT_CLICK);
		visibleWhenInactive_ = pi->check_flag(qdInventory::INV_VISIBLE_WHEN_INACTIVE);
		transparency_ = pi->shadow_alpha();
		shadowColor_.SetColor(pi->shadow_color());
		additionalXCells_ = pi->additional_cells().x;
		additionalYCells_ = pi->additional_cells().y;
	}
	else
	{
		const qdInventoryList & l = m_ptrGameDisp->inventory_list();
		if (l.empty())
		{
			GetDlgItem(IDC_CHECK_TAKE_TO_MOUSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_DONT_OPEN_AFTER_TAKE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_PUT_OBJECT_AFTER_CLICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_PUT_TO_INVERNTORY_BY_RCLICK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_VISIBLE_WHEN_INACTIVE)->EnableWindow(FALSE);
			return;
		}
		qdInventory* const pi = l.front();
		m_bTakeToMouse = (pi->check_flag(qdInventory::INV_TAKE_TO_MOUSE));
		m_bDontOpenAfterTake = (pi->check_flag(qdInventory::INV_DONT_OPEN_AFTER_TAKE ));
		m_bPutObjectAfterClick = pi->check_flag(qdInventory::INV_PUT_OBJECT_AFTER_CLICK);
		m_bPutToInventoryByRClick = pi->check_flag(qdInventory::INV_ENABLE_RIGHT_CLICK);
		visibleWhenInactive_ = pi->check_flag(qdInventory::INV_VISIBLE_WHEN_INACTIVE);

		transparency_ = pi->shadow_alpha();
		shadowColor_.SetColor(pi->shadow_color());
		additionalXCells_ = pi->additional_cells().x;
		additionalYCells_ = pi->additional_cells().y;
	}
}

void InventoryPropPage::wnd2inv()
{
	DWORD dwRemoveFlag = 0;
	DWORD dwAddFlag = 0;
	if (m_bTakeToMouse) 
		dwAddFlag = qdInventory::INV_TAKE_TO_MOUSE;
	else
		dwRemoveFlag = qdInventory::INV_TAKE_TO_MOUSE;

	if (m_bDontOpenAfterTake) 
		dwAddFlag		|= qdInventory::INV_DONT_OPEN_AFTER_TAKE;
	else
		dwRemoveFlag	|= qdInventory::INV_DONT_OPEN_AFTER_TAKE;

	if (m_bPutObjectAfterClick) 
		dwAddFlag		|= qdInventory::INV_PUT_OBJECT_AFTER_CLICK;
	else
		dwRemoveFlag	|= qdInventory::INV_PUT_OBJECT_AFTER_CLICK;

	if (m_bPutToInventoryByRClick) 
		dwAddFlag		|= qdInventory::INV_ENABLE_RIGHT_CLICK;
	else
		dwRemoveFlag	|= qdInventory::INV_ENABLE_RIGHT_CLICK;

	if (visibleWhenInactive_)
		dwAddFlag		|= qdInventory::INV_VISIBLE_WHEN_INACTIVE;
	else
		dwRemoveFlag	|= qdInventory::INV_VISIBLE_WHEN_INACTIVE;
	if (m_pInv)
	{
		m_pInv->drop_flag(dwRemoveFlag);
		m_pInv->set_flag(dwAddFlag);

		m_pInv->set_shadow_alpha(transparency_);
		m_pInv->set_shadow_color(shadowColor_.GetColor());
		m_pInv->set_additional_cells(Vect2s(additionalXCells_, additionalYCells_));
	}
	else
	{
		const qdInventoryList & l = m_ptrGameDisp->inventory_list();
	
		std::for_each(l.begin(),l.end(), 
			std::bind2nd(std::mem_fun(&qdNamedObject::drop_flag), dwRemoveFlag));
		std::for_each(l.begin(),l.end(), 
			std::bind2nd(std::mem_fun(&qdNamedObject::set_flag), dwAddFlag));
		boost::rtl::crange<qdInventoryList const> r(l);
		for(; r; ++r)
		{
			qdInventory* p = *r;
			p->set_shadow_alpha(transparency_);
			p->set_shadow_color(shadowColor_.GetColor());
			p->set_additional_cells(Vect2s(additionalXCells_, additionalYCells_));
		}
	}
}

void InventoryPropPage::OnBnClickedCheck()
{
	if (m_bInitDialog) 
		return;

	SetModified();
}

void InventoryPropPage::OnDeltaposSpinTransparency(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_bInitDialog) 
		return;

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

LRESULT InventoryPropPage::OnColorPicSelChange(WPARAM, LPARAM)
{
	if (m_bInitDialog) 
		return 0;

	SetModified();
	return 0;
}

void InventoryPropPage::OnEnChangeEditTransparency()
{
	if (m_bInitDialog) 
		return;

	UpdateData();
	shadowColor_.EnableWindow(transparency_ != QD_NO_SHADOW_ALPHA);
	SetModified();
}

void InventoryPropPage::OnEnChangeEditAdditX()
{
	if (m_bInitDialog) 
		return;

	SetModified();
}

void InventoryPropPage::OnEnChangeEditAdditY()
{
	if (m_bInitDialog) 
		return;

	SetModified();
}