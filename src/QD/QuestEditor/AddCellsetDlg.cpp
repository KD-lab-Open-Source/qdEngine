// AddCellsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "AddCellsetDlg.h"

#include "qd_game_object_animated.h"
#include "qd_inventory.h"
#include "qd_game_dispatcher.h"

#include "staticobjpage.h"
#include "sv_api.h"
#include "ptree_api.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddCellsetDlg dialog

BOOL AddCellSet(qdGameDispatcher* ptrDisp, qdInventory* ptrInv)
{
	AddCellsetDlg dlg;
	dlg.set_data(ptrDisp, ptrInv);
	return dlg.DoModal() == IDOK;
}

AddCellsetDlg::AddCellsetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddCellsetDlg::IDD, pParent)
	, m_strType(_T(""))
{
	//{{AFX_DATA_INIT(AddCellsetDlg)
	m_iSX = 1;
	m_iSY = 1;
	//}}AFX_DATA_INIT
	m_bInitDialog = true;
}

VOID AddCellsetDlg::set_data(qdGameDispatcher* ptrDisp, qdInventory* ptrInv)
{
	m_ptrGameDisp = ptrDisp;
	m_ptrInventory = ptrInv;
}

void AddCellsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddCellsetDlg)
	DDX_Control(pDX, IDC_PIC_SELECTOR, m_wndPicSelector);
	DDX_Text(pDX, IDC_EDIT_SX, m_iSX);
	DDV_MinMaxInt(pDX, m_iSX, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_SY, m_iSY);
	DDV_MinMaxInt(pDX, m_iSY, 1, 1000);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_STATIC_TYPE, m_strType);
}


BEGIN_MESSAGE_MAP(AddCellsetDlg, CDialog)
	//{{AFX_MSG_MAP(AddCellsetDlg)
	ON_BN_CLICKED(IDC_BTN_ADD_CELLSET, OnBntAddCellset)
	ON_BN_CLICKED(IDC_BTN_ADD_TYPE, OnBtnAddType)
	ON_EN_CHANGE(IDC_EDIT_SX, OnChangeEditSx)
	ON_EN_CHANGE(IDC_EDIT_SY, OnChangeEditSx)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_SB_IMAGE_CHANGED, IDC_PIC_SELECTOR, OnImageChange)
	ON_BN_CLICKED(IDC_BTN_REMOVE_TYPE, OnBnClickedBtnRemoveType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddCellsetDlg message handlers

BOOL AddCellsetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SX))->SetRange(1, 1000);
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_SY))->SetRange(1, 1000);

	LoadCellTypes();

	m_bInitDialog = false;
	return TRUE;  
}

VOID AddCellsetDlg::LoadCellTypes()
{
	const qdInventoryCellTypeVector& v = 
		m_ptrGameDisp->inventory_cell_types();
	typedef qdInventoryCellTypeVector::const_iterator ci_t;
	ci_t i = v.begin(), e = v.end();
	for(; i != e; ++i){
		const qdInventoryCellType& ct = *i;
		m_wndPicSelector.AddImage(ct.sprite_file(), ct.type(), 
			ImageType<IT_TGA_IMAGE>());
	}
	if (m_wndPicSelector.ImagesCount()) 
		m_wndPicSelector.SelectImage(0);
	else
		m_wndPicSelector.EnableWindow(FALSE);
}

void AddCellsetDlg::OnOK() 
{
	if(!UpdateData())
		return;

	if(GetDlgItem(IDC_BTN_ADD_CELLSET)->IsWindowEnabled()
		&&!AddCellset())
		return;
	
	CDialog::OnOK();
}

