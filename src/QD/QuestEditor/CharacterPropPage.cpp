// CharacterGaitsPage.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"

#include "qd_game_object_state.h"
#include "qd_game_object_moving.h"
#include "qd_game_scene.h"
#include "qd_game_dispatcher.h"

#include "AddAnimStateWPg.h"
#include "WizardSheet.h"

#include "CharacterPropPage.h"
#include "editstatedlg.h"

//#include "scenetreeview.h"
#include "ptree_api.h"

//#include "StateCondDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CharacterGaitsPage property page

void ShowError(HWND hPar, DWORD dwMsgResID, DWORD dwCapResID = -1);
void ShowError(HWND hPar, LPCTSTR strMsg, DWORD dwCapResID = -1);
bool ShowCondDlg(qdGameObjectState* ptrState, qdGameDispatcher* ptrDisp);


IMPLEMENT_DYNCREATE(CharacterGaitsPage, CBasePage)

const INT DEFAULT_SPEED = 100;
CharacterGaitsPage::CharacterGaitsPage() : CBasePage(CharacterGaitsPage::IDD, IDS_PROPPAGE_CAPTION_GAITS)
{
	//{{AFX_DATA_INIT(CharacterGaitsPage)
	//}}AFX_DATA_INIT
	m_bInitDialog = true;
	m_ptrDisp = NULL;
	m_ptrObj = NULL;
}

CharacterGaitsPage::~CharacterGaitsPage()
{
}

//! Назначает диспетчер игры и объект
bool CharacterGaitsPage::set_data(qdGameDispatcher* ptrDisp, qdGameObjectMoving* ptrObj)
{
	m_ptrDisp	= ptrDisp;
	m_ptrObj	= ptrObj;
	return true;
}

void CharacterGaitsPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CharacterGaitsPage)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CharacterGaitsPage, CBasePage)
	//{{AFX_MSG_MAP(CharacterGaitsPage)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_ADD_STATE, OnAddState)
	ON_BN_CLICKED(IDC_CHANGE_STATE, OnChangeState)
	ON_BN_CLICKED(IDC_DEL_STATE, OnDelState)
	ON_BN_CLICKED(IDC_BTN_COND, OnBtnCond)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_LCE_ENDEDIT, IDC_LIST1, OnEndItemEditList1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CharacterGaitsPage message handlers

BOOL CharacterGaitsPage::OnInitDialog() 
{
	CBasePage::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	CString str;
	str.LoadString(IDS_CLMN_OBJ_STATE_NUMBER);
	m_list.InsertColumn(0,str, LVCFMT_LEFT, 80);
	
	str.LoadString(IDS_CLMN_OBJ_STATE_NAME);
	m_list.InsertColumn(1,str, LVCFMT_LEFT, 80, 1);
	m_list.SetEditCtrlType(1, ELC_ITEM_EDIT_CTRL_EDIT);
	
	str.LoadString(IDS_CLMN_ANIMATION_NAME);
	m_list.InsertColumn(2,str, LVCFMT_LEFT, 80, 2);
	
	str.LoadString(IDS_CLMN_SOUND_NAME);
	m_list.InsertColumn(3,str, LVCFMT_LEFT, 80, 3);
	
	str.LoadString(IDS_DLG_CURSOR_NAME);
	m_list.InsertColumn(4, str, LVCFMT_LEFT, 80, 4);
	
	LoadStates();
	
	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CharacterGaitsPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBasePage::OnShowWindow(bShow, nStatus);
	
	m_bInitDialog = false;
}


//! Вставляет походку в список
bool CharacterGaitsPage::AddState2List(int idx, qdGameObjectStateWalk* ptrState)
{
	CString str;
	str.Format("%d", idx);
	int iIndex = m_list.InsertItem(idx, str);
	ASSERT(iIndex != -1);

	UpdateState(iIndex, ptrState);
	m_list.SetItemData(iIndex, (DWORD)ptrState);
	return true;
}

//! Загружает список походок в контрол
bool CharacterGaitsPage::LoadStates()
{
	const qdGameObjectStateVector& lst = m_ptrObj->state_vector();
	qdGameObjectStateVector::const_iterator _itr = lst.begin(), 
		_end = lst.end();
	int iIndex = 0;
	for(; _itr != _end; ++_itr, ++iIndex)
	{
		if((*_itr)->state_type() == qdGameObjectState::STATE_WALK)
			AddState2List(iIndex, static_cast<qdGameObjectStateWalk*>(*_itr));
	}
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER );
	return true;
}

#include "name_op.h"
//! проверяет не повторяется ли имя. Если нет, то назначает его
bool CharacterGaitsPage::TestName(int iItem, int iSubItem, LPCTSTR lpszText)
{
	bool bResult = false;
	if(uniq_name(lpszText, m_ptrObj->state_vector()))
	{
		qdGameObjectStateWalk* pa = (qdGameObjectStateWalk*) m_list.GetItemData(iItem);
		pa->set_name(lpszText);
		bResult = true;
	}
	else
	{
		CString msg;
		msg.Format(IDS_NAME_REPEATED, lpszText);
		CString cap((LPTSTR)IDS_ERR_MSG_CAPTION);
		MessageBox(msg, cap, MB_OK|MB_ICONHAND);
	}
	return bResult;
}

