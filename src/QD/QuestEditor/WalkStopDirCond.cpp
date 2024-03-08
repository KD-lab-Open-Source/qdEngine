// WalkStopDirCond.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "WalkStopDirCond.h"

#include "directionnamebyangle.h"

#include "qd_animation_set.h"
#include "qd_game_object.h"

#include <boost/function_output_iterator.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WalkStopDirCond dialog

VOID AdjustListWidth(CCJFlatComboBox* pmyComboBox);

class WalkStopDirCond::GaitInserter
{
public:
	GaitInserter(CCJFlatComboBox& box):
	  m_wndBox(box){}

	void operator()(qdAnimationSet* pset){
		int const iItem = m_wndBox.AddString(pset->name());
		if (iItem != CB_ERR)
			m_wndBox.SetItemDataPtr(iItem, pset);
	}
private:
	CCJFlatComboBox& m_wndBox;
};

WalkStopDirCond::WalkStopDirCond(DWORD id, CWnd* pParent /*=NULL*/)
	: CondDlgBase(WalkStopDirCond::IDD, id, pParent)
{
	//{{AFX_DATA_INIT(WalkStopDirCond)
	//}}AFX_DATA_INIT
	m_bInitDialog = true;
}


void WalkStopDirCond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WalkStopDirCond)
	DDX_Control(pDX, IDC_COMBO_DIR, m_wndComboDir);
	DDX_Control(pDX, IDC_COMBO_PERS, m_wndPers);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_SCENES, m_scenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_scenes_static);
	DDX_Control(pDX, IDC_COMBO_GAITS, m_wndGaits);
}


BEGIN_MESSAGE_MAP(WalkStopDirCond, CDialog)
	//{{AFX_MSG_MAP(WalkStopDirCond)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_PERS, OnSelchangeComboPers)
	ON_CBN_SELCHANGE(IDC_COMBO_DIR, OnSelchangeComboDir)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	ON_CBN_SELCHANGE(IDC_COMBO_GAITS, OnCbnSelchangeComboGaits)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WalkStopDirCond message handlers

void WalkStopDirCond::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	m_bInitDialog = false;
	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

bool WalkStopDirCond::IsCondDataComplete()
{
	if (!get_gait())
		return false;
//	if (!m_qdCondition.get_object(qdCondition::OBJECT_NAME)) {
//		return false;
//	}
	return true;
}

BOOL WalkStopDirCond::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LoadScenes();

	if(!LoadPers())
		return FALSE;

	if (!LoadGaits())
		return FALSE;

	if (!LoadDirs())
		return FALSE;
	OnSelchangeComboPers();
	OnSelchangeComboDir();
	AdjustListWidth(&m_wndPers);
	AdjustListWidth(&m_wndGaits);
	AdjustListWidth(&m_scenes);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int get_max_dirs(){
	qdGameDispatcher* ptrGD = qd_get_game_dispatcher();
	
	int imax = 0;
	const qdAnimationSetList& l = ptrGD->animation_set_list();
	typedef qdAnimationSetList::const_iterator ci_t;
	ci_t i = l.begin(), e = l.end();
	for(;i != e; ++i){
		if (imax < (*i)->size())
			imax = (*i)->size();
	}
	return imax;	
}
bool WalkStopDirCond::LoadDirs(){

	m_wndComboDir.ResetContent();

	qdAnimationSet const* pset = get_gait();
	if (!pset)
	{
		m_wndComboDir.EnableWindow(FALSE);
		return true;
	}

	int nDirCount = pset->size();
	if (nDirCount)
		m_wndComboDir.EnableWindow(TRUE);
	else
	{
		m_wndComboDir.EnableWindow(FALSE);
		return true;
	}

	CString strText;
	for(int i = 0; i < nDirCount; ++i){
		strText.Format(IDS_GAIT_ANGLE_FORMAT, 360.f/nDirCount * i);
		strText += DirectionNameByAngle::get(360.f/nDirCount * i);
		int pos = m_wndComboDir.AddString(strText);
		if (pos != CB_ERR)
			m_wndComboDir.SetItemData(pos, i);
	}

	m_wndComboDir.SetCurSel(0);
	return true;
}