BOOL AddCellsetDlg::AddCellset()
{
	int iSelImage = m_wndPicSelector.GetSelectedImage();
	if (iSelImage == -1)
	{
		AfxMessageBox(IDS_NO_CELL_TYPE_SELECTED);
		return FALSE;
	}
	const qdInventoryCellType* pict = m_ptrGameDisp->
		get_inventory_cell_type(m_wndPicSelector.GetImageData(iSelImage));
	ASSERT(pict);
	
	Vect2s cell_size(pict->size());
	cell_size.x >>= 1;
	cell_size.y >>= 1;
	
	return (m_ptrInventory->add_cell_set(cell_size, Vect2s(m_iSX, m_iSY), *pict) != 0);
}

void AddCellsetDlg::OnBntAddCellset() 
{
	if(!UpdateData())
		return;
	
	if(AddCellset())
	{
		SetProjChange();
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
}

void AddCellsetDlg::OnBtnAddType() 
{
	CStringVect_t vecFiles;
	CString strOpenDlgTitle((LPTSTR)IDS_OSD_CAP_SEL_CELL_TYPE);
	int nCountAdded = 0;
	
	while (true) {
		vecFiles.clear();
		//выбираем картинку
		CString picture = StaticObjPage::SelectPicture(this->m_hWnd, 
														strOpenDlgTitle, &vecFiles);
		if(picture.IsEmpty())
			break;
		
		CStringVect_t::iterator _itr = vecFiles.begin(), 
			_end = vecFiles.end();
		for(; _itr != _end; ++_itr)
		{
			qdInventoryCellType ct(m_ptrGameDisp->get_unique_inventory_cell_type());
			
			ct.set_sprite_file(*_itr);
			if(const qdInventoryCellType* pct = 
				m_ptrGameDisp->add_inventory_cell_type(ct))
			{
				pct->load_resources();
				++nCountAdded;
				if(m_wndPicSelector.AddImage(*_itr, pct->type(), 
									ImageType<IT_TGA_IMAGE>()))
				{
					if (m_wndPicSelector.GetSelectedImage() == -1)
					{
						m_wndPicSelector.SelectImage(0);
						m_wndPicSelector.EnableWindow(TRUE);
					}
				}
			}
			SetProjChange();
		}
		CString strMsg;
		strMsg.LoadString(IDS_CELL_TYPES_ADDED_SUCCESSFULL);
		if (MessageBox(strMsg, NULL, MB_ICONINFORMATION|MB_YESNO|MB_DEFBUTTON2) == IDNO) 
			break;
	}
}

void AddCellsetDlg::OnChangeEditSx() 
{
	if (m_bInitDialog)
		return;
	if(!UpdateData())
		return;
	if (!m_ptrGameDisp->inventory_list().empty()) 
		GetDlgItem(IDC_BTN_ADD_CELLSET)->EnableWindow(TRUE);
}

VOID AddCellsetDlg::OnImageChange(NMHDR * pNotifyStruct, LRESULT * result)
{
	SP_NOTIFY_INFO * pni = reinterpret_cast<SP_NOTIFY_INFO*>(pNotifyStruct);
	if (pni->iNewItem == static_cast<UINT>(-1)) 
	{
		GetDlgItem(IDC_BTN_REMOVE_TYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_ADD_CELLSET)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BTN_REMOVE_TYPE)->EnableWindow(TRUE);
		if (!m_ptrGameDisp->inventory_list().empty()) 
			GetDlgItem(IDC_BTN_ADD_CELLSET)->EnableWindow(TRUE);
	}
	setType(pni->iNewItem);
	UpdateData(FALSE);
}

void AddCellsetDlg::OnBnClickedBtnRemoveType()
{
	if (MessageBox(CString((LPTSTR)IDS_DEL_CELL_TYPE_MSG),
					CString((LPTSTR)IDS_DEL_CAP), MB_OKCANCEL) != IDOK)
		return;
	int iCurImg = m_wndPicSelector.GetSelectedImage();
	DWORD dwImageData = m_wndPicSelector.GetImageData(iCurImg);

	if (remove_cell_type(static_cast<int>(dwImageData))) 
	{
		int res =m_wndPicSelector.RemoveImage(iCurImg);
		SetProjChange();
		if (res == -1) 
		{
			m_wndPicSelector.EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_REMOVE_TYPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_ADD_CELLSET)->EnableWindow(FALSE);
		}
		sv_api::Redraw(sv_api::RM_REDRAW_CONTENT);
	}
}