void CharacterGaitsPage::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LVHITTESTINFO hi;
	GetCursorPos(&hi.pt);
	m_list.ScreenToClient(&hi.pt);
	int iIndex = ListView_SubItemHitTest(m_list.m_hWnd,&hi);
	
	if(iIndex != -1&&(hi.flags&LVHT_ONITEM)&&hi.iSubItem)
	{
		qdGameObjectStateWalk* ptrState = (qdGameObjectStateWalk*)m_list.GetItemData(iIndex);
		ASSERT(ptrState);
		bool bOk = false;
//		if(static_cast<bool>(GetAsyncKeyState(VK_SHIFT)>>15))
			bOk = ShowCondDlg(ptrState, m_ptrDisp);
//		else
//			bOk = ShowEditStateDlg(ptrState);
		
		if(bOk)
		{
			UpdateState(iIndex, ptrState);
			SetModified(TRUE);
			SetProjChange();
			AdjustColumnWidth();
		}
	}
	*pResult = 0;
}

void CharacterGaitsPage::OnEndItemEditList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LCE_INFO * ptrStr = (NM_LCE_INFO*)pNMHDR;
	
	*pResult = FALSE;
	if (TestName(ptrStr->item.iItem, ptrStr->item.iSubItem, ptrStr->item.pszText)) {
		*pResult = TRUE;
	}
}

BOOL CharacterGaitsPage::OnApply()
{
	return CBasePage::OnApply();
}

void CharacterGaitsPage::OnAddState() 
{
	AddAnimStateWPg pg;
	pg.set_data(m_ptrDisp, static_cast<qdGameScene*>(m_ptrObj->owner()));
	pg.set_obj(m_ptrObj);
	pg.set_place(PAGE_PLACE_FIRST_LAST);
	pg.set_obj_type(QD_NAMED_OBJECT_MOVING_OBJ);

	if (m_ptrObj->named_object_type() == QD_NAMED_OBJECT_MOVING_OBJ) {
		pg.set_caption(CString((LPTSTR)IDS_PROPPAGE_CAPTION_ADD_STATES_MOV_OBJ));
	}
	
	pg.set_use_movement();
	
#ifdef _AddAnimState_USE_PROP_PAGE
	CPropertyPage* pPage = &pg;
	WizardSheet ws(&pPage, 1, 0);
	
	ws.DoModal();
#else	
	pg.DoModal();
#endif
	if(pg.GetModified())
	{
		m_list.DeleteAllItems();
		LoadStates();
		AdjustColumnWidth();
		SetProjChange();
	}
	
}

//! Показывает диалог свойств состояния
bool CharacterGaitsPage::ShowEditStateDlg(qdGameObjectStateWalk* ptrState)
{
	EditStateDlg dlg;
	dlg.set_data(m_ptrDisp, m_ptrObj);
	dlg.set_state(ptrState);
	return (dlg.DoModal() == IDOK);
}

void CharacterGaitsPage::OnChangeState() 
{
	int iSelCount = m_list.GetSelectedCount();
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		qdGameObjectStateWalk* ptrState = (qdGameObjectStateWalk*)m_list.GetItemData(iIndex);
		ASSERT(ptrState);
		if(ShowEditStateDlg(ptrState))
		{
			UpdateState(iIndex, ptrState);
			SetProjChange();
			AdjustColumnWidth();
		}
	}
	else
	{
		ShowError(*this,IDS_NO_SELECTION_MSG, IDS_ERR_MSG_CAPTION);
	}
}

//! Для указанной походки заполняет текстом столбцы контрола 
void CharacterGaitsPage::UpdateState(int iIndex, qdGameObjectStateWalk* ptrState)
{
	m_list.SetItemText(iIndex, 1, ptrState->name());
	m_list.SetItemText(iIndex, 2, ptrState->animation_set_info()->name());
}

void CharacterGaitsPage::OnDelState() 
{
	int iSelCount = m_list.GetSelectedCount();
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	ASSERT(pos);
	CString msg;
	CString cap;
	cap.LoadString(IDS_DEL_CAP);
	while (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		CString str = m_list.GetItemText(iIndex, 0);
		msg.Format(IDS_DEL_STATE_MSG, (LPCTSTR)str);
		if(MessageBox(msg, cap, MB_OKCANCEL) == IDOK)
		{
			qdGameObjectStateWalk* pa = (qdGameObjectStateWalk*)m_list.GetItemData(iIndex);
			
			if(ptree_api::RemoveState(pa, true))
				delete pa;
			
			m_list.DeleteItem(iIndex);
			
			iIndex = min(iIndex, m_list.GetItemCount()-1);
			m_list.SetItemState(iIndex, LVIS_SELECTED|LVIS_FOCUSED,
				LVIS_SELECTED|LVIS_FOCUSED);
		}
	}
	AdjustColumnWidth();
}

//! Выравнивает ширину столбцов по тексту
void CharacterGaitsPage::AdjustColumnWidth()
{
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER );
	m_list.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER );
}

void CharacterGaitsPage::OnBtnCond() 
{
	int iSelCount = m_list.GetSelectedCount();
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos) {
		int iIndex = m_list.GetNextSelectedItem(pos);
		qdGameObjectStateWalk* ptrState = (qdGameObjectStateWalk*)m_list.GetItemData(iIndex);
		ASSERT(ptrState);
		if(ShowCondDlg(ptrState, m_ptrDisp))
		{
			UpdateState(iIndex, ptrState);
			SetModified(TRUE);
			SetProjChange();
			AdjustColumnWidth();
		}
	}
	else
	{
		ShowError(*this,IDS_NO_SELECTION_MSG, IDS_ERR_MSG_CAPTION);
	}
}