struct is_moving {
	bool operator()(const qdGameObject* ptrGO) const {
		return (ptrGO->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ);
	}
};

bool WalkStopDirCond::LoadPers()
{
//	qdGameScene * ptrScene = m_ptrMakeCondDlg->get_scene();
	qdGameScene* ptrScene = get_scene();

	if(!ptrScene)
		return false;

	m_wndPers.ResetContent();
	const qdGameObjectList& gol = ptrScene->object_list();
	qdGameObjectList::const_iterator _itr = gol.begin(), _end = gol.end();
	while((_itr = std::find_if(_itr, _end, is_moving())) != _end)
	{
		int iPos = m_wndPers.AddString((*_itr)->name());
		if (iPos != -1) {
			m_wndPers.SetItemDataPtr(iPos, *_itr);
		}
		++_itr;
	}
	CString str;
	str.LoadString(IDS_STATE_OWNER);
	int iItem = m_wndPers.AddString(str);
	if (iItem != CB_ERR)
		m_wndPers.SetCurSel(iItem);

	AdjustListWidth(&m_wndPers);
	return true;
}

bool WalkStopDirCond::LoadScenes()
{
	qdGameDispatcher* ptrGD = qd_get_game_dispatcher();
	ASSERT(ptrGD);
	const qdGameSceneList& gsl = ptrGD->scene_list();
	if(gsl.empty())
	{
		m_scenes.SetCurSel(-1);
		m_scenes.EnableWindow(FALSE);
		return false;
	}

	m_scenes.ResetContent();
	qdGameSceneList::const_iterator itr = gsl.begin(), end = gsl.end();
	for(;itr != end; ++itr){
		int ires = m_scenes.AddString((*itr)->name());
		if (ires != CB_ERR)
			m_scenes.SetItemDataPtr(ires, *itr);
	}
	m_scenes_static.ShowWindow(SW_SHOW);
	m_scenes.ShowWindow(SW_SHOW);
	if (qdGameScene* const pscene = m_ptrMakeCondDlg->get_scene())
		m_scenes.SelectString(-1, pscene->name());
	else
		m_scenes.SetCurSel(0);
	
	return true;
}

class qdGameScene* WalkStopDirCond::get_scene(){
	if (m_scenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes.GetItemDataPtr(m_scenes.GetCurSel()));
}

class qdAnimationSet* WalkStopDirCond::get_gait()
{
	int const iCurSel = m_wndGaits.GetCurSel();
	if (iCurSel == CB_ERR)
		return NULL;
	return static_cast<qdAnimationSet*>(m_wndGaits.GetItemDataPtr(iCurSel));
};

void WalkStopDirCond::OnSelchangeComboPers() 
{
	int iSel = m_wndPers.GetCurSel();

	if(iSel == -1)
		return;
	
	m_qdCondition.put_object(qdCondition::OBJECT_NAME, 
		reinterpret_cast<qdNamedObject*>(m_wndPers.GetItemData(iSel)));
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void WalkStopDirCond::OnSelchangeComboDir() 
{
	int i = m_wndComboDir.GetCurSel();
	if (i == CB_ERR)
		return;

	qdAnimationSet* const pset = get_gait();
	if (!pset)
		return;
	m_qdCondition.put_value(qdCondition::DIRECTION_ANGLE, 
		pset->get_index_angle(i));
	
	m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void WalkStopDirCond::OnCbnSelchangeComboScenes()
{
	LoadPers();
//	LoadDirs();
}

void WalkStopDirCond::OnCbnSelchangeComboGaits()
{
	LoadDirs();
	OnSelchangeComboDir();
}

bool WalkStopDirCond::LoadGaits()
{
	ASSERT(get_scene());
	if (!get_scene())
		return false;

	qdGameDispatcher* const pdisp = static_cast<qdGameDispatcher*>(get_scene()->owner());
	qdAnimationSetList const& asl = pdisp->animation_set_list();
	std::copy(asl.begin(), asl.end(), 
		boost::make_function_output_iterator(GaitInserter(m_wndGaits)));
	if (asl.empty())
	{
		m_wndGaits.EnableWindow(FALSE);
	}
	else
		m_wndGaits.SetCurSel(0);
	return true;
}