bool AddCellsetDlg::remove_cell_type(int type){
	bool bremove_all_invs = false;
	if (m_ptrGameDisp->inventory_cell_types().size() == 1
		&&!m_ptrGameDisp->inventory_list().empty()) 
	{
		if (MessageBox(CString((LPTSTR)IDS_REMOVE_LAST_CELL_TYPE),
			CString((LPTSTR)IDS_ERR_MSG_CAPTION), MB_OKCANCEL) != IDOK)
			return false;
		bremove_all_invs = true;
	}

	const qdInventoryCellType* pt = m_ptrGameDisp->get_inventory_cell_type(type);
	ASSERT (pt); 

	int ins_type = m_ptrGameDisp->inventory_cell_types().front().type();
	if (ins_type == type)
	{
		qdInventoryCellTypeVector::const_iterator g = 
			m_ptrGameDisp->inventory_cell_types().begin();
		++g;
		if (g == m_ptrGameDisp->inventory_cell_types().end()) 
			ins_type = 0;
		else
			//тогда берем следующий
			ins_type = g->type();
	}

	replace_cell_type_in_objects(type, ins_type);
	if (bremove_all_invs) //если все удалим, зачем что-то менять
		remove_all_invs();
	else
		replace_cell_type_in_cellsets(type, ins_type);

	return m_ptrGameDisp->remove_inventory_cell_type(type);
}

bool AddCellsetDlg::replace_cell_type_in_cellsets(int type_remove, int type_insert){
	const qdInventoryList& l = m_ptrGameDisp->inventory_list();
	const qdInventoryCellType*pct_ins = 
		m_ptrGameDisp->get_inventory_cell_type(type_insert);
	ASSERT(pct_ins);

	qdInventoryList::const_iterator i = l.begin(), e = l.end();
	for(;i != e; ++i){
		qdInventory* p = *i;
		const qdInventoryCellSetVector&v = p->cell_sets();
		qdInventoryCellSetVector::const_iterator itr = v.begin(), end = v.end();
		for(; itr != end; ++itr){
			qdInventoryCellSet& s = const_cast<qdInventoryCellSet&>(*itr);
			if (s.cells().front().type() == type_remove)
				s.set_cell_type(*pct_ins);
		}
	}
	return true;
}

bool AddCellsetDlg::replace_cell_type_in_objects(int type, int ins_type){
	const qdGameSceneList& sl = m_ptrGameDisp->scene_list();
	qdGameSceneList::const_iterator i = sl.begin(), e = sl.end();
	for(; i != e; ++i){
		const qdGameObjectList& ol = (*i)->object_list();
		qdGameObjectList::const_iterator itr = ol.begin(), end = ol.end();
		for(; itr != end; ++itr){
			qdGameObject*po = *itr;
			if (po->named_object_type() == QD_NAMED_OBJECT_ANIMATED_OBJ) 
			{
				qdGameObjectAnimated* pao = static_cast<qdGameObjectAnimated*>(po);
				if (pao->inventory_type() == type) 
					pao->set_inventory_type(ins_type);
			}
		}
	}
	return true;
}

bool AddCellsetDlg::remove_all_invs(){
	const qdInventoryList& l = m_ptrGameDisp->inventory_list();
	qdInventoryList::const_iterator i = l.begin(), e = l.end();
	for(; i != e; )
		ptree_api::DeleteObject(*i++, false);
	return true;
}

void AddCellsetDlg::setType(int type)
{
	if (type == -1)
		m_strType.LoadString(IDS_NO_CELL_TYPE_SELECTED);
	else
		m_strType.Format(IDS_CELLSET_TYPE_FORMAT, type);
}
