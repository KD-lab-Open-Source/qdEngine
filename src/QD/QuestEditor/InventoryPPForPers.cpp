// InventoryPPForPers.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "InventoryPPForPers.h"

#include "qd_game_object_moving.h"
#include "qd_game_dispatcher.h"

#include "ptree_hlprs.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InventoryPPForPers property page

IMPLEMENT_DYNCREATE(InventoryPPForPers, CBasePage)

InventoryPPForPers::InventoryPPForPers() : CBasePage(InventoryPPForPers::IDD)
, m_strSettedInventory(reinterpret_cast<LPTSTR>(IDS_NO_INVENTORY))
{
	//{{AFX_DATA_INIT(InventoryPPForPers)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bGlobalObj = false;
	m_bDisableAll = false;
}

InventoryPPForPers::~InventoryPPForPers()
{
}

void InventoryPPForPers::set_data(qdGameDispatcher* ptrDisp, 
								  qdGameObjectMoving* ptrObj)
{
	m_ptrObj = ptrObj;
	m_ptrGameDisp = ptrDisp;
}

void InventoryPPForPers::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InventoryPPForPers)
	DDX_Control(pDX, IDC_COMBO_INVENTORY, m_wndInventory);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_SETTED_INVENTORY, m_strSettedInventory);
}


BEGIN_MESSAGE_MAP(InventoryPPForPers, CBasePage)
	//{{AFX_MSG_MAP(InventoryPPForPers)
	ON_CBN_SELCHANGE(IDC_COMBO_INVENTORY, OnSelchangeComboInventory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InventoryPPForPers message handlers

VOID InventoryPPForPers::LoadInventoryList()
{
	const qdInventoryList& list = m_ptrGameDisp->inventory_list();
	typedef qdInventoryList::const_iterator ci_t;
	ci_t i = list.begin(), e = list.end();
	for(; i != e; ++i)
	{
		qdInventory* p = *i;
		int res = m_wndInventory.AddString(p->name());
		if (res != CB_ERR) 
			m_wndInventory.SetItemDataPtr(res, p);		
	}
	m_wndInventory.InsertString(0, CString((LPTSTR)IDS_NO_INVENTORY));
	m_wndInventory.SetCurSel(0);
}

VOID InventoryPPForPers::obj2wnd()
{
	LPCTSTR lpszInventoryName = m_ptrObj->inventory_name();
	if (lpszInventoryName)
	{
		int res = m_wndInventory.FindStringExact(-1, lpszInventoryName);
		if (res != CB_ERR)
			m_wndInventory.SetCurSel(res);
		m_strSettedInventory = lpszInventoryName;
	}
	else
		m_strSettedInventory.LoadString(IDS_NO_INVENTORY);
}

VOID InventoryPPForPers::wnd2obj()
{
	int iSelected = m_wndInventory.GetCurSel();
	ASSERT(iSelected != CB_ERR);

	CString str;
	if (iSelected) 
		m_wndInventory.GetLBText(iSelected, str);

	m_ptrObj->set_inventory_name(str);
	if (m_bGlobalObj) 
		SetInventoryToCopies(str);
	m_strSettedInventory = str;
	if (m_strSettedInventory.IsEmpty()) 
		m_strSettedInventory.LoadString(IDS_NO_INVENTORY);
}

BOOL InventoryPPForPers::OnApply() 
{
	wnd2obj();	
	return CBasePage::OnApply();
}

BOOL InventoryPPForPers::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	
	LoadInventoryList();
	obj2wnd();

	if (m_bDisableAll)
		DisableAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID InventoryPPForPers::OnSelchangeComboInventory() 
{
	SetModified();	
}

VOID InventoryPPForPers::DisableAll(){
	m_wndInventory.EnableWindow(FALSE);
}

VOID InventoryPPForPers::SetInventoryToCopies(const CString& strInvName){
	const qdGameSceneList& scenes = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator i = scenes.begin(), e = scenes.end();
	for(; i != e; ++i)
		SetInventoryToCopiesInScene(*i, strInvName);
}

VOID InventoryPPForPers::SetInventoryToCopiesInScene(qdGameScene* pscene, 
													 const CString& strInvName)
{
	qdGameObjectMoving* pobj = 
		static_cast<qdGameObjectMoving*>(pscene->get_object(m_ptrObj->name()));
	if (pobj) 
		pobj->set_inventory_name(strInvName);
}