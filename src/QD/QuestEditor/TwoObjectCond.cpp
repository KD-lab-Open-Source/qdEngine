// TwoObjectCond.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "TwoObjectCond.h"

#include "MakeCondDlgBase.h"
#include "qd_game_object_animated.h"
#include "qd_grid_zone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TwoObjectCond dialog
VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

/// Проверяет что объект не статический
struct not_static {
	bool operator()(const qdGameObject* ptrO)const{
		return (ptrO->named_object_type() != QD_NAMED_OBJECT_STATIC_OBJ);
	}
};

/// Проверяет что объект может быть в инвентори
struct has_inventory_flag{
	/// Является ли состояние состоянием для инвентори
	struct is_inventory_state{
		bool operator()(const qdGameObjectState* ptrState) const {
			return true;
		}
	};
	not_static _not_static;
	bool operator()(const qdGameObject* ptrO) const{
		if(_not_static(ptrO))
		{
			const qdGameObjectAnimated* ptrAO = 
				static_cast<const qdGameObjectAnimated*>(ptrO);
			const qdGameObjectStateVector& vec = ptrAO->state_vector();
			qdGameObjectStateVector::const_iterator _itr = 
				std::find_if(vec.begin(), vec.end(), is_inventory_state());
			if (_itr != vec.end()) {
				return true;
			}
		}
		return false;
	}
};


//qdCondition::CONDITION_MOUSE_OBJECT_CLICK
TwoObjectCond::TwoObjectCond(DWORD id, CWnd* pParent /*=NULL*/)
: CondDlgBase(TwoObjectCond::IDD, id, pParent)
{
	//{{AFX_DATA_INIT(TwoObjectCond)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	firstObjectStaticText_.LoadString(IDS_COND_DLG_MOUSE_OBJ_XXX_OBJECT_STATIC_1_CAP);
}

void TwoObjectCond::setSecondObjectStaticText(CString const& str){
	secondObjectStaticText_ = str;
}
void TwoObjectCond::setFirstObjectStaticText(CString const& str){
	firstObjectStaticText_ = str;
}

void TwoObjectCond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TwoObjectCond)
	DDX_Control(pDX, IDC_COMBO_OBJECT2, m_wndObj2);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndObj);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SCENES, m_scenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_scene_static);
	DDX_Control(pDX, IDC_COMBO_SCENES2, m_scenes2);
	DDX_Control(pDX, IDC_STATIC_SCENE2, m_scene_static2);
	DDX_Text(pDX, IDC_STATIC_2, secondObjectStaticText_);
	DDX_Text(pDX, IDC_STATIC_1, firstObjectStaticText_);
}


BEGIN_MESSAGE_MAP(TwoObjectCond, CDialog)
	//{{AFX_MSG_MAP(TwoObjectCond)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnSelchangeComboObject)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT2, OnSelchangeComboObject2)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES2, OnCbnSelchangeComboScenes2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TwoObjectCond message handlers

BOOL TwoObjectCond::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LoadScenes(m_scenes, m_scene_static);
	LoadScenes(m_scenes2, m_scene_static2);

	if(!LoadInventoryObjects())
		return FALSE;

	if(!LoadObjects())
		return FALSE;

	AdjustListWidth(&m_wndObj);
	AdjustListWidth(&m_wndObj2);
	AdjustListWidth(&m_scenes);
	AdjustListWidth(&m_scenes2);
	return TRUE;  
}

bool TwoObjectCond::IsCondDataComplete()
{
	return false;
}

void TwoObjectCond::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool TwoObjectCond::LoadInventoryObjects()
{
	qdGameScene* ptrScene = get_scene();
	if(!ptrScene)
		return false;
	m_wndObj.ResetContent();
	
	qdGameObjectList const& gol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	while((_itr = std::find_if(_itr, _end, has_inventory_flag())) != _end)
	{
		int iPos = m_wndObj.AddString((*_itr)->name());
		if(iPos != -1)
			//запихиваем TRUE, чтобы отследить потом, можно ли использовать
			//это имя
			m_wndObj.SetItemDataPtr(iPos, *_itr);
		++_itr;
	}		
	m_wndObj.SetCurSel(0);
	OnSelchangeComboObject();
	return true;
}

bool TwoObjectCond::LoadScenes(CCJFlatComboBox& box, CStatic& box_static){
	CondDlgBase::LoadScenes(box, box_static, SH_NONE);
	if (qdGameScene* pscene = m_ptrMakeCondDlg->get_scene())
		box.SelectString(-1, pscene->name());
	else{
		box_static.ShowWindow(SW_SHOW);
		box.ShowWindow(SW_SHOW);
		box.SetCurSel(0);
	}
	return true;
}

class qdGameScene* TwoObjectCond::get_scene(){
	if (m_scenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes.GetItemDataPtr(m_scenes.GetCurSel()));
}

class qdGameScene* TwoObjectCond::get_scene2(){
	if (m_scenes2.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes2.GetItemDataPtr(m_scenes2.GetCurSel()));
}

bool TwoObjectCond::LoadObjects()
{
	return false;
}

void TwoObjectCond::OnSelchangeComboObject() 
{
	int iSel = m_wndObj.GetCurSel();
	if(iSel == -1)
		return;
	m_qdCondition.put_object(qdCondition::MOUSE_OBJECT_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndObj.GetItemDataPtr(iSel)));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void TwoObjectCond::OnSelchangeComboObject2() 
{
}

void TwoObjectCond::OnCbnSelchangeComboScenes()
{
	LoadInventoryObjects();
	AdjustListWidth(&m_wndObj);
}

void TwoObjectCond::OnCbnSelchangeComboScenes2()
{
	LoadObjects();
	AdjustListWidth(&m_wndObj2);
}
