// WTCondDlgBase.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QuestEditor.h"
#include "WTCondDlgBase.h"

#include "qd_game_object_state.h"
#include "StatesToCombo.h"

// WTCondDlgBase

IMPLEMENT_DYNAMIC(WTCondDlgBase, CondDlgBase);
WTCondDlgBase::WTCondDlgBase(UINT nIDTemplate, DWORD id,
				UINT uiStatesComboId, UINT uiStatesScenesComboId, UINT uiSceneStaticId)
:CondDlgBase(nIDTemplate, id)
, m_uiStatesComboId(uiStatesComboId)
, m_uiStatesScenesComboId(uiStatesScenesComboId)
, m_uiSceneStaticId(uiSceneStaticId)
{
}

WTCondDlgBase::~WTCondDlgBase()
{
}


BEGIN_MESSAGE_MAP(WTCondDlgBase, CondDlgBase)
END_MESSAGE_MAP()


void WTCondDlgBase::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, m_uiStatesComboId, m_wndStatesCombo);
	DDX_Control(pDX, m_uiStatesScenesComboId, m_wndStatesSceneCombo);
}

BOOL WTCondDlgBase::OnInitDialog()
{
	BOOL res = CondDlgBase::OnInitDialog();

	if (!LoadScenes())
		return FALSE;
	return res;
}

// WTCondDlgBase message handlers

qdGameScene* WTCondDlgBase::getScene(){
	int const iCurSel = m_wndStatesSceneCombo.GetCurSel();
	if (iCurSel == CB_ERR)
		return NULL;
	return reinterpret_cast<qdGameScene*>
		(m_wndStatesSceneCombo.GetItemDataPtr(iCurSel));
}

void WTCondDlgBase::OnCbnSelchangeComboStates()
{
	int const iCurSel = m_wndStatesCombo.GetCurSel();
	if (iCurSel == CB_ERR)
		m_qdCondition.put_object(0, NULL);
	else
	{
		qdGameObjectState* const pstate = reinterpret_cast<qdGameObjectState*>
			(m_wndStatesCombo.GetItemDataPtr(iCurSel));
		m_qdCondition.put_object(0, pstate);
	}
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void WTCondDlgBase::OnCbnSelchangeComboStateScene()
{
	if (qdGameScene const* const pscene = getScene())
	{
		m_wndStatesCombo.ResetContent();
		StatesToCombo::loadFromScene(m_wndStatesCombo, pscene);
		m_wndStatesCombo.SetCurSel(0);
		OnCbnSelchangeComboStates();
	}
}

bool WTCondDlgBase::LoadScenes()
{
	bool const res = CondDlgBase::LoadScenes(m_wndStatesSceneCombo, 
							*GetDlgItem(m_uiSceneStaticId), SH_NONE);
	m_wndStatesSceneCombo.SetCurSel(0);
	if (qdGameScene const *const pscene = m_ptrMakeCondDlg->get_scene())
		m_wndStatesSceneCombo.SelectString(-1, pscene->name());
	OnCbnSelchangeComboStateScene();
	return res;
}

LRESULT WTCondDlgBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND)
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			if (LOWORD(wParam) == m_uiStatesComboId)
				OnCbnSelchangeComboStates();
			else if (LOWORD(wParam) == m_uiStatesScenesComboId)
				OnCbnSelchangeComboStateScene();
		}
	}
	return __super::WindowProc(message, wParam, lParam);
}
