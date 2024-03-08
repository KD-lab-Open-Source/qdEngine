// OneObjectCond.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "OneObjectCond.h"
#include "qd_grid_zone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OneObjectCond dialog

bool LoadObjects(CCJFlatComboBox& wndCombo, qdGameScene* ptrScene);
void AdjustListWidth(CCJFlatComboBox* pmyComboBox);

OneObjectCond::OneObjectCond(DWORD id, CWnd* pParent /*=NULL*/)
: CondDlgBase(OneObjectCond::IDD, id, pParent)
{
	//{{AFX_DATA_INIT(OneObjectCond)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void OneObjectCond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OneObjectCond)
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_wndObj);
	DDX_Control(pDX, IDC_COMBO_SCENES, m_scenes);
	DDX_Control(pDX, IDC_STATIC_SCENE, m_scene_static);
	DDX_Text(pDX, IDC_STATIC_TEXT, objectComboStaticText_);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OneObjectCond, CDialog)
	//{{AFX_MSG_MAP(OneObjectCond)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, OnSelchangeComboObject)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_SCENES, OnCbnSelchangeComboScenes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OneObjectCond message handlers

void OneObjectCond::OnSelchangeComboObject() 
{
}

bool OneObjectCond::LoadScenes()
{
	return CondDlgBase::LoadScenes(m_scenes, m_scene_static, CondDlgBase::SH_SHOWHIDE);
}

class qdGameScene* OneObjectCond::get_scene(){
	if (m_scenes.GetCurSel() == CB_ERR)
		return NULL;
	return static_cast<qdGameScene*>(m_scenes.GetItemDataPtr(m_scenes.GetCurSel()));
}

BOOL OneObjectCond::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LoadScenes();
	LoadObjects();	

	AdjustListWidth(&m_scenes);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool OneObjectCond::IsCondDataComplete()
{
	return true;
}
void OneObjectCond::LoadObjects()
{
	qdGameScene* ptrScene = get_scene();
	if (ptrScene) 
	{
		::LoadObjects(m_wndObj, ptrScene);
		m_wndObj.SetCurSel(0);
	}
}

void OneObjectCond::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(bShow)
		m_ptrMakeCondDlg->OnCondDataComplete(IsCondDataComplete());
}

void OneObjectCond::OnCbnSelchangeComboScenes()
{
	LoadObjects();	
}	

void OneObjectCond::setObjectComboStaticText(CString str)
{
	objectComboStaticText_ = str;
//	UpdateData(FALSE);
}
