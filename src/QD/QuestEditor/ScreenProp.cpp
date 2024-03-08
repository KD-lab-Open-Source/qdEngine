// ScreenProp.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include <xutil.h>

#include "qd_camera.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"
#include "qd_minigame.h"
#include "qd_interface_screen.h"

#include "ScreenProp.h"
#include ".\screenprop.h"

#include <boost/rtl.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScreenProp dialog

ScreenProp::ScreenProp(CWnd* pParent /*=NULL*/)
: CBasePage(ScreenProp::IDD)
, m_iSaveId(0)
, m_bCycleX(FALSE)
, m_bCycleY(FALSE)
, resetTriggersOnLoading_(FALSE)
, enablePersonageSwitch_(TRUE)
, disableMainMenuExit_(FALSE)
{
	//{{AFX_DATA_INIT(ScreenProp)
	m_iCellX = 0;
	m_iCellY = 0;
	//}}AFX_DATA_INIT
	m_ptrGameDisp = NULL;
	m_bRedraw = false;
}

bool ScreenProp::set_data(qdGameDispatcher* ptrGD, qdGameScene* ptrGS)
{
	m_ptrGameDisp = ptrGD;
	ASSERT(m_ptrGameDisp);

	m_ptrScene = ptrGS;
	if(!m_ptrScene)
		m_ptrScene = m_ptrGameDisp->get_active_scene();

	m_bRedraw = (m_ptrScene == m_ptrGameDisp->get_active_scene());

	return bool(m_ptrScene);
}

void ScreenProp::DoDataExchange(CDataExchange* pDX)
{
	//Ограничить m_iVisibleSizeX, m_iVisibleSizeY размерами 
	//экрана

	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScreenProp)
	DDX_Text(pDX, IDC_EDIT_CELLSX, m_iCellX);
	DDX_Text(pDX, IDC_EDIT_CELLSY, m_iCellY);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_SAVE_ID, m_iSaveId);
	DDV_MinMaxInt(pDX, m_iSaveId, -1, INT_MAX);
	DDX_Check(pDX, IDC_CHECK_CYCLEX, m_bCycleX);
	DDX_Check(pDX, IDC_CHECK_CYCLEY, m_bCycleY);
	DDX_Check(pDX, IDC_CHECK_RESET_TRIGGERS_ON_LOAD, resetTriggersOnLoading_);
	DDX_Check(pDX, IDC_CHECK_PERSONAGE_SWITCH, enablePersonageSwitch_);
	DDX_Check(pDX, IDC_CHECK_MANIMENU_EXIT, disableMainMenuExit_);
	DDX_Control(pDX, IDC_COMBO_MINIGAMES, miniGames_);
	DDX_Control(pDX, IDC_COMBO_INTERFACE_SCREENS, interfaceScreens_);
}


BEGIN_MESSAGE_MAP(ScreenProp, CBasePage)
//{{AFX_MSG_MAP(ScreenProp)
ON_EN_CHANGE(IDC_EDIT_CELLSX, OnChangeScreenSize)
ON_EN_CHANGE(IDC_EDIT_CELLSY, OnChangeScreenSize)
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SAVE_ID, OnDeltaposSpinSaveId)
	ON_BN_CLICKED(IDC_CHECK_CYCLEX, OnBnClickedCheckCycle)
	ON_BN_CLICKED(IDC_CHECK_CYCLEY, OnBnClickedCheckCycle)
	ON_BN_CLICKED(IDC_CHECK_RESET_TRIGGERS_ON_LOAD, OnBnClickedCheckCycle)
	ON_BN_CLICKED(IDC_CHECK_PERSONAGE_SWITCH, OnBnClickedCheckCycle)
	ON_CBN_SELCHANGE(IDC_COMBO_MINIGAMES, OnCbnSelchangeComboMinigames)
	ON_CBN_SELCHANGE(IDC_COMBO_INTERFACE_SCREENS, OnCbnSelchangeComboInterfaceScreens)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScreenProp message handlers

void ScreenProp::OnChangeScreenSize() 
{
	SetModified(TRUE);
}


BOOL ScreenProp::OnApply() 
{
	if(!UpdateData())
		return FALSE;
	apply_params();
	redraw();
	return CBasePage::OnApply();
}

void ScreenProp::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CBasePage::OnCancel();
}

void ScreenProp::OnOK() 
{
//	if(!UpdateData())
//		return;
	
//	apply_params();
//	redraw();
	
	CBasePage::OnOK();
}

