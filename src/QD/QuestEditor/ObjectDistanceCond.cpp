// ObjectDistanceCond.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "ObjectDistanceCond.h"
#include "MakeCondDlgBase.h"
#include "condition.h"
#include "qd_game_object_animated.h"

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

namespace{
	struct not_static {
		bool operator()(const qdGameObject* ptrO)const{
			return (ptrO->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ);
		}
	};

bool LoadObjects(CCJFlatComboBox& wndCombo, qdGameScene* ptrScene, qdGameObject* pobj = NULL)
{
	wndCombo.ResetContent();
	const qdGameObjectList& gol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	while((_itr = std::find_if(_itr, _end, not_static())) != _end)
	{
		if (*_itr != pobj) 
		{
			int iPos = wndCombo.AddString((*_itr)->name());
			if(iPos != -1)
				wndCombo.SetItemDataPtr(iPos, *_itr);
		}
		++_itr;
	}
	return true;
}
}
// ObjectDistanceCond dialog

ObjectDistanceCond::ObjectDistanceCond(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(ObjectDistanceCond::IDD, id, pParent)
	, m_fDist(0.f)
{
}

ObjectDistanceCond::~ObjectDistanceCond()
{
}

void ObjectDistanceCond::DoDataExchange(CDataExchange* pDX)
{
	CondDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndObjs1);
	DDX_Control(pDX, IDC_COMBO_OBJECT2, m_wndObjs2);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_scenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_scene_static);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_fDist);
}


BEGIN_MESSAGE_MAP(ObjectDistanceCond, CondDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnCbnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DISTANCE, OnDeltaposSpinDistance)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT2, OnCbnSelchangeComboObject2)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// ObjectDistanceCond message handlers

void ObjectDistanceCond::OnCbnSelchangeComboObject()
{
	const int cur_sel = m_wndObjs2.GetCurSel();
	CString strText;
	if (cur_sel != CB_ERR) 
		m_wndObjs2.GetLBText(cur_sel, strText);
	::LoadObjects(m_wndObjs2, get_scene(), get_object(m_wndObjs1));
	
	if (m_wndObjs2.SelectString(-1, strText) == CB_ERR)
		m_wndObjs2.SetCurSel(0);

	qdGameObject* po = get_object(m_wndObjs1);
	m_qdCondition.put_object(qdCondition::OBJECT_NAME, po);

	OnCbnSelchangeComboObject2();
}

void ObjectDistanceCond::OnCbnSelchangeComboObject2()
{
	qdGameObject* const po = get_object(m_wndObjs2);
	m_qdCondition.put_object(qdCondition::OBJECT2_NAME, po);
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjectDistanceCond::OnCbnSelchangeComboScenes()
{
	LoadObjects();
}

void ObjectDistanceCond::OnDeltaposSpinDistance(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (!UpdateData())
		return;
	m_fDist += pNMUpDown->iDelta/10.f;
	if (m_fDist < 0.f)
		m_fDist = 0.f;
	m_qdCondition.put_value(qdCondition::OBJECTS_DISTANCE, m_fDist);
	UpdateData(FALSE);

	*pResult = 0;
}

BOOL ObjectDistanceCond::OnInitDialog()
{
	CondDlgBase::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_DISTANCE))->SetRange(0, 100);
	LoadScenes();
	
	LoadObjects();
	AdjustListWidth(&m_wndObjs1);
	AdjustListWidth(&m_wndObjs2);
	AdjustListWidth(&m_scenes);
	return TRUE;
}

void ObjectDistanceCond::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CondDlgBase::OnShowWindow(bShow, nStatus);

	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool ObjectDistanceCond::LoadScenes(){
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
	if (qdGameScene* pscene = m_ptrMakeCondDlg->get_scene())
		m_scenes.SelectString(-1, pscene->name());
	else{
		m_scene_static.ShowWindow(SW_SHOW);
		m_scenes.ShowWindow(SW_SHOW);
		m_scenes.SetCurSel(0);
	}
	return true;
}

bool ObjectDistanceCond::LoadObjects()
{
	qdGameScene* ptrScene = get_scene();
	if(!ptrScene)
		return false;
	bool res = ::LoadObjects(m_wndObjs1, ptrScene);
	m_wndObjs1.SetCurSel(0);
	OnCbnSelchangeComboObject();
	return res;
}

class qdGameScene* ObjectDistanceCond::get_scene(){
	if (m_scenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes.GetItemDataPtr(m_scenes.GetCurSel()));
}

qdGameObject* ObjectDistanceCond::get_object(CCJFlatComboBox& cb)
{
	const int cur_sel = cb.GetCurSel();
	if (cur_sel == CB_ERR)
		return NULL;
	return static_cast<qdGameObject*>(cb.GetItemDataPtr(cur_sel));
}

bool ObjectDistanceCond::IsCondDataComplete()
{
	if(!m_qdCondition.get_object(qdCondition::OBJECT_NAME))
		return false;

	if(!m_qdCondition.get_object(qdCondition::OBJECT2_NAME))
		return false;

	float fDist = 0.f;
	if(!m_qdCondition.get_value(qdCondition::OBJECTS_DISTANCE, fDist))
		return false;

	return true;
}

BOOL ObjectDistanceCond::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (GetFocus() == GetDlgItem(IDC_EDIT_DISTANCE))
			{
					if (UpdateData())
					{
						m_qdCondition.put_value(qdCondition::OBJECTS_DISTANCE, m_fDist);
						m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
					}
					return TRUE;
			}
		}
	}
	return CondDlgBase::PreTranslateMessage(pMsg);
}
