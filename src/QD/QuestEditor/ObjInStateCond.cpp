// ObjInStateCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "ObjInStateCond.h"
#include "MakeCondDlgBase.h"
#include "qd_game_object_animated.h"

// ObjInStateCond dialog
VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);
bool LoadObjects(CCJFlatComboBox& wndCombo, qdGameScene* ptrScene);

ObjInStateCond::ObjInStateCond(DWORD id, CWnd* pParent)
	: CondDlgBase(ObjInStateCond::IDD, id, pParent)
{
}

ObjInStateCond::~ObjInStateCond()
{
}

void ObjInStateCond::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndObjs);
	DDX_Control(pDX, IDC_COMBO_STATES, m_wndStates);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_wndScenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_wndSceneStatic);
}


BEGIN_MESSAGE_MAP(ObjInStateCond, CondDlgBase)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnCbnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_STATES, OnCbnSelchangeComboStates)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
END_MESSAGE_MAP()


// ObjInStateCond message handlers

bool ObjInStateCond::IsCondDataComplete(){
	if(!m_qdCondition.get_object(qdCondition::OBJECT_NAME))
		return false;

	if(!m_qdCondition.get_object(qdCondition::OBJECT_STATE_NAME))
		return false;

	return true;
}

void ObjInStateCond::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);

	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

BOOL ObjInStateCond::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	LoadScenes();

	if(!LoadObjects())
		return FALSE;

	AdjustListWidth(&m_wndScenes);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool ObjInStateCond::LoadScenes(){
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

class qdGameScene* ObjInStateCond::get_scene(){
	if (m_wndScenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>
		(m_wndScenes.GetItemDataPtr(m_wndScenes.GetCurSel()));
}

bool ObjInStateCond::LoadObjects()
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

void ObjInStateCond::OnCbnSelchangeComboObject()
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

void ObjInStateCond::OnCbnSelchangeComboStates()
{
	int iSel = m_wndStates.GetCurSel();
	if(iSel == -1)
		return;
	m_qdCondition.put_object(qdCondition::OBJECT_STATE_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndStates.GetItemDataPtr(iSel)));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjInStateCond::OnCbnSelchangeComboScenes()
{
	qdGameScene * pscene = get_scene();
	//определяем сцену для объектов из списка
	//если она та, которую выбрали только что, то ничего не делаем
	int iCurObj = m_wndObjs.GetCurSel();
	if (iCurObj != CB_ERR)
	{
		qdGameObject* pobj = 
			static_cast<qdGameObject*>(m_wndObjs.GetItemDataPtr(iCurObj));
		ASSERT(pobj);
		if (pobj->owner() == pscene)
			return;
	}
	
	LoadObjects();
}

bool ObjInStateCond::LoadStates(const qdGameObjectAnimated* pa)
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