BOOL ScreenProp::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	loadMinigames();
	loadInterfaceScreens();
	
	qdCamera* pcam = m_ptrScene->get_camera();
	m_iCellX = pcam->get_scr_sx();
	m_iCellY = pcam->get_scr_sy();
	m_iSaveId = m_ptrScene->autosave_slot();

	m_bCycleX = m_ptrScene->check_flag(qdGameScene::CYCLE_X);
	m_bCycleY = m_ptrScene->check_flag(qdGameScene::CYCLE_Y);
	resetTriggersOnLoading_ = 
		m_ptrScene->check_flag(qdGameScene::RESET_TRIGGERS_ON_LOAD);
	enablePersonageSwitch_ = !m_ptrScene->check_flag(qdGameScene::DISABLE_KEYBOARD_PERSONAGE_SWITCH);
	disableMainMenuExit_ = m_ptrScene->check_flag(qdGameScene::DISABLE_MAIN_MENU);
	if(m_ptrScene->has_minigame())
		miniGames_.SelectString(-1, m_ptrScene->minigame_name());
	if(m_ptrScene->has_interface_screen())
		interfaceScreens_.SelectString(-1, m_ptrScene->interface_screen_name());

	UpdateData(FALSE);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN6))->SetRange32(0, INT_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN7))->SetRange32(0, INT_MAX);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SAVE_ID))->SetRange32(-1, INT_MAX);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ScreenProp::set_flag(bool bSetFlag, int iValue)
{
	if (bSetFlag)
		m_ptrScene->set_flag(iValue);
	else
		m_ptrScene->drop_flag(iValue);
}

bool ScreenProp::apply_screen_params()
{
	qdCamera* pcam = m_ptrScene->get_camera();

	pcam->set_scr_size(m_iCellX, m_iCellY);
//	m_bCycleX = m_ptrScene->check_flag(qdGameScene::CYCLE_X);
//	m_bCycleY = m_ptrScene->check_flag(qdGameScene::CYCLE_Y);
	set_flag(m_bCycleX, qdGameScene::CYCLE_X);
	set_flag(m_bCycleY, qdGameScene::CYCLE_Y);
	set_flag(resetTriggersOnLoading_, qdGameScene::RESET_TRIGGERS_ON_LOAD);
	set_flag(!enablePersonageSwitch_, qdGameScene::DISABLE_KEYBOARD_PERSONAGE_SWITCH);
	set_flag(disableMainMenuExit_, qdGameScene::DISABLE_MAIN_MENU);

	AdjustBitmapSize();
	return true;
}

bool ScreenProp::apply_va_params()
{
	return true;
}

bool ScreenProp::apply_params()
{
	m_ptrScene->set_autosave_slot(m_iSaveId);
	m_ptrScene->set_minigame_name(getMinigameName());
	m_ptrScene->set_interface_screen_name(getInterfaceScreenName());
	return(apply_screen_params()
		&&apply_va_params());
}

void ScreenProp::redraw()
{
	if(m_bRedraw)
		redraw_scene(true);
}

BOOL ScreenProp::PreTranslateMessage(MSG* pmsg)
{
	if (pmsg->message == WM_KEYDOWN&&pmsg->wParam == VK_RETURN)
	{
		if (::GetDlgItem(m_hWnd, IDC_EDIT_SAVE_ID) == pmsg->hwnd
		||::GetDlgItem(m_hWnd, IDC_EDIT_CELLSX) == pmsg->hwnd
		||::GetDlgItem(m_hWnd, IDC_EDIT_CELLSY) == pmsg->hwnd)
		{
			apply_params();
			redraw();

			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pmsg);
}

void ScreenProp::OnDeltaposSpinSaveId(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	*pResult = 0;
}

void ScreenProp::OnBnClickedCheckCycle()
{
	SetModified();
}

CString const ScreenProp::getMinigameName() const{
	CString res;
	int curSel = miniGames_.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		return res;
	miniGames_.GetLBText(curSel, res);
	return res;
}

CString const ScreenProp::getInterfaceScreenName() const{
	CString res;
	int curSel = interfaceScreens_.GetCurSel();
	if (curSel == CB_ERR||curSel == 0)
		return res;
	interfaceScreens_.GetLBText(curSel, res);
	return res;
}

void ScreenProp::OnCbnSelchangeComboMinigames()
{
	SetModified();
}

void ScreenProp::OnCbnSelchangeComboInterfaceScreens()
{
	SetModified();
}

void ScreenProp::loadMinigames()
{
	boost::rtl::crange<qdMiniGameList const> r(m_ptrGameDisp->minigame_list());
	for(; r; ++r)
		miniGames_.AddString((*r)->name());
	CString str;
	str.LoadString(IDS_NOTHING);
	miniGames_.InsertString(0, str);
	miniGames_.SetCurSel(0);
}

void ScreenProp::loadInterfaceScreens()
{
	boost::rtl::crange<qdInterfaceDispatcher::screen_list_t const> r(qdInterfaceDispatcher::get_dispatcher()->screen_list());
	for(; r; ++r)
		interfaceScreens_.AddString((*r)->name());
	CString str;
	str.LoadString(IDS_NOTHING);
	interfaceScreens_.InsertString(0, str);
	interfaceScreens_.SetCurSel(0);
}


