		// WTGreateWTCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "WTGreateWTCond.h"

#include "qd_game_scene.h"
#include "qd_game_object_state.h"
#include "StatesToCombo.h"

#include <boost/function.hpp>

// WTGreateWTCond dialog

IMPLEMENT_DYNAMIC(WTGreateWTCond, CDialog)
WTGreateWTCond::WTGreateWTCond(DWORD id, CWnd* pParent /*=NULL*/)
: CondDlgBase(WTGreateWTCond::IDD, id, pParent)
{

}

WTGreateWTCond::~WTGreateWTCond()
{
}

void WTGreateWTCond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FIRST_STATE, m_wndFirstState);
	DDX_Control(pDX, IDC_COMBO_SECOND_STATE, m_wndSecondState);
	DDX_Control(pDX, IDC_COMBO_FIRST_STATE_SCENE, m_wndFirstStateScene);
	DDX_Control(pDX, IDC_COMBO_SECOND_STATE_SCENE, m_wndSecondStateScene);
}

BEGIN_MESSAGE_MAP(WTGreateWTCond, CDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_FIRST_STATE, OnCbnSelchangeComboFirstState)
	ON_CBN_SELCHANGE(IDC_COMBO_SECOND_STATE, OnCbnSelchangeComboSecondState)
	ON_CBN_SELCHANGE(IDC_COMBO_FIRST_STATE_SCENE, OnCbnSelchangeComboFirstStateScene)
	ON_CBN_SELCHANGE(IDC_COMBO_SECOND_STATE_SCENE, OnCbnSelchangeComboSecondStateScene)
END_MESSAGE_MAP()


// WTGreateWTCond message handlers

BOOL WTGreateWTCond::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	LoadScenes(m_wndFirstStateScene, *GetDlgItem(IDC_STATIC_FIRST_STATE_SCENE));
	LoadScenes(m_wndSecondStateScene, *GetDlgItem(IDC_STATIC_SECOND_STATE_SCENE));
	if (qdGameScene const* const pscene = m_ptrMakeCondDlg->get_scene())
	{
		m_wndFirstStateScene.SelectString(-1, pscene->name());
		m_wndSecondStateScene.SelectString(-1, pscene->name());
	}
	else
	{
		m_wndFirstStateScene.SetCurSel(0);
		m_wndSecondStateScene.SetCurSel(0);
	}
	OnCbnSelchangeComboFirstStateScene();
	OnCbnSelchangeComboSecondStateScene();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void WTGreateWTCond::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);
	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool WTGreateWTCond::IsCondDataComplete()
{
	qdNamedObject const* const p = m_qdCondition.get_object(0);
	qdNamedObject const* const p1 = m_qdCondition.get_object(1);
	return (p&&p1&&p1 != p);
}

void WTGreateWTCond::OnCbnSelchangeComboFirstState()
{
	int const iCurSel = m_wndFirstState.GetCurSel();
	if (iCurSel == CB_ERR)
		m_qdCondition.put_object(0, NULL);
	else
	{
		qdGameObjectState* pstate = 
			reinterpret_cast<qdGameObjectState*>
			(m_wndFirstState.GetItemDataPtr(iCurSel));
		m_qdCondition.put_object(0, pstate);
	}
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void WTGreateWTCond::OnCbnSelchangeComboSecondState()
{
	int const iCurSel = m_wndSecondState.GetCurSel();
	if (iCurSel == CB_ERR)
		m_qdCondition.put_object(1, NULL);
	else
	{
		qdGameObjectState* pstate = 
			reinterpret_cast<qdGameObjectState*>
				(m_wndSecondState.GetItemDataPtr(iCurSel));
		m_qdCondition.put_object(1, pstate);
	}
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void WTGreateWTCond::OnCbnSelchangeComboFirstStateScene()
{
	int const iCurSel = m_wndFirstStateScene.GetCurSel();
	if (iCurSel == CB_ERR)
		return;
	qdGameScene* pscene = 
		reinterpret_cast<qdGameScene*>(m_wndFirstStateScene.GetItemDataPtr(iCurSel));
	m_wndFirstState.ResetContent();
	StatesToCombo::loadFromScene(m_wndFirstState, pscene);
	m_wndFirstState.SetCurSel(0);
	OnCbnSelchangeComboFirstState();
}

void WTGreateWTCond::OnCbnSelchangeComboSecondStateScene()
{
	int const iCurSel = m_wndSecondStateScene.GetCurSel();
	if (iCurSel == CB_ERR)
		return;
	qdGameScene* pscene = 
		reinterpret_cast<qdGameScene*>(m_wndSecondStateScene.GetItemDataPtr(iCurSel));
	m_wndSecondState.ResetContent();
	StatesToCombo::loadFromScene(m_wndSecondState, pscene);
	m_wndSecondState.SetCurSel(0);
	OnCbnSelchangeComboSecondState();
}
