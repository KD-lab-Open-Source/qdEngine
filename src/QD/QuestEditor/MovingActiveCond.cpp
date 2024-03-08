// MovingActiveCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "MovingActiveCond.h"
#include "qd_game_object_moving.h"
#include "MakeCondDlgBase.h"
#include "ptree_hlprs.hpp"


VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);
// MovingActiveCond dialog

MovingActiveCond::MovingActiveCond(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(MovingActiveCond::IDD, id, pParent)
{
}

MovingActiveCond::~MovingActiveCond()
{
}

void MovingActiveCond::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CHARACTER, m_wndPers);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_scenes_static);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_scenes);
}


BEGIN_MESSAGE_MAP(MovingActiveCond, CondDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_CHARACTER, OnCbnSelchangeComboCharacter)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// MovingActiveCond message handlers

BOOL MovingActiveCond::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	LoadScenes();
	LoadPers();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void MovingActiveCond::OnCbnSelchangeComboCharacter()
{
	const int cur_sel = m_wndPers.GetCurSel();
	CString str;
	if (cur_sel != CB_ERR) 
		m_wndPers.GetLBText(cur_sel, str);

	m_qdCondition.put_value(qdCondition::PERSONAGE_NAME, str);

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void MovingActiveCond::OnCbnSelchangeComboScenes()
{
	LoadPers();
	m_wndPers.SetCurSel(0);
	OnCbnSelchangeComboCharacter();
}

bool MovingActiveCond::IsCondDataComplete()
{
	LPCTSTR pszData = NULL;
	m_qdCondition.get_value(qdCondition::PERSONAGE_NAME, pszData);
	if (!pszData||!*pszData) 
		return false;
	return true;
}

void MovingActiveCond::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);

	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool MovingActiveCond::LoadPers()
{
//	qdGameScene * ptrScene = m_ptrMakeCondDlg->get_scene();
	qdGameScene* ptrScene = get_scene();

	if(!ptrScene)
		return false;

	m_wndPers.ResetContent();
	const qdGameObjectList& gol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	while((_itr = std::find_if(_itr, _end, ptree_helpers::is_obj_moving)) != _end)
	{
		int iPos = m_wndPers.AddString((*_itr)->name());
		if (iPos != -1) {
			m_wndPers.SetItemDataPtr(iPos, *_itr);
		}
		++_itr;
	}
	CString str;
	str.LoadString(IDS_NOTHING);
	m_wndPers.InsertString(0, str);
	if (m_wndPers.GetCount())
		m_wndPers.SetCurSel(0);
	AdjustListWidth(&m_wndPers);
	return true;
}

bool MovingActiveCond::LoadScenes(){
	qdGameDispatcher* ptrGD = qd_get_game_dispatcher();
	ASSERT(ptrGD);
	const qdGameSceneList& gsl = ptrGD->scene_list();
	if(gsl.empty())
	{
		m_scenes.SetCurSel(-1);
		m_scenes.EnableWindow(FALSE);
		return false;
	}

	qdGameSceneList::const_iterator itr = gsl.begin(), end = gsl.end();
	for(;itr != end; ++itr){
		int ires = m_scenes.AddString((*itr)->name());
		if (ires != CB_ERR)
			m_scenes.SetItemDataPtr(ires, *itr);
	}
	m_scenes_static.ShowWindow(SW_SHOW);
	m_scenes.ShowWindow(SW_SHOW);
	qdGameScene * ptrScene = m_ptrMakeCondDlg->get_scene();
	if (!ptrScene||(m_scenes.SelectString(-1, ptrScene->name()) == CB_ERR))
		m_scenes.SetCurSel(0);
	m_scenes.SetDroppedWidth(DROPPED_WIDTH_AUTO_FIT);
	return true;
}

class qdGameScene* MovingActiveCond::get_scene(){
	if (m_scenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes.GetItemDataPtr(m_scenes.GetCurSel()));
}
