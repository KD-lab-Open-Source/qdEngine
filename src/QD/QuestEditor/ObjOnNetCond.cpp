// ObjOnNetCond.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "ObjOnNetCond.h"

#include "qd_grid_zone.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ObjOnNetCond dialog

bool LoadObjects(CCJFlatComboBox& wndCombo, qdGameScene* ptrScene);
VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);


ObjOnNetCond::ObjOnNetCond(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(ObjOnNetCond::IDD, id, pParent)
{
	//{{AFX_DATA_INIT(ObjOnNetCond)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ObjOnNetCond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ObjOnNetCond)
	DDX_Control(pDX, IDC_COMBO_ZONE, m_wndZones);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndObjs);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SCENES, m_scenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_scenes_static);
}


BEGIN_MESSAGE_MAP(ObjOnNetCond, CDialog)
//{{AFX_MSG_MAP(ObjOnNetCond)
ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnSelchangeComboObject)
ON_CBN_SELCHANGE(IDC_COMBO_ZONE, OnSelchangeComboZone)
//}}AFX_MSG_MAP
ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ObjOnNetCond message handlers
bool ObjOnNetCond::IsCondDataComplete()
{
	if(!m_qdCondition.get_object(qdCondition::OBJECT_NAME))
		return false;
	if(!m_qdCondition.get_object(qdCondition::ZONE_NAME))
		return false;
	return true;
}

bool ObjOnNetCond::LoadScenes(){
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
	{
		m_scenes.SelectString(-1, pscene->name());
		m_scenes.ShowWindow(SW_HIDE);
		m_scenes_static.ShowWindow(SW_HIDE);
	}
	else{
		m_scenes_static.ShowWindow(SW_SHOW);
		m_scenes.ShowWindow(SW_SHOW);
		m_scenes.SetCurSel(0);
	}
	return true;
}

class qdGameScene* ObjOnNetCond::get_scene(){
	if (m_scenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes.GetItemDataPtr(m_scenes.GetCurSel()));
}

bool ObjOnNetCond::LoadObjects()
{
//	qdGameScene* ptrScene = m_ptrMakeCondDlg->get_scene();
	qdGameScene* ptrScene = get_scene();
	if(!ptrScene)
		return false;
	
	::LoadObjects(m_wndObjs, ptrScene);
	if (m_wndObjs.GetCount())
		m_wndObjs.SetCurSel(0);
	AdjustListWidth(&m_wndObjs);
	AdjustListWidth(&m_scenes);
	return true;
}

bool ObjOnNetCond::LoadZones()
{
	qdGameScene* ptrScene = get_scene();

	if(!ptrScene)
		return false;
	
	const qdGridZoneList& gol = ptrScene->grid_zone_list();
	qdGridZoneList::const_iterator _itr = gol.begin(), _end = gol.end();
	for(;_itr != _end; ++_itr)
	{
		int iPos = m_wndZones.AddString((*_itr)->name());
		if(iPos != -1)
			m_wndZones.SetItemDataPtr(iPos, *_itr);
	}
	if (m_wndZones.GetCount())
		m_wndZones.SetCurSel(0);
	AdjustListWidth(&m_wndZones);
	return true;
}

BOOL ObjOnNetCond::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LoadScenes();
	if(!LoadZones())
		return FALSE;
	if(!LoadObjects())
		return FALSE;
	
	return TRUE;  
}

void ObjOnNetCond::OnSelchangeComboObject() 
{
	int iSel = m_wndObjs.GetCurSel();
	if(iSel == -1)
		return;
	m_qdCondition.put_object(qdCondition::OBJECT_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndObjs.GetItemData(iSel)));
	
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjOnNetCond::OnSelchangeComboZone() 
{
	int iSel = m_wndZones.GetCurSel();
	if(iSel == -1)
		return;
//	DWORD dwData = m_wndZones.GetItemData(iSel);
//	if(dwData)
//	{
//		CString strText;
//		m_wndZones.GetLBText(iSel, strText);
//		m_qdCondition.put_value(qdCondition::ZONE_NAME, strText);
//	}
//	else
//		m_qdCondition.put_value(qdCondition::ZONE_NAME, "");
	m_qdCondition.put_object(qdCondition::ZONE_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndZones.GetItemDataPtr(iSel)));
	
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void ObjOnNetCond::OnCbnSelchangeComboScenes()
{
	LoadZones();
	LoadObjects();
}
