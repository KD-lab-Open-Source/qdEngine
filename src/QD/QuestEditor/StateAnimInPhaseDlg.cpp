// StateAnimInPhaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "StateAnimInPhaseDlg.h"
#include "MakeCondDlgBase.h"
#include "qd_game_object_animated.h"
#include "qd_game_object_state.h"


// StateAnimInPhaseDlg dialog

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);
bool LoadObjects(CCJFlatComboBox& wndCombo, qdGameScene* ptrScene);

int const MAX_PHASE = 100;

StateAnimInPhaseDlg::StateAnimInPhaseDlg(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(StateAnimInPhaseDlg::IDD, id, pParent)
	, m_iFrom(0)
	, m_iTo(MAX_PHASE)
{
	m_qdCondition.put_value(qdCondition::ANIMATION_PHASE, 0.f, 0);
	m_qdCondition.put_value(qdCondition::ANIMATION_PHASE, 1.f, 1);
}

StateAnimInPhaseDlg::~StateAnimInPhaseDlg()
{
}

void StateAnimInPhaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BEG_PHASE, m_iFrom);
	DDX_Text(pDX, IDC_EDIT_END_PHASE, m_iTo);
	DDX_Control(pDX, IDC_COMBO_STATES, m_wndStates);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndObjs);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_wndScenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_wndSceneStatic);
	DDV_MinMaxInt(pDX, m_iFrom, 0, m_iTo-1);
	DDV_MinMaxInt(pDX, m_iTo, m_iFrom+1, MAX_PHASE);
}


BEGIN_MESSAGE_MAP(StateAnimInPhaseDlg, CondDlgBase)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_STATES, OnCbnSelchangeComboStates)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnCbnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEG_PHASE, OnDeltaposSpinBegPhase)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_PHASE, OnDeltaposSpinEndPhase)
END_MESSAGE_MAP()


// StateAnimInPhaseDlg message handlers
BOOL StateAnimInPhaseDlg::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_BEG_PHASE))->SetRange(0,MAX_PHASE);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_END_PHASE))->SetRange(0,MAX_PHASE);
	CondToDlg();
	LoadScenes();
	LoadObjects();
	return res;
}

void StateAnimInPhaseDlg::CondToDlg()
{
	
}

bool StateAnimInPhaseDlg::IsCondDataComplete()
{
	if(!m_qdCondition.get_object(qdCondition::OBJECT_NAME))
		return false;

	if(!m_qdCondition.get_object(qdCondition::OBJECT_STATE_NAME))
		return false;
	float from = 0.f;
	m_qdCondition.get_value(qdCondition::ANIMATION_PHASE, from, 0);
	float to = 1.f;
	m_qdCondition.get_value(qdCondition::ANIMATION_PHASE, to, 1);
	return (from <= to);
}

void StateAnimInPhaseDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);

	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void StateAnimInPhaseDlg::OnCbnSelchangeComboStates()
{
	int iSel = m_wndStates.GetCurSel();
	if(iSel == -1)
		return;
	m_qdCondition.put_object(qdCondition::OBJECT_STATE_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndStates.GetItemDataPtr(iSel)));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void StateAnimInPhaseDlg::OnCbnSelchangeComboObject()
{
	int iSel = m_wndObjs.GetCurSel();
	if(iSel == -1)
		return;
	DWORD_PTR dwData = m_wndObjs.GetItemData(iSel);
	if(dwData)
	{
		qdNamedObject* const p = reinterpret_cast<qdNamedObject*>(dwData);
		m_qdCondition.put_object(qdCondition::OBJECT_NAME, p);

		LoadStates(static_cast<qdGameObjectAnimated*>(p));
	}
	else
		m_qdCondition.put_object(qdCondition::OBJECT_NAME, NULL);

	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void StateAnimInPhaseDlg::OnCbnSelchangeComboScenes()
{
	qdGameScene * pscene = get_scene();
	//определяем сцену для объектов из списка
	//если она та, которую выбрали только что, то ничего не делаем
	int iCurObj = m_wndObjs.GetCurSel();
	if (iCurObj != CB_ERR)
	{
		qdGameObject* const pobj = 
			static_cast<qdGameObject*>(m_wndObjs.GetItemDataPtr(iCurObj));
		ASSERT(pobj);
		if (pobj->owner() == pscene)
			return;
	}

	LoadObjects();
}

bool StateAnimInPhaseDlg::LoadScenes(){
	qdGameDispatcher* ptrGD = qd_get_game_dispatcher();
	ASSERT(ptrGD);
	const qdGameSceneList& gsl = ptrGD->scene_list();
	if(gsl.empty())
	{
		m_wndScenes.SetCurSel(-1);
		m_wndScenes.EnableWindow(FALSE);
		return false;
	}

	qdGameSceneList::const_iterator itr = gsl.begin(), end = gsl.end();
	for(;itr != end; ++itr){
		int ires = m_wndScenes.AddString((*itr)->name());
		if (ires != CB_ERR)
			m_wndScenes.SetItemDataPtr(ires, *itr);
	}
	if (qdGameScene* pscene = m_ptrMakeCondDlg->get_scene())
		m_wndScenes.SelectString(-1, pscene->name());
	else{
		m_wndSceneStatic.ShowWindow(SW_SHOW);
		m_wndScenes.ShowWindow(SW_SHOW);
		m_wndScenes.SetCurSel(0);
	}
	return true;
}

class qdGameScene* StateAnimInPhaseDlg::get_scene(){
	if (m_wndScenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>
		(m_wndScenes.GetItemDataPtr(m_wndScenes.GetCurSel()));
}

bool StateAnimInPhaseDlg::LoadObjects()
{
	qdGameScene* ptrScene = get_scene();
	if(!ptrScene)
		return false;
	if(::LoadObjects(m_wndObjs, ptrScene))
	{
		m_wndObjs.SetCurSel(0);
		OnCbnSelchangeComboObject();
	}
	AdjustListWidth(&m_wndObjs);
	return true;
}

void StateAnimInPhaseDlg::OnDeltaposSpinBegPhase(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}

	*pResult = 0;

	m_iFrom += pNMUpDown->iDelta;
	if (m_iFrom >= m_iTo)
	{
		m_iFrom = m_iTo-1;
		*pResult = 1;
	}
	if (m_iFrom < 0)
		m_iFrom = 0;

	m_qdCondition.put_value(qdCondition::ANIMATION_PHASE, 
			static_cast<float>(m_iFrom)/MAX_PHASE, 0);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	UpdateData(FALSE);

}

void StateAnimInPhaseDlg::OnDeltaposSpinEndPhase(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if(!UpdateData()){
		*pResult = 1;
		return;
	}

	*pResult = 0;

	m_iTo += pNMUpDown->iDelta;
	if (m_iTo <= m_iFrom)
	{
		m_iTo = m_iFrom+1;
		*pResult = 1;
	}
	if (m_iTo > MAX_PHASE)
		m_iTo = MAX_PHASE;

	m_qdCondition.put_value(qdCondition::ANIMATION_PHASE, 
		static_cast<float>(m_iTo)/MAX_PHASE, 1);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
	UpdateData(FALSE);
}

bool StateAnimInPhaseDlg::LoadStates(qdGameObjectAnimated const* pa)
{
	m_wndStates.ResetContent();
	typedef qdGameObjectStateVector::const_iterator ci_t;

	const qdGameObjectStateVector& v = pa->state_vector();
	ci_t i = v.begin(), e = v.end();
	for(; i != e; ++i){
		qdGameObjectState* pstate = *i;
		int iItem = m_wndStates.AddString(pstate->name());
		if (iItem != CB_ERR)
			m_wndStates.SetItemDataPtr(iItem, pstate);
	}
	m_wndStates.SetCurSel(0);
	OnCbnSelchangeComboStates();
	AdjustListWidth(&m_wndStates);
	return true;
}

BOOL StateAnimInPhaseDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		CWnd* const pFocus = GetFocus();
		if (pFocus == GetDlgItem(IDC_EDIT_BEG_PHASE)
			||pFocus == GetDlgItem(IDC_EDIT_END_PHASE))
		{
			if (UpdateData())
			{
				m_qdCondition.put_value(qdCondition::ANIMATION_PHASE, 
					static_cast<float>(m_iFrom)/MAX_PHASE, 0);
				m_qdCondition.put_value(qdCondition::ANIMATION_PHASE, 
					static_cast<float>(m_iTo)/MAX_PHASE, 1);
			}
			m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
			return TRUE;
		}
	}
	return CondDlgBase::PreTranslateMessage(pMsg);
}
