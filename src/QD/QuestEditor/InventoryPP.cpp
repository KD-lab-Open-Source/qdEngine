// InventoryPP.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "InventoryPP.h"

#include "qd_game_object_animated.h"
#include "qd_game_dispatcher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InventoryPP property page

IMPLEMENT_DYNCREATE(InventoryPP, CBasePage)

InventoryPP::InventoryPP() : CBasePage(InventoryPP::IDD)
, m_strType(_T(""))
{
	//{{AFX_DATA_INIT(InventoryPP)
	//}}AFX_DATA_INIT
}

InventoryPP::~InventoryPP()
{
}

VOID InventoryPP::set_data(qdGameDispatcher* ptrDisp, 
						   qdGameObjectAnimated* ptrAO)
{
	m_ptrObj = ptrAO;
	m_ptrGameDisp = ptrDisp;
}

void InventoryPP::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InventoryPP)
	DDX_Control(pDX, IDC_PIC_SEL, m_wndPicSelector);
	DDX_Control(pDX, IDC_COMBO_INVENTORY, m_wndInventory);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strType);
}


BEGIN_MESSAGE_MAP(InventoryPP, CBasePage)
	//{{AFX_MSG_MAP(InventoryPP)
	ON_CBN_SELCHANGE(IDC_COMBO_INVENTORY, OnSelchangeComboInventory)
	ON_NOTIFY(NM_SB_IMAGE_CHANGED, IDC_PIC_SEL, OnCellImageChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InventoryPP message handlers

BOOL InventoryPP::OnInitDialog() 
{
	CBasePage::OnInitDialog();
	
	// TODO: Add extra initialization here

	LoadInventoryList();
	LoadCellTypes();

	obj2wnd();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID InventoryPP::LoadInventoryList()
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
	m_wndInventory.InsertString(0, CString((LPTSTR)IDS_ACTIVE_INVENTORY));
	m_wndInventory.SetCurSel(0);
}

VOID InventoryPP::LoadCellTypes()
{
	 const qdInventoryCellTypeVector& v = m_ptrGameDisp->inventory_cell_types();
	 typedef qdInventoryCellTypeVector::const_iterator ci_t;
	 ci_t i = v.begin(), e = v.end();
	 for(; i != e; ++i)
	 {
		 const qdInventoryCellType& ct = *i;
		 m_wndPicSelector.AddImage(ct.sprite_file(), ct.type(), 
			 ImageType<IT_TGA_IMAGE>());
	 }
}

VOID InventoryPP::obj2wnd()
{
	int inventory_cell_type = m_ptrObj->inventory_type();
	UINT nCount = m_wndPicSelector.ImagesCount();
	for(UINT i = 0; i < nCount; ++i)
	{
		if (m_wndPicSelector.GetImageData(i) == inventory_cell_type)
		{
			m_wndPicSelector.SelectImage(i);
			break;
		}
	}
	setType(inventory_cell_type);
	m_wndInventory.SetCurSel(0);
	if (m_ptrObj->has_inventory_name())
		m_wndInventory.SelectString(0, m_ptrObj->inventory_name());
}

VOID InventoryPP::wnd2obj()
{
	int iSelectedImage = m_wndPicSelector.GetSelectedImage();
	if (iSelectedImage != -1)
	{
		m_ptrObj->set_inventory_type
			(m_wndPicSelector.GetImageData(iSelectedImage));
	}
	CString str;
	if (m_wndInventory.GetCurSel() != 0)//не выбран элемент с загловком "Активный"
	{
		m_wndInventory.GetLBText(m_wndInventory.GetCurSel(), str);
		m_ptrObj->set_inventory_name(str);
	}
	else
		m_ptrObj->set_inventory_name(NULL);
}

BOOL InventoryPP::OnApply() 
{
	wnd2obj();	
	return CBasePage::OnApply();
}

void InventoryPP::OnSelchangeComboInventory() 
{
	SetModified();	
}

void InventoryPP::OnCellImageChanged(NMHDR* phdr, LRESULT * pResult)
{
	SP_NOTIFY_INFO *pSpNotifyInfo = reinterpret_cast<SP_NOTIFY_INFO *>(phdr);
	if (pSpNotifyInfo)
		setType(pSpNotifyInfo->iNewItem);
	SetModified();
}
void InventoryPP::setType(int type)
{
	if (type != -1) 
		m_strType.Format(IDS_CELLSET_TYPE_FORMAT, type);
	else
		m_strType.LoadString(IDS_NO_CELL_TYPE_SELECTED);
	UpdateData(FALSE);
}
